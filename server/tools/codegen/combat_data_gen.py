#!/usr/bin/env python3
"""Generate combat data DTOs, enums, and loaders from schema/data JSON pairs."""

from __future__ import annotations

import argparse
import json
import re
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[2]
DEFAULT_SCHEMA_DIR = REPO_ROOT / "docs" / "proposal" / "ingame" / "data"
DEFAULT_SERVER_OUT = REPO_ROOT / "server" / "src" / "generated"
DEFAULT_UNREAL_OUT = REPO_ROOT / "client" / "Source" / "client" / "Public" / "Generated"
OUTPUT_SUBDIR = "combat"
MANIFEST_NAME = "combat_data_tables.h"
COMMON_NAME = "combat_data_common.h"
ENUM_NAME = "combat_data_enums.h"


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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate combat DTOs and loaders from schema/data JSON pairs.")
    parser.add_argument("--schema-dir", type=Path, default=DEFAULT_SCHEMA_DIR)
    parser.add_argument("--server-out", type=Path, default=DEFAULT_SERVER_OUT)
    parser.add_argument("--unreal-out", type=Path, default=DEFAULT_UNREAL_OUT)
    return parser.parse_args()


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")


def clean_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def read_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


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
    name = sanitize_identifier(text).upper()
    if name and name[0].isdigit():
        name = f"_{name}"
    return name


def cpp_string(text: str) -> str:
    escaped = text.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def cpp_wstring(text: str) -> str:
    escaped = text.replace("\\", "\\\\").replace('"', '\\"')
    return f'TEXT("{escaped}")'


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


def normalize_enum_name(name: str, runtime: str) -> str:
    clean = sanitize_identifier(name)
    if runtime == "unreal" and not clean.startswith("E"):
        return f"E{clean}"
    return clean


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
        items = raw_enums.items()
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


def runtime_namespace(runtime: str) -> str:
    return "psh::generated::combat" if runtime == "server" else ""


def primitive_cpp_type(field: FieldDefinition, runtime: str) -> str:
    if field.server_type and runtime == "server":
        return field.server_type
    if field.unreal_type and runtime == "unreal":
        return field.unreal_type

    mapping = {
        "server": {
            "int32": "std::int32_t",
            "int64": "std::int64_t",
            "float": "float",
            "double": "double",
            "bool": "bool",
            "string": "std::string",
            "name": "std::string",
            "text": "std::string",
        },
        "unreal": {
            "int32": "int32",
            "int64": "int64",
            "float": "float",
            "double": "double",
            "bool": "bool",
            "string": "FString",
            "name": "FName",
            "text": "FText",
        },
    }

    if field.kind == "enum":
        if not field.enum_name:
            raise ValueError(f"enum field '{field.name}' is missing enum name")
        return normalize_enum_name(field.enum_name, runtime)

    if field.kind not in mapping[runtime]:
        raise ValueError(f"unsupported field type '{field.kind}' for field '{field.name}'")
    return mapping[runtime][field.kind]


def render_common_header(runtime: str) -> str:
    lines: list[str] = ["// Auto-generated. Do not edit.", "#pragma once", ""]
    if runtime == "server":
        lines.extend(
            [
                "#include <cstddef>",
                "#include <string_view>",
                "",
                f"namespace {runtime_namespace(runtime)} {{",
                "struct TableFieldMetadata",
                "{",
                "    std::string_view Name;",
                "    std::string_view Type;",
                "    bool bRequired;",
                "    bool bKey;",
                "};",
                "",
                "struct TableDescriptor",
                "{",
                "    std::string_view TableName;",
                "    std::string_view SchemaFile;",
                "    std::string_view DataFile;",
                "    std::size_t RowCount;",
                "    std::size_t FieldCount;",
                "};",
                "",
                f"}} // namespace {runtime_namespace(runtime)}",
                "",
            ]
        )
    else:
        lines.extend(
            [
                "#include \"CoreMinimal.h\"",
                "",
                "struct FTableFieldMetadata",
                "{",
                "    const TCHAR* Name;",
                "    const TCHAR* Type;",
                "    bool bRequired;",
                "    bool bKey;",
                "};",
                "",
                "struct FTableDescriptor",
                "{",
                "    const TCHAR* TableName;",
                "    const TCHAR* SchemaFile;",
                "    const TCHAR* DataFile;",
                "    int32 RowCount;",
                "    int32 FieldCount;",
                "};",
                "",
                "",
            ]
        )
    return "\n".join(lines)


