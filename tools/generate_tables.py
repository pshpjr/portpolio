#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import re
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SERVER_OUT = REPO_ROOT / "shared" / "generated" / "server"
DEFAULT_UNREAL_OUT = REPO_ROOT / "client" / "Source" / "client" / "Public" / "Generated"
DEFAULT_DATA_OUT = REPO_ROOT / "client" / "Content" / "Data"


@dataclass(frozen=True)
class EnumDefinition:
    name: str
    values: tuple[str, ...]


@dataclass(frozen=True)
class FieldDefinition:
    name: str
    field_type: str
    enum_name: str | None = None


@dataclass(frozen=True)
class KeyDefinition:
    field_name: str
    unique: bool


@dataclass(frozen=True)
class TableDefinition:
    table_name: str
    snake_table_name: str
    schema_path: Path
    data_path: Path
    row_count: int
    fields: tuple[FieldDefinition, ...]
    keys: tuple[KeyDefinition, ...]
    enums: tuple[EnumDefinition, ...]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate table headers and data copies from a schema/data pair.")
    parser.add_argument("--schema", type=Path, help="Path to a single *.schema.json file.")
    parser.add_argument(
        "--schema-dir",
        type=Path,
        help="Directory containing exactly one *.schema.json file to process.",
    )
    parser.add_argument(
        "--server-out",
        type=Path,
        default=DEFAULT_SERVER_OUT,
        help="Directory where server headers are written.",
    )
    parser.add_argument(
        "--unreal-out",
        type=Path,
        default=DEFAULT_UNREAL_OUT,
        help="Directory where Unreal headers are written.",
    )
    parser.add_argument(
        "--data-src",
        type=Path,
        help="Path to the data JSON file. Defaults to the schema sibling with .data.json suffix.",
    )
    parser.add_argument(
        "--data-out",
        type=Path,
        default=DEFAULT_DATA_OUT,
        help="Directory where the source data JSON is copied.",
    )
    parser.add_argument(
        "--subdir",
        default="",
        help="Optional server header subdirectory and manifest prefix.",
    )
    return parser.parse_args()


def read_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")


def cpp_string(value: str) -> str:
    escaped = value.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def cpp_text(value: str) -> str:
    escaped = value.replace("\\", "\\\\").replace('"', '\\"')
    return f'TEXT("{escaped}")'


def snake_case(value: str) -> str:
    value = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", value)
    value = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", value)
    value = re.sub(r"[^A-Za-z0-9]+", "_", value)
    value = re.sub(r"_+", "_", value).strip("_")
    return value.lower()


def pascal_case(value: str) -> str:
    parts = re.split(r"[^A-Za-z0-9]+", value)
    return "".join(part[:1].upper() + part[1:] for part in parts if part)


def resolve_schema_path(args: argparse.Namespace) -> Path:
    if bool(args.schema) == bool(args.schema_dir):
        raise ValueError("exactly one of --schema or --schema-dir must be provided")

    if args.schema is not None:
        schema_path = args.schema.resolve()
        if not schema_path.is_file():
            raise FileNotFoundError(f"schema file not found: {schema_path}")
        return schema_path

    schema_dir = args.schema_dir.resolve()
    if not schema_dir.is_dir():
        raise FileNotFoundError(f"schema directory not found: {schema_dir}")

    schema_paths = sorted(schema_dir.glob("*.schema.json"))
    if len(schema_paths) != 1:
        raise ValueError(f"--schema-dir must contain exactly one *.schema.json file: {schema_dir}")
    return schema_paths[0]


def resolve_data_path(schema_path: Path, data_src: Path | None) -> Path:
    if data_src is not None:
        resolved = data_src.resolve()
        if not resolved.is_file():
            raise FileNotFoundError(f"data file not found: {resolved}")
        return resolved

    candidate = schema_path.with_name(schema_path.name.removesuffix(".schema.json") + ".data.json")
    if not candidate.is_file():
        raise FileNotFoundError(f"matching data file not found: {candidate}")
    return candidate


def normalize_enum_name(name: str) -> str:
    if not name:
        raise ValueError("enum name cannot be empty")
    return name


def normalize_field_type(field_type: str) -> str:
    supported = {"int32", "int64", "float", "double", "bool", "string", "enum"}
    normalized = field_type.strip().lower()
    if normalized not in supported:
        raise ValueError(f"unsupported field type: {field_type}")
    return normalized


