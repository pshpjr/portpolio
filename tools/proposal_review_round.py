#!/usr/bin/env python3
"""Run multiple OpenCode review jobs and write a markdown report."""

from __future__ import annotations

import argparse
import base64
import json
import os
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from urllib.error import HTTPError, URLError
from urllib.parse import urlencode
from urllib.request import Request, urlopen

REPO_ROOT = Path(__file__).resolve().parents[1]
OPENCODE_BASE_URL = "http://127.0.0.1:4096"
OPENCODE_USERNAME = "opencode"
DEFAULT_PROVIDER_ID = "zai-coding-plan"
DEFAULT_TIMEOUT_SECONDS = 600

EXIT_SUCCESS = 0
EXIT_CONFIG_ERROR = 3
EXIT_HTTP_ERROR = 4
EXIT_OUTPUT_ERROR = 5


@dataclass(frozen=True)
class JobConfig:
    persona: str
    model_id: str
    provider_id: str
    prompt: str
    files: list[Path]
    file_labels: list[str]


@dataclass(frozen=True)
class Config:
    title: str
    output_path: Path
    jobs: list[JobConfig]


class ConfigError(RuntimeError):
    pass


class OpenCodeError(RuntimeError):
    pass


def get_opencode_base_url() -> str:
    return os.environ.get("OPENCODE_SERVER_URL", OPENCODE_BASE_URL)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run multiple OpenCode review jobs and write a markdown report.",
    )
    parser.add_argument(
        "--config",
        required=True,
        help="Path to a JSON config file.",
    )
    return parser.parse_args(argv)


def normalize_text(value: Any, field_name: str) -> str:
    if not isinstance(value, str) or not value.strip():
        raise ConfigError(f"{field_name} must be a non-empty string")
    return value


def normalize_string_list(value: Any, field_name: str) -> list[str]:
    if not isinstance(value, list):
        raise ConfigError(f"{field_name} must be an array")
    items: list[str] = []
    for index, item in enumerate(value, start=1):
        if not isinstance(item, str) or not item.strip():
            raise ConfigError(f"{field_name}[{index}] must be a non-empty string")
        items.append(item)
    return items