def render_enum_header(runtime: str, enums: tuple[EnumDefinition, ...]) -> str:
    lines: list[str] = ["// Auto-generated. Do not edit.", "#pragma once", ""]
    if runtime == "server":
        lines.extend(["#include <cstdint>", "#include <string_view>", ""])
    else:
        lines.extend(["#include \"CoreMinimal.h\"", ""])

    if runtime == "server":
        lines.append(f"namespace {runtime_namespace(runtime)} {{")
        lines.append("")

    for enum_def in enums:
        enum_name = normalize_enum_name(enum_def.name, runtime)
        enumerators = ",\n".join(
            f"    {value}"
            for value in enum_def.values
        )
        lines.extend(
            [
                f"enum class {enum_name} : {'uint8' if runtime == 'unreal' else 'std::uint8_t'}",
                "{",
                enumerators,
                "};",
                "",
            ]
        )

        if runtime == "server":
            lines.extend(
                [
                    f"inline bool TryParse{enum_name}(std::string_view text, {enum_name}& out_value)",
                    "{",
                ]
            )
            for index, value in enumerate(enum_def.values):
                comparator = f'text == {cpp_string(value)}'
                prefix = "if" if index == 0 else "else if"
                lines.append(f"    {prefix} ({comparator})")
                lines.append("    {")
                lines.append(f"        out_value = {enum_name}::{value};")
                lines.append("        return true;")
                lines.append("    }")
            lines.extend(["    return false;", "}", ""])
        else:
            lines.extend(
                [
                    f"inline bool TryParse{enum_name}(const FString& text, {enum_name}& out_value)",
                    "{",
                ]
            )
            for index, value in enumerate(enum_def.values):
                comparator = f'text.Equals(TEXT("{value}"))'
                prefix = "if" if index == 0 else "else if"
                lines.append(f"    {prefix} ({comparator})")
                lines.append("    {")
                lines.append(f"        out_value = {enum_name}::{value};")
                lines.append("        return true;")
                lines.append("    }")
            lines.extend(["    return false;", "}", ""])

    if runtime == "server":
        lines.append(f"}} // namespace {runtime_namespace(runtime)}")
        lines.append("")
    return "\n".join(lines)


def render_server_field_loader(field: FieldDefinition, table_name: str, object_name: str, row_name: str) -> str:
    json_key = field.json_name
    field_literal = cpp_string(json_key)
    error_prefix = f"{table_name}.{field.name}"
    lines: list[str] = []

    if field.kind == "enum":
        enum_name = normalize_enum_name(field.enum_name or field.name, "server")
        lines.extend(
            [
                f"const auto {field.name}_it = {object_name}.find({field_literal});",
                f"if ({field.name}_it == {object_name}.end())",
                "{",
                f'    throw std::runtime_error("missing required field: {error_prefix}");',
                "}",
                f"if (!{field.name}_it->is_string())",
                "{",
                f'    throw std::runtime_error("enum fields must be strings: {error_prefix}");',
                "}",
                f"{enum_name} {field.name}_value{{}};",
                f"if (!TryParse{enum_name}({field.name}_it->get<std::string>(), {field.name}_value))",
                "{",
                f'    throw std::runtime_error("failed to parse enum field: {error_prefix}");',
                "}",
                f"{row_name}.{field.name} = {field.name}_value;",
            ]
        )
        return "\n".join(lines)

    if field.kind in {"int32", "int64"}:
        cpp_type = primitive_cpp_type(field, "server")
        lines.extend(
            [
                f"const auto {field.name}_it = {object_name}.find({field_literal});",
                f"if ({field.name}_it == {object_name}.end())",
                "{",
                f'    throw std::runtime_error("missing required field: {error_prefix}");',
                "}",
                f"if (!{field.name}_it->is_number_integer() && !{field.name}_it->is_number_unsigned())",
                "{",
                f'    throw std::runtime_error("integer field must be numeric: {error_prefix}");',
                "}",
                f"{row_name}.{field.name} = static_cast<{cpp_type}>({field.name}_it->get<long long>());",
            ]
        )
        return "\n".join(lines)

    if field.kind in {"float", "double"}:
        cpp_type = primitive_cpp_type(field, "server")
        lines.extend(
            [
                f"const auto {field.name}_it = {object_name}.find({field_literal});",
                f"if ({field.name}_it == {object_name}.end())",
                "{",
                f'    throw std::runtime_error("missing required field: {error_prefix}");',
                "}",
                f"if (!{field.name}_it->is_number())",
                "{",
                f'    throw std::runtime_error("float field must be numeric: {error_prefix}");',
                "}",
                f"{row_name}.{field.name} = static_cast<{cpp_type}>({field.name}_it->get<double>());",
            ]
        )
        return "\n".join(lines)

    if field.kind == "bool":
        lines.extend(
            [
                f"const auto {field.name}_it = {object_name}.find({field_literal});",
                f"if ({field.name}_it == {object_name}.end())",
                "{",
                f'    throw std::runtime_error("missing required field: {error_prefix}");',
                "}",
                f"if (!{field.name}_it->is_boolean())",
                "{",
                f'    throw std::runtime_error("bool field must be a boolean: {error_prefix}");',
                "}",
                f"{row_name}.{field.name} = {field.name}_it->get<bool>();",
            ]
        )
        return "\n".join(lines)

    if field.kind in {"string", "name", "text"}:
        lines.extend(
            [
                f"const auto {field.name}_it = {object_name}.find({field_literal});",
                f"if ({field.name}_it == {object_name}.end())",
                "{",
                f'    throw std::runtime_error("missing required field: {error_prefix}");',
                "}",
                f"if (!{field.name}_it->is_string())",
                "{",
                f'    throw std::runtime_error("string field must be a string: {error_prefix}");',
                "}",
                f"{row_name}.{field.name} = {field.name}_it->get<std::string>();",
            ]
        )
        return "\n".join(lines)

    raise ValueError(f"unsupported field type for server loader: {field.kind}")