def validate_data_rows(fields: tuple[FieldDefinition, ...], data_path: Path) -> int:
    raw_data = read_json(data_path)
    if isinstance(raw_data, list):
        rows = raw_data
    elif isinstance(raw_data, dict):
        rows = raw_data.get("rows") or raw_data.get("data")
        if not isinstance(rows, list):
            raise ValueError(f"data root must be an array or expose 'rows'/'data': {data_path}")
    else:
        raise ValueError(f"data root must be an array or object: {data_path}")

    required_fields = {field.name for field in fields}
    for index, row in enumerate(rows, start=1):
        if not isinstance(row, dict):
            raise ValueError(f"row {index} in {data_path} must be an object")
        missing = sorted(required_fields - set(row.keys()))
        if missing:
            raise ValueError(f"row {index} in {data_path} is missing fields: {', '.join(missing)}")
    return len(rows)


def load_table_definition(schema_path: Path, data_path: Path) -> TableDefinition:
    raw_schema = read_json(schema_path)
    if not isinstance(raw_schema, dict):
        raise ValueError(f"schema must be an object: {schema_path}")

    table_name = raw_schema.get("table")
    if not isinstance(table_name, str) or not table_name.strip():
        raise ValueError(f"schema is missing a valid 'table' name: {schema_path}")
    table_name = table_name.strip()

    raw_fields = raw_schema.get("fields")
    if not isinstance(raw_fields, list) or not raw_fields:
        raise ValueError(f"schema must define a non-empty 'fields' array: {schema_path}")

    fields: list[FieldDefinition] = []
    field_names: set[str] = set()
    for raw_field in raw_fields:
        if not isinstance(raw_field, dict):
            raise ValueError("field definitions must be objects")
        name = raw_field.get("name")
        field_type = raw_field.get("type")
        if not isinstance(name, str) or not name.strip():
            raise ValueError("field is missing a valid name")
        if not isinstance(field_type, str) or not field_type.strip():
            raise ValueError(f"field '{name}' is missing a valid type")
        if name in field_names:
            raise ValueError(f"duplicate field name: {name}")
        field_names.add(name)

        normalized_type = normalize_field_type(field_type)
        enum_name = raw_field.get("enum_name")
        if normalized_type == "enum":
            if not isinstance(enum_name, str) or not enum_name.strip():
                raise ValueError(f"enum field '{name}' is missing enum_name")
            enum_name = normalize_enum_name(enum_name.strip())
        else:
            enum_name = None

        fields.append(FieldDefinition(name=name.strip(), field_type=normalized_type, enum_name=enum_name))

    raw_keys = raw_schema.get("keys", [])
    if raw_keys is None:
        raw_keys = []
    if not isinstance(raw_keys, list):
        raise ValueError("'keys' must be an array when present")

    keys: list[KeyDefinition] = []
    key_names: set[str] = set()
    for raw_key in raw_keys:
        if not isinstance(raw_key, dict):
            raise ValueError("key definitions must be objects")
        field_name = raw_key.get("field")
        if not isinstance(field_name, str) or not field_name.strip():
            raise ValueError("key is missing a valid field name")
        if field_name not in field_names:
            raise ValueError(f"key references unknown field: {field_name}")
        if field_name in key_names:
            raise ValueError(f"duplicate key definition: {field_name}")
        key_names.add(field_name)
        keys.append(KeyDefinition(field_name=field_name, unique=bool(raw_key.get("unique", True))))

    raw_enums = raw_schema.get("enums", [])
    if not isinstance(raw_enums, list):
        raise ValueError("'enums' must be an array when present")

    enums_by_name: dict[str, tuple[str, ...]] = {}
    for raw_enum in raw_enums:
        if not isinstance(raw_enum, dict):
            raise ValueError("enum definitions must be objects")
        name = raw_enum.get("name")
        values = raw_enum.get("values")
        if not isinstance(name, str) or not name.strip():
            raise ValueError("enum is missing a valid name")
        if not isinstance(values, list) or not values:
            raise ValueError(f"enum '{name}' must define a non-empty values array")
        normalized_values: list[str] = []
        for raw_value in values:
            if not isinstance(raw_value, str) or not raw_value.strip():
                raise ValueError(f"enum '{name}' contains an invalid value")
            normalized_values.append(raw_value.strip())
        key = normalize_enum_name(name.strip())
        values_tuple = tuple(normalized_values)
        if key in enums_by_name and enums_by_name[key] != values_tuple:
            raise ValueError(f"enum '{key}' is defined more than once with different values")
        enums_by_name[key] = values_tuple

    for field in fields:
        if field.enum_name and field.enum_name not in enums_by_name:
            raise ValueError(f"field '{field.name}' references unknown enum '{field.enum_name}'")

    row_count = validate_data_rows(fields=tuple(fields), data_path=data_path)

    return TableDefinition(
        table_name=table_name,
        snake_table_name=snake_case(table_name),
        schema_path=schema_path,
        data_path=data_path,
        row_count=row_count,
        fields=tuple(fields),
        keys=tuple(keys),
        enums=tuple(EnumDefinition(name=name, values=values) for name, values in enums_by_name.items()),
    )


