"""Editor startup hook for the icon import pipeline.

Runs once when the editor starts (PythonScriptPlugin auto-executes any
`init_unreal.py` under `Content/Python/`). Hashes the icon manifest and
re-runs the importer only when the hash differs from the last run's cache.

Hash cache lives under `Saved/IconPipeline/last_manifest.sha256` so it stays
out of version control and is per-user.
"""

from __future__ import annotations

import hashlib
import os

import unreal

import import_icons

LOG_TAG = "[IconPipeline]"
HASH_FILE_REL = os.path.join("IconPipeline", "last_manifest.sha256")


def _log(msg: str) -> None:
    unreal.log("{0} {1}".format(LOG_TAG, msg))


def _warn(msg: str) -> None:
    unreal.log_warning("{0} {1}".format(LOG_TAG, msg))


def _saved_dir() -> str:
    return unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_saved_dir())


def _hash_cache_path() -> str:
    return os.path.normpath(os.path.join(_saved_dir(), HASH_FILE_REL))


def _compute_manifest_hash() -> str | None:
    path = import_icons._manifest_path()  # intentional: same-module helper reuse
    if not os.path.exists(path):
        return None
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def _load_cached_hash() -> str | None:
    path = _hash_cache_path()
    if not os.path.exists(path):
        return None
    try:
        with open(path, "r", encoding="utf-8") as f:
            return f.read().strip() or None
    except OSError as exc:
        _warn("failed to read hash cache: {0}".format(exc))
        return None


def _store_hash(digest: str) -> None:
    path = _hash_cache_path()
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        f.write(digest)


def _maybe_run() -> None:
    current = _compute_manifest_hash()
    if current is None:
        _log("manifest missing, skipping startup import")
        return

    cached = _load_cached_hash()
    if cached == current:
        _log("manifest unchanged, no import needed")
        return

    _log("manifest changed (was={0}, now={1}), running import".format(
        (cached or "none")[:8], current[:8]))

    succeeded, failed = import_icons.run(force=False)
    # Only cache the hash if nothing failed, so the next startup retries failures.
    if failed == 0:
        _store_hash(current)
    else:
        _warn("{0} entries failed; hash not cached so next startup retries".format(failed))


try:
    _maybe_run()
except Exception as exc:
    unreal.log_error("{0} startup hook raised: {1}".format(LOG_TAG, exc))