def render_unreal_field_loader(field: FieldDefinition, table_name: str, object_name: str, row_name: str, error_name: str) -> str:
    json_key = field.json_name
    field_literal = cpp_wstring(json_key)
    error_prefix = f"{table_name}.{field.name}"
    lines: list[str] = []

    if field.kind == "enum":
        enum_name = normalize_enum_name(field.enum_name or field.name, "unreal")
        lines.extend(
            [
                "{",
                "    FString value;",
                f"    if (!{object_name}->TryGetStringField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required enum field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {enum_name} parsed_value{{}};",
                f"    if (!TryParse{enum_name}(value, parsed_value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("failed to parse enum field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = parsed_value;",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind in {"int32", "int64"}:
        cpp_type = primitive_cpp_type(field, "unreal")
        lines.extend(
            [
                "{",
                "    double value = 0.0;",
                f"    if (!{object_name}->TryGetNumberField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required numeric field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = static_cast<{cpp_type}>(value);",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind in {"float", "double"}:
        cpp_type = primitive_cpp_type(field, "unreal")
        lines.extend(
            [
                "{",
                "    double value = 0.0;",
                f"    if (!{object_name}->TryGetNumberField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required numeric field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = static_cast<{cpp_type}>(value);",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind == "bool":
        lines.extend(
            [
                "{",
                "    bool value = false;",
                f"    if (!{object_name}->TryGetBoolField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required bool field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = value;",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind == "string":
        lines.extend(
            [
                "{",
                "    FString value;",
                f"    if (!{object_name}->TryGetStringField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required string field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = MoveTemp(value);",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind == "name":
        lines.extend(
            [
                "{",
                "    FString value;",
                f"    if (!{object_name}->TryGetStringField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required name field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = FName(*value);",
                "}",
            ]
        )
        return "\n".join(lines)

    if field.kind == "text":
        lines.extend(
            [
                "{",
                "    FString value;",
                f"    if (!{object_name}->TryGetStringField({field_literal}, value))",
                "    {",
                f'        {error_name} = FString::Printf(TEXT("missing required text field: {error_prefix}"));',
                "        return false;",
                "    }",
                f"    {row_name}.{field.name} = FText::FromString(MoveTemp(value));",
                "}",
            ]
        )
        return "\n".join(lines)

    raise ValueError(f"unsupported field type for unreal loader: {field.kind}")


def render_table_header(runtime: str, table: TableDefinition) -> str:
    row_struct = table.server_struct if runtime == "server" else table.unreal_struct
    lines: list[str] = ["// Auto-generated. Do not edit.", "#pragma once", ""]

    if runtime == "server":
        lines.extend(
            [
                "#include <array>",
                "#include <cstdint>",
                "#include <filesystem>",
                "#include <fstream>",
                "#include <nlohmann/json.hpp>",
                "#include <stdexcept>",
                "#include <string>",
                "#include <string_view>",
                "#include <vector>",
                "",
                f'#include "{COMMON_NAME}"',
                f'#include "{ENUM_NAME}"',
                "",
                "namespace psh::generated::combat {",
                "",
                f"struct {row_struct}",
                "{",
            ]
        )
        for field in table.fields:
            lines.append(f"    {primitive_cpp_type(field, runtime)} {field.name}{{}};")
        lines.extend(
            [
                "};",
                "",
                f"struct {table.table_name}Schema",
                "{",
                f"    static constexpr std::string_view TableName = {cpp_string(table.table_name)};",
                f"    static constexpr std::string_view SchemaFile = {cpp_string(table.schema_path.name)};",
                f"    static constexpr std::string_view DataFile = {cpp_string(table.data_path.name)};",
                f"    static constexpr std::size_t RowCount = {table.row_count};",
                f"    static constexpr std::size_t FieldCount = {len(table.fields)};",
                f"    static constexpr std::array<TableFieldMetadata, FieldCount> Fields = {{",
            ]
        )
        for field in table.fields:
            lines.append(
                f"        TableFieldMetadata{{{cpp_string(field.json_name)}, {cpp_string(field.kind)}, {str(field.required).lower()}, {str(field.key).lower()}}},"
            )
        lines.extend(
            [
                "    };",
                "};",
                "",
                f"inline {row_struct} Load{table.table_name}Row(const nlohmann::json& object)",
                "{",
                "    if (!object.is_object())",
                "    {",
                '        throw std::runtime_error("table row must be a JSON object");',
                "    }",
                "",
                f"    {row_struct} row{{}};",
            ]
        )
        for field in table.fields:
            lines.extend(f"    {line}" for line in render_server_field_loader(field, table.table_name, "object", "row").splitlines())
        lines.extend(
            [
                "    return row;",
                "}",
                "",
                f"inline std::vector<{row_struct}> Load{table.table_name}(const nlohmann::json& root)",
                "{",
                "    const nlohmann::json* rows = &root;",
                "    if (root.is_object())",
                "    {",
                '        if (auto it = root.find("rows"); it != root.end())',
                "        {",
                "            rows = &*it;",
                "        }",
                '        else if (auto it = root.find("data"); it != root.end())',
                "        {",
                "            rows = &*it;",
                "        }",
                "    }",
                "",
                "    if (!rows->is_array())",
                "    {",
                '        throw std::runtime_error("table root must be an array or an object with rows/data");',
                "    }",
                "",
                f"    std::vector<{row_struct}> result;",
                "    result.reserve(rows->size());",
                "    for (const auto& entry : *rows)",
                "    {",
                f"        result.push_back(Load{table.table_name}Row(entry));",
                "    }",
                "    return result;",
                "}",
                "",
                f"inline std::vector<{row_struct}> Load{table.table_name}FromFile(const std::filesystem::path& path)",
                "{",
                "    std::ifstream input(path);",
                "    if (!input.is_open())",
                "    {",
                '        throw std::runtime_error("failed to open data file");',
                "    }",
                "",
                "    nlohmann::json root;",
                "    input >> root;",
                f"    return Load{table.table_name}(root);",
                "}",
                "",
                "} // namespace psh::generated::combat",
                "",
            ]
        )
        return "\n".join(lines)

    lines.extend(
        [
            "#include \"CoreMinimal.h\"",
            "#include \"Dom/JsonObject.h\"",
            "#include \"Misc/FileHelper.h\"",
            "#include \"Misc/Paths.h\"",
            "#include \"Serialization/JsonReader.h\"",
            "#include \"Serialization/JsonSerializer.h\"",
            f'#include "{COMMON_NAME}"',
            f'#include "{ENUM_NAME}"',
            "",
            f"struct {row_struct}",
            "{",
        ]
    )
    for field in table.fields:
        lines.append(f"    {primitive_cpp_type(field, runtime)} {field.name}{{}};")
        lines.append("")
    if lines[-1] == "":
        lines.pop()
    lines.extend(
        [
            "};",
            "",
            f"struct {table.table_name}Schema",
            "{",
            f"    static constexpr const TCHAR* TableName = {cpp_wstring(table.table_name)};",
            f"    static constexpr const TCHAR* SchemaFile = {cpp_wstring(table.schema_path.name)};",
            f"    static constexpr const TCHAR* DataFile = {cpp_wstring(table.data_path.name)};",
            f"    static constexpr int32 RowCount = {table.row_count};",
            f"    static constexpr int32 FieldCount = {len(table.fields)};",
            f"    static constexpr FTableFieldMetadata Fields[FieldCount] = {{",
        ]
    )
    for field in table.fields:
        lines.append(
            f"        FTableFieldMetadata{{{cpp_wstring(field.json_name)}, {cpp_wstring(field.kind)}, {str(field.required).lower()}, {str(field.key).lower()}}},"
        )
    lines.extend(
        [
            "    };",
            "};",
            "",
            f"inline bool Load{table.table_name}Row(const TSharedPtr<FJsonObject>& object, {row_struct}& out_row, FString& out_error)",
            "{",
            "    if (!object.IsValid())",
            "    {",
            '        out_error = TEXT("table row must be a JSON object");',
            "        return false;",
            "    }",
            "",
        ]
    )
    for field in table.fields:
        lines.extend(f"    {line}" for line in render_unreal_field_loader(field, table.table_name, "object", "out_row", "out_error").splitlines())
    lines.extend(
        [
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}(const FString& json_text, TArray<{row_struct}>& out_rows, FString& out_error)",
            "{",
            "    TSharedPtr<FJsonObject> root_object;",
            "    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json_text);",
            "    if (!FJsonSerializer::Deserialize(reader, root_object) || !root_object.IsValid())",
            "    {",
            '        out_error = TEXT("failed to parse table JSON");',
            "        return false;",
            "    }",
            "",
            "    const TArray<TSharedPtr<FJsonValue>>* rows = nullptr;",
            '    if (!root_object->TryGetArrayField(TEXT("rows"), rows))',
            "    {",
            '        if (!root_object->TryGetArrayField(TEXT("data"), rows))',
            "        {",
            '            out_error = TEXT("table root must expose a \\"rows\\" or \\"data\\" array");',
            "            return false;",
            "        }",
            "    }",
            "",
            "    out_rows.Reset(rows->Num());",
            "    for (const TSharedPtr<FJsonValue>& entry : *rows)",
            "    {",
            "        const TSharedPtr<FJsonObject>* entry_object = nullptr;",
            "        if (!entry.IsValid() || entry->Type != EJson::Object || !entry->TryGetObject(entry_object) || !entry_object || !entry_object->IsValid())",
            "        {",
            '            out_error = TEXT("table row must be a JSON object");',
            "            return false;",
            "        }",
            "",
            f"        {row_struct} row{{}};",
            f"        if (!Load{table.table_name}Row(*entry_object, row, out_error))",
            "        {",
            "            return false;",
            "        }",
            "        out_rows.Add(MoveTemp(row));",
            "    }",
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}FromFile(const FString& file_path, TArray<{row_struct}>& out_rows, FString& out_error)",
            "{",
            "    FString json_text;",
            "    if (!FFileHelper::LoadFileToString(json_text, *file_path))",
            "    {",
            '        out_error = FString::Printf(TEXT("failed to read data file: %s"), *file_path);',
            "        return false;",
            "    }",
            f"    return Load{table.table_name}(json_text, out_rows, out_error);",
            "}",
            "",
        ]
    )
    return "\n".join(lines)


def gather_unique_enums(tables: tuple[TableDefinition, ...]) -> tuple[EnumDefinition, ...]:
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


def render_manifest_header(runtime: str, tables: tuple[TableDefinition, ...]) -> str:
    lines: list[str] = ["// Auto-generated. Do not edit.", "#pragma once", ""]
    if runtime == "server":
        lines.extend(
            [
                "#include <array>",
                "#include <filesystem>",
                "#include <string_view>",
                "#include <vector>",
                "",
            ]
        )
    else:
        lines.extend(["#include \"CoreMinimal.h\"", "#include \"Misc/Paths.h\"", ""])

    lines.append(f'#include "{COMMON_NAME}"')
    lines.append(f'#include "{ENUM_NAME}"')
    for table in tables:
        lines.append(f'#include "{OUTPUT_SUBDIR}/{table.source_stem}.h"')
    lines.append("")

    descriptor_type = "TableDescriptor" if runtime == "server" else "FTableDescriptor"
    bundle_type = "CombatDataBundle" if runtime == "server" else "FCombatDataBundle"
    string_literal = cpp_string if runtime == "server" else cpp_wstring

    if runtime == "server":
        lines.append("namespace psh::generated::combat {")
        lines.append("")

    lines.append(f"inline constexpr std::array<{descriptor_type}, {len(tables)}> kCombatDataTables = {{")
    for table in tables:
        lines.append(
            f"    {descriptor_type}{{{string_literal(table.table_name)}, {string_literal(table.schema_path.name)}, {string_literal(table.data_path.name)}, {table.row_count}, {len(table.fields)}}},"
        )
    lines.append("};")
    lines.append("")

    lines.append(f"struct {bundle_type}")
    lines.append("{")
    for table in tables:
        row_struct = table.server_struct if runtime == "server" else table.unreal_struct
        value_type = f"std::vector<{row_struct}>" if runtime == "server" else f"TArray<{row_struct}>"
        lines.append(f"    {value_type} {table.table_name}Rows;")
    lines.append("};")
    lines.append("")

    if runtime == "server":
        lines.append(f"inline {bundle_type} LoadCombatDataBundleFromDirectory(const std::filesystem::path& directory)")
        lines.append("{")
        lines.append(f"    {bundle_type} bundle{{}};")
        for table in tables:
            lines.append(
                f"    bundle.{table.table_name}Rows = Load{table.table_name}FromFile(directory / {cpp_string(table.data_path.name)});"
            )
        lines.append("    return bundle;")
        lines.append("}")
    else:
        lines.append(
            f"inline bool LoadCombatDataBundleFromDirectory(const FString& directory, {bundle_type}& out_bundle, FString& out_error)"
        )
        lines.append("{")
        for table in tables:
            lines.append(
                f"    if (!Load{table.table_name}FromFile(FPaths::Combine(directory, {cpp_wstring(table.data_path.name)}), out_bundle.{table.table_name}Rows, out_error))"
            )
            lines.append("    {")
            lines.append("        return false;")
            lines.append("    }")
        lines.append("    return true;")
        lines.append("}")

    if runtime == "server":
        lines.append("")
        lines.append("} // namespace psh::generated::combat")
        lines.append("")

    lines.append("")
    return "\n".join(lines)


def build_runtime_outputs(runtime: str, tables: tuple[TableDefinition, ...]) -> dict[Path, str]:
    enums = gather_unique_enums(tables)
    outputs: dict[Path, str] = {
        Path(COMMON_NAME): render_common_header(runtime),
        Path(ENUM_NAME): render_enum_header(runtime, enums),
        Path(MANIFEST_NAME): render_manifest_header(runtime, tables),
    }
    for table in tables:
        outputs[Path(OUTPUT_SUBDIR) / f"{table.source_stem}.h"] = render_table_header(runtime, table)
    return outputs


def emit_outputs(root: Path, outputs: dict[Path, str]) -> None:
    for relative_path, content in outputs.items():
        write_text(root / relative_path, content)


def prepare_output_root(root: Path) -> None:
    root.mkdir(parents=True, exist_ok=True)
    shutil.rmtree(root / OUTPUT_SUBDIR, ignore_errors=True)
    for filename in (COMMON_NAME, ENUM_NAME, MANIFEST_NAME):
        target = root / filename
        if target.exists():
            target.unlink()


def main() -> int:
    args = parse_args()
    schema_dir = args.schema_dir.resolve()
    server_out = args.server_out.resolve()
    unreal_out = args.unreal_out.resolve()

    tables = collect_tables(schema_dir) if schema_dir.exists() else []
    prepare_output_root(server_out)
    prepare_output_root(unreal_out)
    emit_outputs(server_out, build_runtime_outputs("server", tuple(tables)))
    emit_outputs(unreal_out, build_runtime_outputs("unreal", tuple(tables)))

    print(f"Generated combat data code for {len(tables)} table(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