def first_enum_value(enum_name: str, enums: tuple[EnumDefinition, ...]) -> str:
    for enum_definition in enums:
        if enum_definition.name == enum_name:
            return enum_definition.values[0]
    raise ValueError(f"unknown enum: {enum_name}")


def unreal_type(field: FieldDefinition) -> str:
    mapping = {
        "int32": "int32",
        "int64": "int64",
        "float": "float",
        "double": "double",
        "bool": "bool",
        "string": "FString",
    }
    if field.field_type == "enum":
        if field.enum_name is None:
            raise ValueError(f"field '{field.name}' is missing enum_name")
        return field.enum_name
    return mapping[field.field_type]


def server_type(field: FieldDefinition) -> str:
    mapping = {
        "int32": "std::int32_t",
        "int64": "std::int64_t",
        "float": "float",
        "double": "double",
        "bool": "bool",
        "string": "std::string",
    }
    if field.field_type == "enum":
        if field.enum_name is None:
            raise ValueError(f"field '{field.name}' is missing enum_name")
        return field.enum_name
    return mapping[field.field_type]


def unreal_initializer(field: FieldDefinition, enums: tuple[EnumDefinition, ...]) -> str:
    if field.field_type in {"int32", "int64", "float", "double"}:
        return " = 0"
    if field.field_type == "bool":
        return " = false"
    if field.field_type == "string":
        return "{}"
    if field.field_type == "enum":
        if field.enum_name is None:
            raise ValueError(f"field '{field.name}' is missing enum_name")
        return f" = {field.enum_name}::{first_enum_value(field.enum_name, enums)}"
    raise ValueError(f"unsupported field type: {field.field_type}")


def key_cpp_type(field: FieldDefinition, runtime: str) -> str:
    return unreal_type(field) if runtime == "unreal" else server_type(field)


def build_manifest_identity(table: TableDefinition, subdir: str) -> tuple[str, str, str, str]:
    stem = subdir.strip() or table.snake_table_name
    manifest_path = f"{stem}_data_tables.h"
    manifest_name = pascal_case(stem) or pascal_case(table.snake_table_name)
    constant_name = f"k{manifest_name}DataTables"
    bundle_name = f"{manifest_name}DataBundle"
    loader_name = f"Load{manifest_name}DataBundleFromDirectory"
    return manifest_path, constant_name, bundle_name, loader_name


def namespace_for_subdir(subdir: str) -> str:
    cleaned = subdir.strip()
    if not cleaned:
        return "psh::generated"
    return f"psh::generated::{cleaned}"


def render_unreal_enums(enums: tuple[EnumDefinition, ...]) -> list[str]:
    lines: list[str] = [
        "// ============================================================",
        "// Enums",
        "// ============================================================",
    ]
    for enum_definition in enums:
        lines.extend(
            [
                "UENUM(BlueprintType)",
                f"enum class {enum_definition.name} : uint8",
                "{",
            ]
        )
        for index, value in enumerate(enum_definition.values):
            suffix = "," if index + 1 < len(enum_definition.values) else ""
            lines.append(f'    {value} UMETA(DisplayName = "{value}"){suffix}')
        lines.extend(
            [
                "};",
                "",
                f"inline bool TryParse{enum_definition.name}(const FString& Value, {enum_definition.name}& Out)",
                "{",
            ]
        )
        for index, value in enumerate(enum_definition.values):
            keyword = "if" if index == 0 else "else if"
            lines.extend(
                [
                    f"    {keyword} (Value == {cpp_text(value)})",
                    "    {",
                    f"        Out = {enum_definition.name}::{value};",
                    "        return true;",
                    "    }",
                ]
            )
        lines.extend(["    return false;", "}", ""])
    return lines


