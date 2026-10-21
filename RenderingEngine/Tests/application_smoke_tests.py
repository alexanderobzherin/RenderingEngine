#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
"""Build and launch existing Rendering Engine example/test applications.

Runs on Linux and FreeBSD. Verifies successful compilation and that
applications remain running during a short observation period.
Requires a graphical environment; not intended for headless CI.
"""

import argparse
import os
from pathlib import Path
import signal
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
APPLICATION_FOLDERS = ("ContentExamples", "TestApplications")
EXCLUDED_APPLICATIONS = {"TextureBenchmark"}


def run_build(command, cwd):
    print(f"\n$ {' '.join(map(str, command))}", flush=True)
    result = subprocess.run(command, cwd=cwd, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"Build failed (exit code {result.returncode}): {command}")


def discover_applications():
    apps = []
    for folder in APPLICATION_FOLDERS:
        parent = ROOT / folder
        if not parent.is_dir():
            raise RuntimeError(f"Missing directory: {parent}")
        for project in sorted(parent.iterdir()):
            if project.name in EXCLUDED_APPLICATIONS:
                continue
            if project.is_dir() and (project / "build_project.sh").is_file():
                apps.append(project)
    if not apps:
        raise RuntimeError("No application build scripts found")
    return apps


def run_application(executable, seconds, shutdown_timeout):
    # Child uses its own process group so cleanup also handles descendants.
    with open(os.devnull, "rb") as devnull:
        process = subprocess.Popen(
            [str(executable)], cwd=executable.parent, stdin=devnull,
            start_new_session=True,
        )
        try:
            try:
                code = process.wait(timeout=seconds)
            except subprocess.TimeoutExpired:
                code = None

            if code is not None:
                if code == 0:
                    return "PASS", "Exited normally (code 0)"
                return "FAIL", f"Exited unexpectedly (code {code})"

            # The GUI is still alive after the observation window.
            os.killpg(process.pid, signal.SIGTERM)
            try:
                code = process.wait(timeout=shutdown_timeout)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
                return "FAIL", "Did not stop after SIGTERM; SIGKILL required"

            if code in (-signal.SIGTERM, 128 + signal.SIGTERM):
                return "PASS", f"Stayed alive {seconds:g}s; terminated by test runner (code {code})"
            if code == 0:
                return "PASS", "Stayed alive; shutdown returned code 0"
            return "FAIL", f"Unexpected exit code during shutdown: {code}"
        finally:
            if process.poll() is None:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--release", action="store_true", help="Build Release rather than Debug")
    parser.add_argument("--seconds", type=float, default=3.0, help="GUI observation time (default 3)")
    parser.add_argument("--shutdown-timeout", type=float, default=3.0)
    parser.add_argument("--skip-engine-build", action="store_true", help="Reuse existing engine build")
    args = parser.parse_args()
    if os.name != "posix" or not (
        sys.platform == "linux" or sys.platform.startswith("freebsd")
    ):
        parser.error("This smoke test supports Linux and FreeBSD only")
    if args.seconds <= 0 or args.shutdown_timeout <= 0:
        parser.error("Timeouts must be positive")

    build_type = "Release" if args.release else "Debug"
    build_flag = "--release" if args.release else "--debug"
    apps = discover_applications()
    print(f"Rendering Engine root: {ROOT}")
    print(f"Build configuration: {build_type}")
    print(f"Applications found: {len(apps)}", flush=True)

    if not args.skip_engine_build:
        run_build(["sh", str(ROOT / "build_engine.sh"), "--engine-only", build_flag], ROOT)

    results = []
    for app in apps:
        print(f"\n=== {app.name} ===", flush=True)
        try:
            run_build(["sh", str(app / "build_project.sh"), build_flag], app)
            executable = ROOT / "Build" / "UserApplications" / app.name / build_type / app.name
            if not executable.is_file():
                raise RuntimeError(f"Executable not found: {executable}")
            if not os.access(executable, os.X_OK):
                raise RuntimeError(f"Executable not executable: {executable}")
            status, reason = run_application(executable, args.seconds, args.shutdown_timeout)
        except (OSError, RuntimeError) as exc:
            status, reason = "FAIL", str(exc)
        results.append((app.name, status, reason))
        print(f"{status}: {app.name}: {reason}", flush=True)

    print("\n=== Summary ===")
    for name, status, reason in results:
        print(f"{status:4} {name}: {reason}")
    passed = sum(status == "PASS" for _, status, _ in results)
    print(f"Passed {passed}/{len(results)}")
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

