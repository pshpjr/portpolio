#!/usr/bin/env python3
"""Generate game-data DTOs, enums, and loaders from schema/data JSON pairs.

Server + Unreal generator. Uses Jinja2 templates for C++ code emission.
Server enum parsing relies on magic_enum (no manual TryParse).
Unreal enum parsing uses generated TryParse functions.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from jinja2 import Environment, FileSystemLoader

from env_utils import REPO_ROOT, load_dotenv

load_dotenv(REPO_ROOT / ".env")

SCRIPT_DIR = Path(__file__).resolve().parent
TEMPLATE_DIR = SCRIPT_DIR / "templates"
DEFAULT_SCHEMA_DIR = REPO_ROOT / "docs" / "proposal" / "ingame" / "data"
DEFAULT_SERVER_OUT = Path(
    os.environ.get("PORTPOLIO_SERVER_OUT", str(REPO_ROOT / "server" / "src" / "generated"))
)
DEFAULT_UNREAL_OUT = Path(
    os.environ.get("PORTPOLIO_UNREAL_OUT", str(REPO_ROOT / "client" / "Source" / "client" / "Public" / "Generated"))
)
OUTPUT_SUBDIR = "game_data"


# ---------------------------------------------------------------------------
# Data model (unchanged from combat_data_gen.py)
# ---------------------------------------------------------------------------

@dataclass(frozen=True)
class EnumDefinition:
    name: str
    values: tuple[str, ...]


@dataclass(frozen=True)
class FieldDefinition:
    name: str
    kind: str
    key: bool
    required: bool
    enum_name: str | None
    json_name: str
    server_type: str | None
    unreal_type: str | None
    default: Any | None


@dataclass(frozen=True)
class TableDefinition:
    source_stem: str
    schema_path: Path
    data_path: Path
    table_name: str
    server_struct: str
    unreal_struct: str
    fields: tuple[FieldDefinition, ...]
    enums: tuple[EnumDefinition, ...]
    row_count: int


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def pascal_case(text: str) -> str:
    parts = re.split(r"[^A-Za-z0-9]+", text)
    return "".join(part[:1].upper() + part[1:] for part in parts if part)


def snake_case(text: str) -> str:
    text = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", text)
    text = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", text)
    text = re.sub(r"[^A-Za-z0-9]+", "_", text)
    text = re.sub(r"_+", "_", text).strip("_")
    return text.lower()


def sanitize_identifier(text: str) -> str:
    name = re.sub(r"[^A-Za-z0-9_]", "_", text)
    if not name:
        return "_"
    if name[0].isdigit():
        name = f"_{name}"
    return name


def sanitize_enum_value(text: str) -> str:
    name = sanitize_identifier(text)
    if name and name[0].isdigit():
        name = f"_{name}"
    return name


def read_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")


# ---------------------------------------------------------------------------
# Type mapping
# ---------------------------------------------------------------------------

_SERVER_TYPE_MAP: dict[str, str] = {
    "int32": "std::int32_t",
    "int64": "std::int64_t",
    "float": "float",
    "double": "double",
    "bool": "bool",
    "string": "std::string",
    "name": "std::string",
    "text": "std::string",
}

_UNREAL_TYPE_MAP: dict[str, str] = {
    "int32": "int32",
    "int64": "int64",
    "float": "float",
    "double": "double",
    "bool": "bool",
    "string": "FString",
    "name": "FName",
    "text": "FText",
}


def normalize_enum_name(name: str, runtime: str) -> str:
    clean = sanitize_identifier(name)
    if runtime == "unreal" and not clean.startswith("E"):
        return f"E{clean}"
    return clean


def server_cpp_type(field: FieldDefinition) -> str:
    if field.server_type:
        return field.server_type
    if field.kind == "enum":
        if not field.enum_name:
            raise ValueError(f"enum field '{field.name}' is missing enum name")
        return field.enum_name
    if field.kind not in _SERVER_TYPE_MAP:
        raise ValueError(f"unsupported field type '{field.kind}' for field '{field.name}'")
    return _SERVER_TYPE_MAP[field.kind]


def unreal_cpp_type(field: FieldDefinition) -> str:
    if field.unreal_type:
        return field.unreal_type
    if field.kind == "enum":
        if not field.enum_name:
            raise ValueError(f"enum field '{field.name}' is missing enum name")
        return normalize_enum_name(field.enum_name, "unreal")
    if field.kind not in _UNREAL_TYPE_MAP:
        raise ValueError(f"unsupported field type '{field.kind}' for field '{field.name}'")
    return _UNREAL_TYPE_MAP[field.kind]


# ---------------------------------------------------------------------------
# Schema parsing (reused from combat_data_gen.py)
# ---------------------------------------------------------------------------

def normalize_table_name(source_stem: str, schema: dict[str, Any]) -> str:
    raw = schema.get("table_name") or schema.get("table") or schema.get("name")
    if isinstance(raw, str) and raw.strip():
        return sanitize_identifier(raw.strip())
    return pascal_case(source_stem)


def normalize_struct_name(table_name: str, schema: dict[str, Any], runtime: str) -> str:
    key = f"{runtime}_struct"
    raw = schema.get(key) or schema.get("struct") or schema.get("row_struct")
    if isinstance(raw, str) and raw.strip():
        return sanitize_identifier(raw.strip())
    if runtime == "unreal":
        return f"F{table_name}Row"
    return f"{table_name}Row"


def normalize_fields(schema: dict[str, Any]) -> tuple[FieldDefinition, ...]:
    raw_fields = schema.get("fields")
    if not isinstance(raw_fields, list) or not raw_fields:
        raise ValueError("schema must define a non-empty 'fields' array")

    raw_primary_keys = schema.get("primary_key", [])
    primary_keys: set[str] = set()
    if raw_primary_keys is None:
        raw_primary_keys = []
    if not isinstance(raw_primary_keys, list):
        raise ValueError("'primary_key' must be an array when present")
    for raw_key in raw_primary_keys:
        if not isinstance(raw_key, str) or not raw_key.strip():
            raise ValueError("primary key entries must be non-empty strings")
        primary_keys.add(sanitize_identifier(raw_key.strip()))

    fields: list[FieldDefinition] = []
    seen: set[str] = set()
    for raw_field in raw_fields:
        if not isinstance(raw_field, dict):
            raise ValueError("each field entry must be an object")

        name = raw_field.get("name")
        if not isinstance(name, str) or not name.strip():
            raise ValueError("field is missing a non-empty 'name'")
        name = sanitize_identifier(name.strip())
        if name in seen:
            raise ValueError(f"duplicate field name: {name}")
        seen.add(name)

        kind = raw_field.get("type") or raw_field.get("kind")
        if not isinstance(kind, str) or not kind.strip():
            raise ValueError(f"field '{name}' is missing 'type'")
        kind = kind.strip().lower()

        enum_name = raw_field.get("enum_name") or raw_field.get("enum")
        if enum_name is not None and not isinstance(enum_name, str):
            raise ValueError(f"field '{name}' enum name must be a string")

        json_name = raw_field.get("json_name") or raw_field.get("source_name") or name
        if not isinstance(json_name, str) or not json_name.strip():
            raise ValueError(f"field '{name}' json name must be a string")

        fields.append(
            FieldDefinition(
                name=name,
                kind=kind,
                key=bool(raw_field.get("key", False)) or name in primary_keys,
                required=bool(raw_field.get("required", True)),
                enum_name=sanitize_identifier(enum_name.strip()) if isinstance(enum_name, str) and enum_name.strip() else None,
                json_name=sanitize_identifier(json_name.strip()),
                server_type=raw_field.get("server_type") if isinstance(raw_field.get("server_type"), str) else None,
                unreal_type=raw_field.get("unreal_type") if isinstance(raw_field.get("unreal_type"), str) else None,
                default=raw_field.get("default"),
            )
        )

    return tuple(fields)


def normalize_enums(schema: dict[str, Any]) -> tuple[EnumDefinition, ...]:
    raw_enums = schema.get("enums")
    if raw_enums is None:
        return ()

    enums: dict[str, tuple[str, ...]] = {}

    if isinstance(raw_enums, dict):
        items = list(raw_enums.items())
    elif isinstance(raw_enums, list):
        items = []
        for raw_enum in raw_enums:
            if not isinstance(raw_enum, dict):
                raise ValueError("enum entries must be objects")
            enum_name = raw_enum.get("name") or raw_enum.get("enum_name")
            values = raw_enum.get("values") or raw_enum.get("enumerators")
            if not isinstance(enum_name, str) or not enum_name.strip():
                raise ValueError("enum entry is missing a name")
            if not isinstance(values, list) or not values:
                raise ValueError(f"enum '{enum_name}' must define a non-empty value list")
            items.append((enum_name, values))
    else:
        raise ValueError("'enums' must be either a map or a list")

    for raw_name, raw_values in items:
        if not isinstance(raw_name, str) or not raw_name.strip():
            raise ValueError("enum names must be non-empty strings")
        enum_name = sanitize_identifier(raw_name.strip())
        if not isinstance(raw_values, list) or not raw_values:
            raise ValueError(f"enum '{enum_name}' must define a non-empty value list")

        values: list[str] = []
        for raw_value in raw_values:
            if not isinstance(raw_value, str) or not raw_value.strip():
                raise ValueError(f"enum '{enum_name}' contains an invalid value")
            values.append(sanitize_enum_value(raw_value.strip()))

        if enum_name in enums and enums[enum_name] != tuple(values):
            raise ValueError(f"enum '{enum_name}' is defined more than once with different values")
        enums[enum_name] = tuple(values)

    return tuple(EnumDefinition(name=name, values=values) for name, values in sorted(enums.items()))


def validate_rows(
    schema_path: Path,
    data_path: Path,
    rows: list[dict[str, Any]],
    fields: tuple[FieldDefinition, ...],
) -> None:
    if not rows:
        return

    allowed = {field.json_name for field in fields}
    for index, row in enumerate(rows, start=1):
        if not isinstance(row, dict):
            raise ValueError(f"row {index} in {data_path} must be an object")

        row_keys = {sanitize_identifier(str(key)) for key in row.keys()}
        missing = [field.json_name for field in fields if field.required and field.json_name not in row_keys]
        if missing:
            raise ValueError(f"row {index} in {data_path} is missing required fields: {', '.join(missing)}")

        extra = sorted(row_keys - allowed)
        if extra:
            raise ValueError(f"row {index} in {data_path} contains unknown fields: {', '.join(extra)}")


def load_table_definition(schema_path: Path, data_path: Path) -> TableDefinition:
    schema_raw = read_json(schema_path)
    if not isinstance(schema_raw, dict):
        raise ValueError(f"schema file must contain an object: {schema_path}")

    data_raw = read_json(data_path)
    rows: list[dict[str, Any]]
    if isinstance(data_raw, list):
        rows = data_raw
    elif isinstance(data_raw, dict):
        candidate = data_raw.get("rows") or data_raw.get("data")
        if not isinstance(candidate, list):
            raise ValueError(f"data file must contain either an array or a 'rows'/'data' array: {data_path}")
        rows = candidate
    else:
        raise ValueError(f"data file must contain an array or object: {data_path}")

    source_stem = schema_path.name.removesuffix(".schema.json")
    table_name = normalize_table_name(source_stem, schema_raw)
    fields = normalize_fields(schema_raw)
    enums = normalize_enums(schema_raw)
    server_struct = normalize_struct_name(table_name, schema_raw, "server")
    unreal_struct = normalize_struct_name(table_name, schema_raw, "unreal")
    validate_rows(schema_path=schema_path, data_path=data_path, rows=rows, fields=fields)

    return TableDefinition(
        source_stem=source_stem,
        schema_path=schema_path,
        data_path=data_path,
        table_name=table_name,
        server_struct=server_struct,
        unreal_struct=unreal_struct,
        fields=fields,
        enums=enums,
        row_count=len(rows),
    )


def collect_tables(schema_dir: Path) -> list[TableDefinition]:
    tables: list[TableDefinition] = []
    for schema_path in sorted(schema_dir.glob("*.schema.json"), key=lambda p: p.name):
        data_path = schema_path.with_name(schema_path.name.removesuffix(".schema.json") + ".data.json")
        if not data_path.exists():
            raise FileNotFoundError(f"Missing matching data file for {schema_path.name}: {data_path.name}")
        tables.append(load_table_definition(schema_path=schema_path, data_path=data_path))
    return tables


def gather_unique_enums(tables: list[TableDefinition]) -> tuple[EnumDefinition, ...]:
    merged: dict[str, tuple[str, ...]] = {}
    for table in tables:
        for enum_def in table.enums:
            existing = merged.get(enum_def.name)
            if existing is None:
                merged[enum_def.name] = enum_def.values
                continue
            if existing != enum_def.values:
                raise ValueError(f"enum '{enum_def.name}' has conflicting definitions")
    return tuple(EnumDefinition(name=name, values=values) for name, values in sorted(merged.items()))


# ---------------------------------------------------------------------------
# Jinja2 rendering
# ---------------------------------------------------------------------------

def create_jinja_env() -> Environment:
    return Environment(
        loader=FileSystemLoader(str(TEMPLATE_DIR)),
        keep_trailing_newline=True,
        trim_blocks=True,
        lstrip_blocks=True,
    )


def build_server_field_context(field: FieldDefinition) -> dict[str, Any]:
    """Enrich a FieldDefinition with the resolved server C++ type for templates."""
    return {
        "name": field.name,
        "kind": field.kind,
        "key": field.key,
        "required": field.required,
        "enum_name": field.enum_name,
        "json_name": field.json_name,
        "cpp_type": server_cpp_type(field),
    }


def build_unreal_field_context(field: FieldDefinition) -> dict[str, Any]:
    """Enrich a FieldDefinition with the resolved Unreal C++ type for templates."""
    return {
        "name": field.name,
        "kind": field.kind,
        "key": field.key,
        "required": field.required,
        "enum_name": normalize_enum_name(field.enum_name, "unreal") if field.enum_name else None,
        "json_name": field.json_name,
        "cpp_type": unreal_cpp_type(field),
    }


def generate_server_outputs(
    env: Environment,
    tables: list[TableDefinition],
    out_dir: Path,
) -> None:
    enums = gather_unique_enums(tables)

    # table_common.h — static, no context needed
    common_tmpl = env.get_template("server_table_common.h.j2")
    write_text(out_dir / "table_common.h", common_tmpl.render())

    # enums.h
    enums_tmpl = env.get_template("server_enums.h.j2")
    write_text(out_dir / "enums.h", enums_tmpl.render(enums=enums))

    # per-table headers
    table_tmpl = env.get_template("server_table.h.j2")
    for table in tables:
        fields_ctx = [build_server_field_context(f) for f in table.fields]
        content = table_tmpl.render(
            table_name=table.table_name,
            row_struct=table.server_struct,
            schema_file=table.schema_path.name,
            data_file=table.data_path.name,
            row_count=table.row_count,
            fields=fields_ctx,
        )
        write_text(out_dir / f"{table.source_stem}.h", content)

    # manifest (game_data_tables.h)
    manifest_tmpl = env.get_template("server_manifest.h.j2")
    table_ctx = [
        {
            "source_stem": t.source_stem,
            "table_name": t.table_name,
            "row_struct": t.server_struct,
            "schema_file": t.schema_path.name,
            "data_file": t.data_path.name,
            "row_count": t.row_count,
            "field_count": len(t.fields),
        }
        for t in tables
    ]
    write_text(out_dir / "game_data_tables.h", manifest_tmpl.render(tables=table_ctx))


def generate_unreal_outputs(
    env: Environment,
    tables: list[TableDefinition],
    out_dir: Path,
) -> None:
    enums = gather_unique_enums(tables)
    # Unreal enum names need E-prefix
    unreal_enums = tuple(
        EnumDefinition(
            name=normalize_enum_name(e.name, "unreal"),
            values=e.values,
        )
        for e in enums
    )

    # table_common.h
    common_tmpl = env.get_template("unreal_table_common.h.j2")
    write_text(out_dir / "table_common.h", common_tmpl.render())

    # enums.h (with TryParse)
    enums_tmpl = env.get_template("unreal_enums.h.j2")
    write_text(out_dir / "enums.h", enums_tmpl.render(enums=unreal_enums))

    # per-table headers
    table_tmpl = env.get_template("unreal_table.h.j2")
    for table in tables:
        fields_ctx = [build_unreal_field_context(f) for f in table.fields]
        content = table_tmpl.render(
            source_stem=table.source_stem,
            table_name=table.table_name,
            row_struct=table.unreal_struct,
            schema_file=table.schema_path.name,
            data_file=table.data_path.name,
            row_count=table.row_count,
            fields=fields_ctx,
        )
        write_text(out_dir / f"{table.source_stem}.h", content)

    # manifest (game_data_tables.h)
    manifest_tmpl = env.get_template("unreal_manifest.h.j2")
    table_ctx = [
        {
            "source_stem": t.source_stem,
            "table_name": t.table_name,
            "row_struct": t.unreal_struct,
            "schema_file": t.schema_path.name,
            "data_file": t.data_path.name,
            "row_count": t.row_count,
            "field_count": len(t.fields),
        }
        for t in tables
    ]
    write_text(out_dir / "game_data_tables.h", manifest_tmpl.render(tables=table_ctx))


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate game-data DTOs and loaders.")
    parser.add_argument("--schema-dir", type=Path, default=DEFAULT_SCHEMA_DIR)
    parser.add_argument("--server-out", type=Path, default=DEFAULT_SERVER_OUT)
    parser.add_argument("--unreal-out", type=Path, default=DEFAULT_UNREAL_OUT)
    return parser.parse_args()


def prepare_output_dir(out_dir: Path) -> None:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)


def main() -> int:
    args = parse_args()
    schema_dir = args.schema_dir.resolve()
    server_out = args.server_out.resolve() / OUTPUT_SUBDIR
    unreal_out = args.unreal_out.resolve() / OUTPUT_SUBDIR

    tables = collect_tables(schema_dir) if schema_dir.exists() else []

    env = create_jinja_env()

    prepare_output_dir(server_out)
    generate_server_outputs(env, tables, server_out)

    prepare_output_dir(unreal_out)
    generate_unreal_outputs(env, tables, unreal_out)

    print(f"Generated game data code for {len(tables)} table(s)")
    print(f"  server -> {server_out}")
    print(f"  unreal -> {unreal_out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())