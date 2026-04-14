"""Icon import pipeline for the client UE project.

Reads `client/tools/icon_staging/manifest.json` and imports each listed PNG
into `/Game/UI/Icons/<Category>/T_<IconKey>` as a UTexture2D.

Invoked automatically by `init_unreal.py` at editor startup when the manifest
hash changed, and manually by `tools/sync_icons.bat` with `--force`.

All logs are tagged `[IconPipeline]` for easy filtering in Output Log.
"""

from __future__ import annotations

import json
import os
import sys
from typing import Iterable

import unreal

LOG_TAG = "[IconPipeline]"
ALLOWED_LICENSES = {"CC0", "CC-BY", "CC-BY-SA", "Custom"}
# Must stay in sync with EItemCategory in Source/client/Public/Generated/ItemTableRow.h.
# `None` is excluded because unassigned items have no meaningful icon folder.
ALLOWED_CATEGORIES = {
    "Weapon",
    "Armor",
    "Consumable",
    "SkillBook",
    "Material",
    "Quest",
    "Currency",
}
PACKAGE_ROOT = "/Game/UI/Icons"


def _project_dir() -> str:
    # `.../client/Intermediate/...` or `.../client/` — normalize to project root.
    return unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())


def _manifest_path() -> str:
    return os.path.normpath(os.path.join(_project_dir(), "tools", "icon_staging", "manifest.json"))


def _staging_dir() -> str:
    return os.path.normpath(os.path.join(_project_dir(), "tools", "icon_staging"))


def _log(msg: str) -> None:
    unreal.log("{0} {1}".format(LOG_TAG, msg))


def _warn(msg: str) -> None:
    unreal.log_warning("{0} {1}".format(LOG_TAG, msg))


def _error(msg: str) -> None:
    unreal.log_error("{0} {1}".format(LOG_TAG, msg))


def _load_manifest() -> dict:
    path = _manifest_path()
    if not os.path.exists(path):
        _warn("manifest.json not found at {0}".format(path))
        return {"version": 1, "entries": []}
    with open(path, "r", encoding="utf-8") as f:
        data = json.load(f)
    if data.get("version") != 1:
        raise RuntimeError("unsupported manifest version: {0}".format(data.get("version")))
    return data


def _validate_entry(entry: dict) -> list[str]:
    errors: list[str] = []
    for key in ("IconKey", "Category", "Source", "License"):
        if not entry.get(key):
            errors.append("missing field '{0}'".format(key))
    license_value = entry.get("License", "")
    if license_value and license_value not in ALLOWED_LICENSES:
        errors.append("license '{0}' not in whitelist {1}".format(license_value, sorted(ALLOWED_LICENSES)))
    category_value = entry.get("Category", "")
    if category_value and category_value not in ALLOWED_CATEGORIES:
        errors.append("Category '{0}' not in EItemCategory whitelist {1}".format(
            category_value, sorted(ALLOWED_CATEGORIES)))
    icon_key = entry.get("IconKey", "")
    if icon_key and not icon_key.replace("_", "").isalnum():
        errors.append("IconKey must be snake_case alphanumeric, got '{0}'".format(icon_key))
    return errors


def _asset_package_path(category: str, icon_key: str) -> str:
    return "{0}/{1}/T_{2}".format(PACKAGE_ROOT, category, icon_key)


def _destination_folder(category: str) -> str:
    return "{0}/{1}".format(PACKAGE_ROOT, category)


def _import_texture(source_abs: str, category: str, icon_key: str) -> bool:
    """Import or reimport a single PNG as UTexture2D. Returns True on success."""
    destination_folder = _destination_folder(category)
    asset_name = "T_{0}".format(icon_key)
    package_path = _asset_package_path(category, icon_key)

    task = unreal.AssetImportTask()
    task.filename = source_abs
    task.destination_path = destination_folder
    task.destination_name = asset_name
    task.replace_existing = True
    task.automated = True
    task.save = True

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset_tools.import_asset_tasks([task])

    imported = task.get_editor_property("imported_object_paths")
    if not imported:
        _error("import failed: {0} -> {1}".format(source_abs, package_path))
        return False
    _log("imported {0} -> {1}".format(os.path.basename(source_abs), package_path))
    return True


def _iter_entries(manifest: dict) -> Iterable[dict]:
    entries = manifest.get("entries") or []
    seen: dict[str, int] = {}
    for idx, entry in enumerate(entries):
        key = entry.get("IconKey", "")
        if key and key in seen:
            _warn("duplicate IconKey '{0}' — last entry wins (index {1} overrides {2})".format(key, idx, seen[key]))
        seen[key] = idx
        yield entry


def run(force: bool = False) -> tuple[int, int]:
    """Run the import pipeline. Returns (succeeded, failed)."""
    manifest = _load_manifest()
    entries = list(_iter_entries(manifest))
    if not entries:
        _log("manifest empty, nothing to import")
        return (0, 0)

    staging = _staging_dir()
    succeeded = 0
    failed = 0

    for idx, entry in enumerate(entries):
        errors = _validate_entry(entry)
        if errors:
            _warn("entry #{0} skipped: {1}".format(idx, "; ".join(errors)))
            failed += 1
            continue

        source_rel = entry["Source"]
        source_abs = os.path.normpath(os.path.join(staging, source_rel))
        if not os.path.exists(source_abs):
            _warn("entry '{0}' source not found: {1}".format(entry["IconKey"], source_abs))
            failed += 1
            continue

        try:
            ok = _import_texture(source_abs, entry["Category"], entry["IconKey"])
        except Exception as exc:  # keep the pipeline going even if one entry throws
            _error("entry '{0}' raised: {1}".format(entry["IconKey"], exc))
            failed += 1
            continue

        if ok:
            succeeded += 1
        else:
            failed += 1

    _log("done: {0} succeeded, {1} failed".format(succeeded, failed))
    return (succeeded, failed)


def _parse_force_flag(argv: list[str]) -> bool:
    return any(a in ("--force", "-f") for a in argv)


if __name__ == "__main__":
    run(force=_parse_force_flag(sys.argv))