def render_server_enums(enums: tuple[EnumDefinition, ...]) -> list[str]:
    lines: list[str] = [
        "// ============================================================",
        "// Enums",
        "// ============================================================",
    ]
    for enum_definition in enums:
        lines.extend(
            [
                f"enum class {enum_definition.name} : std::uint8_t",
                "{",
            ]
        )
        for index, value in enumerate(enum_definition.values):
            separator = "," if index + 1 < len(enum_definition.values) else ""
            assignment = " = 0" if index == 0 else ""
            lines.append(f"    {value}{assignment}{separator}")
        lines.extend(
            [
                "};",
                "",
                f"inline bool TryParse{enum_definition.name}(const std::string& Value, {enum_definition.name}& Out)",
                "{",
            ]
        )
        for index, value in enumerate(enum_definition.values):
            keyword = "if" if index == 0 else "else if"
            lines.extend(
                [
                    f"    {keyword} (Value == {cpp_string(value)})",
                    "    {",
                    f"        Out = {enum_definition.name}::{value};",
                    "        return true;",
                    "    }",
                ]
            )
        lines.extend(["    return false;", "}", ""])
    return lines


def render_unreal_row_struct(table: TableDefinition) -> list[str]:
    lines = [
        "// ============================================================",
        "// Row Struct",
        "// ============================================================",
        "USTRUCT(BlueprintType)",
        f"struct F{table.table_name}Row",
        "{",
        "    GENERATED_BODY()",
        "",
    ]
    for index, field in enumerate(table.fields):
        lines.extend(
            [
                '    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")',
                f"    {unreal_type(field)} {field.name}{unreal_initializer(field, table.enums)};",
            ]
        )
        if index + 1 < len(table.fields):
            lines.append("")
    lines.extend(["};", ""])
    return lines


def render_server_row_struct(table: TableDefinition) -> list[str]:
    lines = [
        "// ============================================================",
        "// Row Struct",
        "// ============================================================",
        f"struct {table.table_name}Row",
        "{",
    ]
    for field in table.fields:
        lines.append(f"    {server_type(field)} {field.name}{{}};")
    lines.extend(["};", ""])
    return lines


def render_unreal_field_loader(table_name: str, field: FieldDefinition) -> list[str]:
    field_path = f"{table_name}.{field.name}"
    if field.field_type == "enum":
        if field.enum_name is None:
            raise ValueError(f"field '{field.name}' is missing enum_name")
        return [
            "    {",
            "        FString Value;",
            f"        if (!Object->TryGetStringField({cpp_text(field.name)}, Value))",
            "        {",
            f"            OutError = FString::Printf(TEXT(\"missing required enum field: {field_path}\"));",
            "            return false;",
            "        }",
            f"        if (!TryParse{field.enum_name}(Value, OutRow.{field.name}))",
            "        {",
            f"            OutError = FString::Printf(TEXT(\"failed to parse enum field: {field_path}\"));",
            "            return false;",
            "        }",
            "    }",
        ]

    if field.field_type in {"int32", "int64", "float", "double"}:
        return [
            "    {",
            "        double Value = 0.0;",
            f"        if (!Object->TryGetNumberField({cpp_text(field.name)}, Value))",
            "        {",
            f"            OutError = FString::Printf(TEXT(\"missing required numeric field: {field_path}\"));",
            "            return false;",
            "        }",
            f"        OutRow.{field.name} = static_cast<{unreal_type(field)}>(Value);",
            "    }",
        ]

    if field.field_type == "bool":
        return [
            "    {",
            "        bool Value = false;",
            f"        if (!Object->TryGetBoolField({cpp_text(field.name)}, Value))",
            "        {",
            f"            OutError = FString::Printf(TEXT(\"missing required bool field: {field_path}\"));",
            "            return false;",
            "        }",
            f"        OutRow.{field.name} = Value;",
            "    }",
        ]

    if field.field_type == "string":
        return [
            "    {",
            "        FString Value;",
            f"        if (!Object->TryGetStringField({cpp_text(field.name)}, Value))",
            "        {",
            f"            OutError = FString::Printf(TEXT(\"missing required string field: {field_path}\"));",
            "            return false;",
            "        }",
            f"        OutRow.{field.name} = MoveTemp(Value);",
            "    }",
        ]

    raise ValueError(f"unsupported field type: {field.field_type}")