def load_config(config_path: Path) -> Config:
    try:
        raw = json.loads(config_path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ConfigError(f"config file not found: {config_path}") from exc
    except UnicodeDecodeError as exc:
        raise ConfigError(f"config file is not valid UTF-8: {config_path}") from exc
    except json.JSONDecodeError as exc:
        raise ConfigError(f"invalid JSON in config file: {config_path}: {exc}") from exc

    if not isinstance(raw, dict):
        raise ConfigError("config root must be a JSON object")

    title = normalize_text(raw.get("title"), "title")
    output_path_raw = normalize_text(raw.get("output_path"), "output_path")

    jobs_raw = raw.get("jobs")
    if not isinstance(jobs_raw, list) or not jobs_raw:
        raise ConfigError("jobs must be a non-empty array")

    config_dir = config_path.parent.resolve()
    jobs: list[JobConfig] = []
    for index, job_raw in enumerate(jobs_raw, start=1):
        if not isinstance(job_raw, dict):
            raise ConfigError(f"jobs[{index}] must be an object")

        persona = normalize_text(job_raw.get("persona"), f"jobs[{index}].persona")
        model_id = normalize_text(job_raw.get("model_id"), f"jobs[{index}].model_id")
        provider_id = job_raw.get("provider_id", DEFAULT_PROVIDER_ID)
        provider_id = normalize_text(provider_id, f"jobs[{index}].provider_id")
        prompt = normalize_text(job_raw.get("prompt"), f"jobs[{index}].prompt")
        file_labels = normalize_string_list(job_raw.get("files", []), f"jobs[{index}].files")

        files = []
        for file_label in file_labels:
            file_path = Path(file_label)
            if not file_path.is_absolute():
                file_path = (config_dir / file_path).resolve()
            files.append(file_path)

        jobs.append(
            JobConfig(
                persona=persona,
                model_id=model_id,
                provider_id=provider_id,
                prompt=prompt,
                files=files,
                file_labels=file_labels,
            )
        )

    output_path = Path(output_path_raw)
    if not output_path.is_absolute():
        output_path = (config_dir / output_path).resolve()

    return Config(title=title, output_path=output_path, jobs=jobs)


def read_job_files(job: JobConfig) -> list[tuple[str, str]]:
    file_entries: list[tuple[str, str]] = []
    for label, file_path in zip(job.file_labels, job.files):
        if not file_path.exists():
            raise ConfigError(f"job file does not exist: {label} -> {file_path}")
        if not file_path.is_file():
            raise ConfigError(f"job file is not a regular file: {label} -> {file_path}")
        try:
            content = file_path.read_text(encoding="utf-8")
        except UnicodeDecodeError as exc:
            raise ConfigError(f"job file is not valid UTF-8: {file_path}") from exc
        file_entries.append((label, content))
    return file_entries


def fenced_block(text: str, language: str = "text") -> str:
    fence = "```"
    if "```" in text:
        fence = "````"
    return f"{fence}{language}\n{text}\n{fence}"


def build_prompt(job: JobConfig, file_entries: list[tuple[str, str]]) -> str:
    lines = [
        f"You are acting as: {job.persona}",
        "",
        "Review task:",
        job.prompt,
        "",
        "Review the following files:",
    ]
    for label, content in file_entries:
        lines.append(f"### File: {label}")
        lines.append(fenced_block(content))
        lines.append("")
    lines.append("Return concise, actionable findings.")
    return "\n".join(lines).rstrip() + "\n"


def basic_auth_header(password: str) -> str:
    token = base64.b64encode(f"{OPENCODE_USERNAME}:{password}".encode("utf-8")).decode("ascii")
    return f"Basic {token}"


def post_json(url: str, body: dict[str, Any], password: str, timeout: int = DEFAULT_TIMEOUT_SECONDS) -> Any:
    data = json.dumps(body, ensure_ascii=False).encode("utf-8")
    request = Request(url, data=data, method="POST")
    request.add_header("Content-Type", "application/json; charset=utf-8")
    request.add_header("Accept", "application/json")
    request.add_header("Authorization", basic_auth_header(password))

    try:
        with urlopen(request, timeout=timeout) as response:
            payload = response.read().decode("utf-8")
    except HTTPError as exc:
        error_text = exc.read().decode("utf-8", errors="replace")
        raise OpenCodeError(f"HTTP {exc.code} for {url}: {error_text or exc.reason}") from exc
    except URLError as exc:
        raise OpenCodeError(f"failed to reach OpenCode server at {url}: {exc.reason}") from exc

    try:
        return json.loads(payload) if payload else None
    except json.JSONDecodeError as exc:
        raise OpenCodeError(f"OpenCode returned invalid JSON for {url}: {exc}") from exc


def create_session(title: str, password: str) -> str:
    base_url = get_opencode_base_url()
    url = f"{base_url}/session"
    query = urlencode({"directory": str(REPO_ROOT)})
    payload = {"title": title}
    response = post_json(f"{url}?{query}", payload, password)
    session_id = response.get("id") if isinstance(response, dict) else None
    if not isinstance(session_id, str) or not session_id:
        raise OpenCodeError(f"OpenCode session create response did not include an id: {response!r}")
    return session_id


def send_prompt(session_id: str, job: JobConfig, prompt: str, password: str) -> Any:
    base_url = get_opencode_base_url()
    url = f"{base_url}/session/{session_id}/message"
    query = urlencode({"directory": str(REPO_ROOT)})
    body = {
        "model": {
            "providerID": job.provider_id,
            "modelID": job.model_id,
        },
        "parts": [
            {
                "type": "text",
                "text": prompt,
            }
        ],
    }
    return post_json(f"{url}?{query}", body, password)


def collect_text_parts(node: Any) -> list[str]:
    parts: list[str] = []
    if isinstance(node, dict):
        text_value = node.get("text")
        if isinstance(text_value, str) and text_value:
            parts.append(text_value)
        else:
            content_value = node.get("content")
            if isinstance(content_value, str) and content_value:
                parts.append(content_value)
        for value in node.values():
            if isinstance(value, (dict, list)):
                parts.extend(collect_text_parts(value))
    elif isinstance(node, list):
        for item in node:
            parts.extend(collect_text_parts(item))

    return parts


def render_job_section(index: int, job: JobConfig, session_id: str, response: Any) -> str:
    text_parts = collect_text_parts(response)
    if not text_parts:
        text_parts = ["_(No text parts returned by OpenCode.)_"]

    response_text = "\n\n".join(text_parts).strip()
    file_list = ", ".join(f"`{label}`" for label in job.file_labels) if job.file_labels else "_No files_"

    lines = [
        f"## Job {index}: {job.persona}",
        "",
        f"- Session: `{session_id}`",
        f"- Model: `{job.provider_id}/{job.model_id}`",
        f"- Files: {file_list}",
        "",
        "### Result",
        "",
        response_text,
        "",
    ]
    return "\n".join(lines)


def render_report(title: str, job_sections: list[str]) -> str:
    timestamp = datetime.now(timezone.utc).astimezone().isoformat(timespec="seconds")
    lines = [
        f"# {title}",
        "",
        f"- Generated: {timestamp}",
        f"- Jobs: {len(job_sections)}",
        "",
    ]
    lines.extend(job_sections)
    return "\n".join(lines).rstrip() + "\n"


def write_report(output_path: Path, title: str, job_sections: list[str]) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(render_report(title, job_sections), encoding="utf-8")


def run(config: Config) -> int:
    password = os.environ.get("OPENCODE_SERVER_PASSWORD")
    if not password:
        print("error: OPENCODE_SERVER_PASSWORD environment variable is required", file=sys.stderr)
        return EXIT_CONFIG_ERROR

    job_sections: list[str] = []
    try:
        for index, job in enumerate(config.jobs, start=1):
            file_entries = read_job_files(job)
            prompt = build_prompt(job, file_entries)
            session_title = f"{config.title} - {job.persona}"
            session_id = create_session(session_title, password)
            response = send_prompt(session_id, job, prompt, password)
            job_sections.append(render_job_section(index, job, session_id, response))
            write_report(config.output_path, config.title, job_sections)
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return EXIT_CONFIG_ERROR
    except OpenCodeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return EXIT_HTTP_ERROR
    except OSError as exc:
        print(f"error: failed to write report: {exc}", file=sys.stderr)
        return EXIT_OUTPUT_ERROR

    print(f"OK: wrote report to {config.output_path}")
    return EXIT_SUCCESS


def main(argv: list[str]) -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")

    args = parse_args(argv)
    try:
        config = load_config(Path(args.config).resolve())
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return EXIT_CONFIG_ERROR

    return run(config)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
