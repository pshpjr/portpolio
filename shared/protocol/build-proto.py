#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_SERVER_OUT = SCRIPT_DIR.parent / "generated" / "server"
DEFAULT_UNREAL_OUT = SCRIPT_DIR.parent / "generated" / "unreal"


@dataclass(frozen=True)
class HeaderRenderModel:
    prefix: str
    include_lines: list[str]
    msg_names: list[str]
    max_len: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate shared packet code from .proto files")
    parser.add_argument("--schema-dir", type=Path, default=SCRIPT_DIR)
    parser.add_argument("--server-out", type=Path, default=DEFAULT_SERVER_OUT)
    parser.add_argument("--unreal-out", type=Path, default=DEFAULT_UNREAL_OUT)
    parser.add_argument("--protoc", type=Path, default=None)
    return parser.parse_args()


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def clean_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def write_utf8_no_bom(path: Path, content: str) -> None:
    path.write_text(content, encoding="utf-8", newline="\n")


def remove_block_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)


def remove_line_comment(line: str) -> str:
    return re.sub(r"//.*$", "", line)


def get_proto_messages_by_prefix(prefix: str, proto_root: Path) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []

    proto_files = sorted(proto_root.glob(f"{prefix}.*.proto"), key=lambda p: p.name)
    for proto_file in proto_files:
        raw = remove_block_comments(proto_file.read_text(encoding="utf-8"))
        for line in raw.splitlines():
            clean = remove_line_comment(line)
            match = re.match(r"^\s*message\s+([A-Za-z_][A-Za-z0-9_]*)", clean)
            if not match:
                continue

            name = match.group(1)
            if name not in seen:
                seen.add(name)
                ordered.append(name)

    return ordered


def resolve_protoc(schema_dir: Path, explicit: Path | None) -> Path | None:
    candidates: list[Path] = []
    if explicit is not None:
        candidates.append(explicit)

    candidates.extend(
        [
            schema_dir / "protoc.exe",
            SCRIPT_DIR / "protoc.exe",
        ]
    )

    for candidate in candidates:
        resolved = candidate.resolve()
        if resolved.exists():
            return resolved

    return None


def compile_proto_files(proto_root: Path, out_dir: Path, protoc: Path) -> None:
    proto_files = sorted(proto_root.glob("*.proto"), key=lambda p: p.name)
    if not proto_files:
        print("No .proto files found. Skipping protobuf compilation.")
        return

    print("== Protobuf generation start ==")
    for index, proto_file in enumerate(proto_files, start=1):
        print(f"{index}. {proto_file.name} ...")
        result = subprocess.run(
            [
                str(protoc),
                f"--proto_path={proto_root}",
                f"--cpp_out={out_dir}",
                proto_file.name,
            ],
            cwd=proto_root,
            text=True,
            capture_output=True,
        )

        if result.returncode != 0:
            if result.stdout:
                print(result.stdout)
            if result.stderr:
                print(result.stderr, file=sys.stderr)
            raise RuntimeError(f"protoc failed for {proto_file.name}")

        print(f"   * {proto_file.name} : OK")


def distribute_generated_files(server_proto_dir: Path, unreal_proto_dir: Path) -> None:
    ensure_dir(unreal_proto_dir)
    for generated in sorted(server_proto_dir.glob("*.pb.*"), key=lambda p: p.name):
        shutil.copy2(generated, unreal_proto_dir / generated.name)


def collect_header_groups(pb_root: Path) -> dict[str, list[Path]]:
    groups: dict[str, list[Path]] = {}
    for header in sorted(pb_root.glob("*.pb.h"), key=lambda p: p.name):
        stem_parts = header.stem.split(".")
        if len(stem_parts) < 3:
            continue
        prefix = stem_parts[0]
        groups.setdefault(prefix, []).append(header)
    return groups


def make_kpacket_specialization_left(prefix: str, msg_name: str) -> str:
    return f"template <> inline constexpr PACKET_TYPE kPacketType<{prefix}::{msg_name}>"


def build_render_model(prefix: str, headers: list[Path], proto_root: Path) -> HeaderRenderModel:
    include_lines = [f'#include "Protobuf/{header.name}"' for header in headers]
    msg_names = [
        name
        for name in get_proto_messages_by_prefix(prefix=prefix, proto_root=proto_root)
        if re.match(r"^(Rq|Rs)[A-Za-z0-9_]*$", name, flags=re.IGNORECASE)
    ]

    left_default = "template <typename T> inline constexpr PACKET_TYPE kPacketType"
    max_len = max(
        [len(left_default)] + [len(make_kpacket_specialization_left(prefix, name)) for name in msg_names],
        default=len(left_default),
    )

    return HeaderRenderModel(
        prefix=prefix,
        include_lines=include_lines,
        msg_names=msg_names,
        max_len=max_len,
    )