def render_server_field_loader(table_name: str, field: FieldDefinition) -> list[str]:
    field_path = f"{table_name}.{field.name}"
    iterator_name = f"{field.name}It"
    lines = [
        f"    const auto {iterator_name} = Object.find({cpp_string(field.name)});",
        f"    if ({iterator_name} == Object.end())",
        "    {",
    ]
    if field.field_type == "enum":
        lines.append(f'        OutError = "missing required enum field: {field_path}";')
    elif field.field_type in {"int32", "int64", "float", "double"}:
        lines.append(f'        OutError = "missing required numeric field: {field_path}";')
    elif field.field_type == "bool":
        lines.append(f'        OutError = "missing required bool field: {field_path}";')
    else:
        lines.append(f'        OutError = "missing required string field: {field_path}";')
    lines.extend(["        return false;", "    }"])

    if field.field_type == "enum":
        if field.enum_name is None:
            raise ValueError(f"field '{field.name}' is missing enum_name")
        lines.extend(
            [
                f"    if (!{iterator_name}->is_string())",
                "    {",
                f'        OutError = "enum field must be a string: {field_path}";',
                "        return false;",
                "    }",
                f"    if (!TryParse{field.enum_name}({iterator_name}->get<std::string>(), OutRow.{field.name}))",
                "    {",
                f'        OutError = "failed to parse enum field: {field_path}";',
                "        return false;",
                "    }",
            ]
        )
        return lines

    if field.field_type == "int32":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_number_integer() && !{iterator_name}->is_number_unsigned())",
                "    {",
                f'        OutError = "integer field must be numeric: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = static_cast<std::int32_t>({iterator_name}->get<long long>());",
            ]
        )
        return lines

    if field.field_type == "int64":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_number_integer() && !{iterator_name}->is_number_unsigned())",
                "    {",
                f'        OutError = "integer field must be numeric: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = static_cast<std::int64_t>({iterator_name}->get<long long>());",
            ]
        )
        return lines

    if field.field_type == "float":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_number())",
                "    {",
                f'        OutError = "float field must be numeric: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = static_cast<float>({iterator_name}->get<double>());",
            ]
        )
        return lines

    if field.field_type == "double":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_number())",
                "    {",
                f'        OutError = "double field must be numeric: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = static_cast<double>({iterator_name}->get<double>());",
            ]
        )
        return lines

    if field.field_type == "bool":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_boolean())",
                "    {",
                f'        OutError = "bool field must be a boolean: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = {iterator_name}->get<bool>();",
            ]
        )
        return lines

    if field.field_type == "string":
        lines.extend(
            [
                f"    if (!{iterator_name}->is_string())",
                "    {",
                f'        OutError = "string field must be a string: {field_path}";',
                "        return false;",
                "    }",
                f"    OutRow.{field.name} = {iterator_name}->get<std::string>();",
            ]
        )
        return lines

    raise ValueError(f"unsupported field type: {field.field_type}")


def render_unreal_loaders(table: TableDefinition) -> list[str]:
    row_struct = f"F{table.table_name}Row"
    lines = [
        "// ============================================================",
        "// JSON Loaders (inline)",
        "// ============================================================",
        f"inline bool Load{table.table_name}Row(const TSharedPtr<FJsonObject>& Object,",
        f"                               {row_struct}& OutRow, FString& OutError)",
        "{",
        "    if (!Object.IsValid())",
        "    {",
        '        OutError = TEXT("table row must be a JSON object");',
        "        return false;",
        "    }",
        "",
    ]
    for field in table.fields:
        lines.extend(render_unreal_field_loader(table.table_name, field))
    lines.extend(
        [
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}(const FString& JsonText,",
            f"                            TArray<{row_struct}>& OutRows, FString& OutError)",
            "{",
            "    TArray<TSharedPtr<FJsonValue>> RootArray;",
            "    {",
            "        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);",
            "        if (FJsonSerializer::Deserialize(Reader, RootArray))",
            "        {",
            "            OutRows.Reset(RootArray.Num());",
            "            for (const TSharedPtr<FJsonValue>& Entry : RootArray)",
            "            {",
            "                const TSharedPtr<FJsonObject> EntryObject = Entry.IsValid() ? Entry->AsObject() : nullptr;",
            "                if (!EntryObject.IsValid())",
            "                {",
            '                    OutError = TEXT("table row must be a JSON object");',
            "                    return false;",
            "                }",
            "",
            f"                {row_struct} Row{{}};",
            f"                if (!Load{table.table_name}Row(EntryObject, Row, OutError))",
            "                {",
            "                    return false;",
            "                }",
            "                OutRows.Add(MoveTemp(Row));",
            "            }",
            "            return true;",
            "        }",
            "    }",
            "",
            "    TSharedPtr<FJsonObject> RootObject;",
            "    {",
            "        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);",
            "        if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())",
            "        {",
            '            OutError = TEXT("failed to parse table JSON");',
            "            return false;",
            "        }",
            "    }",
            "",
            "    const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;",
            f"    if (!RootObject->TryGetArrayField({cpp_text('rows')}, Rows) && !RootObject->TryGetArrayField({cpp_text('data')}, Rows))",
            "    {",
            '        OutError = TEXT("table root must be an array or an object with rows/data");',
            "        return false;",
            "    }",
            "",
            "    OutRows.Reset(Rows->Num());",
            "    for (const TSharedPtr<FJsonValue>& Entry : *Rows)",
            "    {",
            "        const TSharedPtr<FJsonObject> EntryObject = Entry.IsValid() ? Entry->AsObject() : nullptr;",
            "        if (!EntryObject.IsValid())",
            "        {",
            '            OutError = TEXT("table row must be a JSON object");',
            "            return false;",
            "        }",
            "",
            f"        {row_struct} Row{{}};",
            f"        if (!Load{table.table_name}Row(EntryObject, Row, OutError))",
            "        {",
            "            return false;",
            "        }",
            "        OutRows.Add(MoveTemp(Row));",
            "    }",
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}FromFile(const FString& FilePath,",
            f"                                    TArray<{row_struct}>& OutRows, FString& OutError)",
            "{",
            "    FString JsonText;",
            "    if (!FFileHelper::LoadFileToString(JsonText, *FilePath))",
            "    {",
            '        OutError = FString::Printf(TEXT("failed to read data file: %s"), *FilePath);',
            "        return false;",
            "    }",
            f"    return Load{table.table_name}(JsonText, OutRows, OutError);",
            "}",
            "",
        ]
    )
    return lines


def render_server_loaders(table: TableDefinition) -> list[str]:
    row_struct = f"{table.table_name}Row"
    lines = [
        "// ============================================================",
        "// JSON Loaders",
        "// ============================================================",
        f"inline bool Load{table.table_name}Row(const nlohmann::json& Object, {row_struct}& OutRow, std::string& OutError)",
        "{",
        "    if (!Object.is_object())",
        "    {",
        '        OutError = "table row must be a JSON object";',
        "        return false;",
        "    }",
        "",
    ]
    for field in table.fields:
        lines.extend(render_server_field_loader(table.table_name, field))
    lines.extend(
        [
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}(const std::string& JsonText, std::vector<{row_struct}>& OutRows, std::string& OutError)",
            "{",
            "    const nlohmann::json Root = nlohmann::json::parse(JsonText, nullptr, false);",
            "    if (Root.is_discarded())",
            "    {",
            '        OutError = "failed to parse table JSON";',
            "        return false;",
            "    }",
            "",
            "    const nlohmann::json* Rows = nullptr;",
            "    if (Root.is_array())",
            "    {",
            "        Rows = &Root;",
            "    }",
            '    else if (Root.is_object() && Root.contains("rows"))',
            "    {",
            '        Rows = &Root.at("rows");',
            "    }",
            '    else if (Root.is_object() && Root.contains("data"))',
            "    {",
            '        Rows = &Root.at("data");',
            "    }",
            "",
            "    if (Rows == nullptr || !Rows->is_array())",
            "    {",
            '        OutError = "table root must be an array or an object with rows/data";',
            "        return false;",
            "    }",
            "",
            "    OutRows.clear();",
            "    OutRows.reserve(Rows->size());",
            "    for (const auto& Entry : *Rows)",
            "    {",
            f"        {row_struct} Row{{}};",
            f"        if (!Load{table.table_name}Row(Entry, Row, OutError))",
            "        {",
            "            return false;",
            "        }",
            "        OutRows.push_back(std::move(Row));",
            "    }",
            "    return true;",
            "}",
            "",
            f"inline bool Load{table.table_name}FromFile(const std::string& FilePath, std::vector<{row_struct}>& OutRows, std::string& OutError)",
            "{",
            "    std::ifstream Input(FilePath);",
            "    if (!Input.is_open())",
            "    {",
            '        OutError = "failed to open data file";',
            "        return false;",
            "    }",
            "",
            "    std::ostringstream Buffer;",
            "    Buffer << Input.rdbuf();",
            f"    return Load{table.table_name}(Buffer.str(), OutRows, OutError);",
            "}",
            "",
        ]
    )
    return lines