def render_header(model: HeaderRenderModel) -> str:
    include_block = "\n".join(model.include_lines)

    enum_lines = "\n".join(f"    {msg_name}," for msg_name in model.msg_names)

    specializations = "\n".join(
        f"{make_kpacket_specialization_left(model.prefix, msg_name).ljust(model.max_len)} = "
        f"PACKET_TYPE::{msg_name};"
        for msg_name in model.msg_names
    )

    lines = [
        "// Auto-generated. Do not edit.",
        "#pragma once",
        "",
        "#include <concepts>",
        "#include <type_traits>",
        "",
    ]

    if include_block:
        lines.append(include_block)
        lines.append("")

    lines.extend(
        [
            f"namespace {model.prefix} {{",
            "enum class PACKET_TYPE",
            "{",
            "    NONE,",
        ]
    )

    if enum_lines:
        lines.append(enum_lines)

    lines.extend(
        [
            "};",
            "",
            f'{"template <typename T> inline constexpr PACKET_TYPE kPacketType".ljust(model.max_len)} = PACKET_TYPE::NONE;',
        ]
    )

    if specializations:
        lines.append(specializations)

    lines.extend(
        [
            f"}} // namespace {model.prefix}",
            "",
            "",
            "// Concepts",
            "template <class T>",
            f"concept Is{model.prefix} = ({model.prefix}::kPacketType<std::remove_cvref_t<T>> != {model.prefix}::PACKET_TYPE::NONE);",
            "",
            "template <class T>",
            f"inline constexpr bool exactly_{model.prefix} = Is{model.prefix}<T>;",
            "",
            "",
            "// Traits",
            f"struct {model.prefix}_Traits",
            "{",
            f"    using PacketType = {model.prefix}::PACKET_TYPE;",
            "",
            "    template <typename T>",
            "    static consteval PacketType GetType()",
            "    {",
            "        using Msg = std::remove_cvref_t<T>;",
            f"        return {model.prefix}::kPacketType<Msg>;",
            "    }",
            "};",
            "",
        ]
    )

    return "\n".join(lines)


def generate_aggregated_headers(packet_root: Path, proto_root: Path) -> None:
    pb_root = packet_root / "Protobuf"
    for old_header in packet_root.glob("*.header.h"):
        old_header.unlink(missing_ok=True)

    groups = collect_header_groups(pb_root)
    if not groups:
        return

    for prefix in sorted(groups.keys()):
        headers = sorted(groups[prefix], key=lambda p: p.name)
        model = build_render_model(prefix=prefix, headers=headers, proto_root=proto_root)
        rendered = render_header(model)
        out_path = packet_root / f"{prefix}.header.h"
        write_utf8_no_bom(out_path, rendered)
        print(f"Generated {out_path.name}")


def main() -> int:
    args = parse_args()

    schema_dir = args.schema_dir.resolve()
    server_out = args.server_out.resolve()
    unreal_out = args.unreal_out.resolve()

    server_proto_dir = server_out / "Protobuf"
    unreal_proto_dir = unreal_out / "Protobuf"

    clean_dir(server_out)
    clean_dir(unreal_out)
    ensure_dir(server_proto_dir)
    ensure_dir(unreal_proto_dir)

    proto_files = sorted(schema_dir.glob("*.proto"), key=lambda p: p.name)
    if proto_files:
        protoc = resolve_protoc(schema_dir=schema_dir, explicit=args.protoc)
        if protoc is None:
            raise FileNotFoundError(
                "protoc executable not found. Pass --protoc or place protoc.exe under shared/protocol."
            )
        compile_proto_files(proto_root=schema_dir, out_dir=server_proto_dir, protoc=protoc)
        distribute_generated_files(server_proto_dir=server_proto_dir, unreal_proto_dir=unreal_proto_dir)
    else:
        print("No protocol schema files found. Created output directories only.")

    generate_aggregated_headers(packet_root=server_out, proto_root=schema_dir)
    generate_aggregated_headers(packet_root=unreal_out, proto_root=schema_dir)

    print("== Done ==")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