def render_unreal_index_builders(table: TableDefinition) -> list[str]:
    if not table.keys:
        return []

    row_struct = f"F{table.table_name}Row"
    field_lookup = {field.name: field for field in table.fields}
    lines = [
        "// ============================================================",
        "// Index Builders",
        "// ============================================================",
    ]
    for key in table.keys:
        field = field_lookup[key.field_name]
        value_type = row_struct if key.unique else f"TArray<{row_struct}>"
        lines.extend(
            [
                f"inline TMap<{key_cpp_type(field, 'unreal')}, {value_type}> Build{table.table_name}IndexBy{key.field_name}(",
                f"    const TArray<{row_struct}>& Rows)",
                "{",
                f"    TMap<{key_cpp_type(field, 'unreal')}, {value_type}> Index;",
                "    for (const auto& Row : Rows)",
                "    {",
            ]
        )
        if key.unique:
            lines.append(f"        Index.Add(Row.{key.field_name}, Row);")
        else:
            lines.append(f"        Index.FindOrAdd(Row.{key.field_name}).Add(Row);")
        lines.extend(["    }", "    return Index;", "}", ""])
    return lines


def render_server_index_builders(table: TableDefinition) -> list[str]:
    if not table.keys:
        return []

    row_struct = f"{table.table_name}Row"
    field_lookup = {field.name: field for field in table.fields}
    lines = [
        "// ============================================================",
        "// Index Builders",
        "// ============================================================",
    ]
    for key in table.keys:
        field = field_lookup[key.field_name]
        value_type = row_struct if key.unique else f"std::vector<{row_struct}>"
        lines.extend(
            [
                f"inline std::unordered_map<{key_cpp_type(field, 'server')}, {value_type}> Build{table.table_name}IndexBy{key.field_name}(",
                f"    const std::vector<{row_struct}>& Rows)",
                "{",
                f"    std::unordered_map<{key_cpp_type(field, 'server')}, {value_type}> Index;",
                "    for (const auto& Row : Rows)",
                "    {",
            ]
        )
        if key.unique:
            lines.append(f"        Index[Row.{key.field_name}] = Row;")
        else:
            lines.append(f"        Index[Row.{key.field_name}].push_back(Row);")
        lines.extend(["    }", "    return Index;", "}", ""])
    return lines


def render_unreal_header(table: TableDefinition) -> str:
    lines = [
        "// Auto-generated. Do not edit.",
        "#pragma once",
        "",
        '#include "CoreMinimal.h"',
        '#include "Dom/JsonObject.h"',
        '#include "Misc/FileHelper.h"',
        '#include "Serialization/JsonReader.h"',
        '#include "Serialization/JsonSerializer.h"',
        # generated.h must be the last #include, placed BEFORE UENUM/USTRUCT definitions.
        f'#include "{table.table_name}Row.generated.h"',
        "",
    ]
    lines.extend(render_unreal_enums(table.enums))
    lines.extend(render_unreal_row_struct(table))
    lines.extend(render_unreal_loaders(table))
    lines.extend(render_unreal_index_builders(table))
    return "\n".join(lines).rstrip() + "\n"


def render_server_header(table: TableDefinition, namespace_name: str) -> str:
    lines = [
        "// Auto-generated. Do not edit.",
        "#pragma once",
        "",
        "#include <cstdint>",
        "#include <fstream>",
        "#include <nlohmann/json.hpp>",
        "#include <sstream>",
        "#include <string>",
        "#include <unordered_map>",
        "#include <utility>",
        "#include <vector>",
        "",
        f"namespace {namespace_name} {{",
        "",
    ]
    lines.extend(render_server_enums(table.enums))
    lines.extend(render_server_row_struct(table))
    lines.extend(render_server_loaders(table))
    lines.extend(render_server_index_builders(table))
    lines.extend([f"}} // namespace {namespace_name}", ""])
    return "\n".join(lines)


def render_server_manifest(table: TableDefinition, subdir: str, namespace_name: str) -> tuple[str, str]:
    manifest_path, constant_name, bundle_name, loader_name = build_manifest_identity(table, subdir)
    include_path = f"{subdir.strip()}/{table.snake_table_name}.h" if subdir.strip() else f"{table.snake_table_name}.h"
    row_struct = f"{table.table_name}Row"
    lines = [
        "// Auto-generated. Do not edit.",
        "#pragma once",
        "",
        "#include <array>",
        "#include <cstddef>",
        "#include <filesystem>",
        "#include <string>",
        "#include <string_view>",
        "#include <vector>",
        "",
        f'#include "{include_path}"',
        "",
        f"namespace {namespace_name} {{",
        "",
        "struct FTableDescriptor",
        "{",
        "    std::string_view TableName;",
        "    std::string_view SchemaFile;",
        "    std::string_view DataFile;",
        "    std::size_t RowCount;",
        "    std::size_t FieldCount;",
        "};",
        "",
        f"inline constexpr std::array<FTableDescriptor, 1> {constant_name} = {{",
        f'    FTableDescriptor{{"{table.table_name}", "{table.schema_path.name}", "{table.data_path.name}", {table.row_count}, {len(table.fields)}}},',
        "};",
        "",
        f"struct {bundle_name}",
        "{",
        f"    std::vector<{row_struct}> {table.table_name}Rows;",
        "};",
        "",
        f"inline bool {loader_name}(const std::filesystem::path& Directory, {bundle_name}& OutBundle, std::string& OutError)",
        "{",
        f'    return Load{table.table_name}FromFile((Directory / "{table.data_path.name}").string(), OutBundle.{table.table_name}Rows, OutError);',
        "}",
        "",
        f"}} // namespace {namespace_name}",
        "",
    ]
    return manifest_path, "\n".join(lines)


def copy_data_file(source_path: Path, destination_dir: Path) -> Path:
    destination_dir.mkdir(parents=True, exist_ok=True)
    destination_path = destination_dir / source_path.name
    shutil.copyfile(source_path, destination_path)
    return destination_path


def main() -> int:
    args = parse_args()
    schema_path = resolve_schema_path(args)
    data_path = resolve_data_path(schema_path, args.data_src)
    table = load_table_definition(schema_path=schema_path, data_path=data_path)
    namespace_name = namespace_for_subdir(args.subdir)

    server_output_dir = args.server_out.resolve()
    server_header_dir = server_output_dir / args.subdir if args.subdir else server_output_dir
    unreal_output_dir = args.unreal_out.resolve()
    data_output_dir = args.data_out.resolve()

    server_header_path = server_header_dir / f"{table.snake_table_name}.h"
    unreal_header_path = unreal_output_dir / f"{table.table_name}Row.h"
    manifest_relative_path, manifest_content = render_server_manifest(table=table, subdir=args.subdir, namespace_name=namespace_name)
    manifest_path = server_output_dir / manifest_relative_path

    write_text(server_header_path, render_server_header(table=table, namespace_name=namespace_name))
    write_text(unreal_header_path, render_unreal_header(table=table))
    write_text(manifest_path, manifest_content)
    copied_data_path = copy_data_file(data_path, data_output_dir)

    print(f"Generated server header: {server_header_path}")
    print(f"Generated manifest: {manifest_path}")
    print(f"Generated Unreal header: {unreal_header_path}")
    print(f"Copied data: {copied_data_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
