#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
"""Unix developer-mode and packaged-SDK project workflow smoke tests."""

import argparse
from contextlib import contextmanager
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import tempfile

from application_smoke_tests import ROOT, run_application, run_build

SCRIPTS = ROOT / "RenderingEngine" / "Scripts"
INTERNAL_ROOT = ROOT / "UserApplications"
INTERNAL_CMAKE = INTERNAL_ROOT / "CMakeLists.txt"
SOURCE_REGISTRY = ROOT / "Local" / "registered_projects.cmake"


@contextmanager
def preserve_file(path):
    """Restore an existing file byte-for-byte, or remove a newly created file."""
    existed = path.is_file()
    original = path.read_bytes() if existed else None
    try:
        yield
    finally:
        if existed:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(original)
        elif path.exists():
            path.unlink()


def verify_project(project):
    if not project.is_dir():
        raise RuntimeError(f"Project was not created: {project}")
    for name in ("CMakeLists.txt", "build_project.sh"):
        if not (project / name).is_file():
            raise RuntimeError(f"Missing generated file: {project / name}")


def run_checked(command, cwd, required_output=()):
    """Execute a command, show its output, and verify expected CMake messages."""
    print(f"\n$ {' '.join(map(str, command))}", flush=True)
    result = subprocess.run(
        command, cwd=cwd, stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, text=True, errors="replace", check=False,
    )
    print(result.stdout, end="" if result.stdout.endswith("\n") else "\n", flush=True)
    if result.returncode != 0:
        raise RuntimeError(f"Command failed (exit code {result.returncode}): {command}")
    for expected in required_output:
        if expected not in result.stdout:
            raise RuntimeError(f"Expected build output not found: {expected}")


def verify_registration(project, engine_root):
    config = project / "Local" / "rendering_engine.cmake"
    if not config.is_file():
        raise RuntimeError(f"Missing project registration: {config}")
    expected = str(engine_root.resolve()).replace("\\", "/")
    if f'"{expected}"' not in config.read_text():
        raise RuntimeError(f"Project registration does not point to {expected}: {config}")


def build_and_run(project, build_type, build_flag, seconds, shutdown_timeout,
                  external, expected_engine=None, expected_dev_mode=None):
    command = ["sh", str(project / "build_project.sh"), build_flag]
    if expected_engine is None:
        run_build(command, project)
    else:
        run_checked(command, project, (
            f"RenderingEngine: using registered engine at {expected_engine}",
            f"RenderingEngine: RE_DEV_MODE={expected_dev_mode}",
        ))

    build_root = (project / "Build" if external else
                  ROOT / "Build" / "UserApplications" / project.name)
    executable = build_root / build_type / project.name
    if not executable.is_file() or not os.access(executable, os.X_OK):
        raise RuntimeError(f"Executable missing or not executable: {executable}")
    status, reason = run_application(executable, seconds, shutdown_timeout)
    if status != "PASS":
        raise RuntimeError(reason)
    return reason


def test_internal(build_type, build_flag, seconds, shutdown_timeout):
    name = "HelloApp1"
    project = INTERNAL_ROOT / name
    build_dir = ROOT / "Build" / "UserApplications" / name
    if project.exists() or project.is_symlink():
        raise RuntimeError(f"Refusing to overwrite existing project: {project}")
    if build_dir.exists() or build_dir.is_symlink():
        raise RuntimeError(f"Refusing to overwrite existing build: {build_dir}")
    if not INTERNAL_CMAKE.is_file():
        raise RuntimeError(f"Missing internal CMake file: {INTERNAL_CMAKE}")

    with preserve_file(INTERNAL_CMAKE):
        try:
            run_build([sys.executable, str(SCRIPTS / "create_project.py"), name], ROOT)
            verify_project(project)
            return build_and_run(project, build_type, build_flag,
                                 seconds, shutdown_timeout, False)
        finally:
            if project.is_dir() and not project.is_symlink():
                shutil.rmtree(project)
            if build_dir.is_dir() and not build_dir.is_symlink():
                shutil.rmtree(build_dir)


def test_external(build_type, build_flag, seconds, shutdown_timeout):
    name = "HelloApp1"
    with tempfile.TemporaryDirectory(prefix="rendering-engine-workflow-") as temp:
        destination = Path(temp)
        project = destination / name
        with preserve_file(SOURCE_REGISTRY):
            run_build([sys.executable, str(SCRIPTS / "create_project.py"),
                       name, str(destination)], ROOT)
            verify_project(project)
            verify_registration(project, ROOT)
            return build_and_run(project, build_type, build_flag,
                                 seconds, shutdown_timeout, True, ROOT, "ON")


def discover_archive(archive_arg):
    if archive_arg:
        archive = Path(archive_arg).expanduser().resolve()
        if not archive.is_file():
            raise RuntimeError(f"SDK archive not found: {archive}")
        return archive
    platform = "FreeBSD" if sys.platform.startswith("freebsd") else "Linux"
    candidates = sorted((ROOT / "Build" / "Packages").glob(
        f"RenderingEngine-*-SDK-{platform}.tar.gz"))
    if len(candidates) != 1:
        raise RuntimeError(
            f"Expected exactly one {platform} SDK archive under Build/Packages; "
            f"found {len(candidates)}. Specify --sdk-archive explicitly."
        )
    return candidates[0]


def extract_sdk(archive, destination):
    """Extract a trusted locally produced SDK archive, rejecting unsafe paths."""
    with tarfile.open(archive, "r:gz") as tar:
        members = tar.getmembers()
        roots = {Path(member.name).parts[0] for member in members if Path(member.name).parts}
        if len(roots) != 1:
            raise RuntimeError(f"Expected one top-level SDK directory: {roots}")
        root_name = roots.pop()
        target_root = (destination / root_name).resolve()
        for member in members:
            target = (destination / member.name).resolve()
            if not target.is_relative_to(target_root):
                raise RuntimeError(f"Unsafe archive path: {member.name}")
            if member.issym() or member.islnk() or member.isdev():
                raise RuntimeError(f"Unsupported archive entry: {member.name}")
        # The SDK is produced locally; use the validated member set.
        tar.extractall(destination, members=members, filter="data")
    sdk = destination / root_name
    for relative in (
        "RenderingEngine/Scripts/create_project.py",
        "RenderingEngine/Scripts/register_project.py",
        "RenderingEngine/RenderingLibrary/Library/libRenderingEngine.so",
    ):
        if not (sdk / relative).is_file():
            raise RuntimeError(f"SDK is missing {relative}")
    return sdk


def test_sdk_created(sdk, workspace, build_type, build_flag, seconds, shutdown_timeout):
    name = "HelloApp2"
    destination = workspace / "sdk-created"
    destination.mkdir()
    project = destination / name
    sdk_scripts = sdk / "RenderingEngine" / "Scripts"
    sdk_registry = sdk / "Local" / "registered_projects.cmake"
    with preserve_file(sdk_registry):
        run_build([sys.executable, str(sdk_scripts / "create_project.py"),
                   name, str(destination)], sdk)
        verify_project(project)
        verify_registration(project, sdk)
        return build_and_run(project, build_type, build_flag,
                             seconds, shutdown_timeout, True, sdk, "OFF")


def test_sdk_registered(sdk, workspace, build_type, build_flag, seconds, shutdown_timeout):
    name = "HelloApp3"
    destination = workspace / "source-created"
    destination.mkdir()
    project = destination / name
    sdk_scripts = sdk / "RenderingEngine" / "Scripts"
    sdk_registry = sdk / "Local" / "registered_projects.cmake"
    with preserve_file(SOURCE_REGISTRY), preserve_file(sdk_registry):
        run_build([sys.executable, str(SCRIPTS / "create_project.py"),
                   name, str(destination)], ROOT)
        verify_project(project)
        verify_registration(project, ROOT)
        run_build([sys.executable, str(sdk_scripts / "register_project.py"),
                   str(project)], sdk)
        verify_registration(project, sdk)
        return build_and_run(project, build_type, build_flag,
                             seconds, shutdown_timeout, True, sdk, "OFF")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--release", action="store_true", help="Build Release instead of Debug")
    parser.add_argument("--seconds", type=float, default=3.0, help="GUI observation time")
    parser.add_argument("--shutdown-timeout", type=float, default=3.0)
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--internal-only", action="store_true")
    group.add_argument("--external-only", action="store_true")
    group.add_argument("--sdk-only", action="store_true")
    parser.add_argument("--sdk-archive", help="Explicit path to packaged SDK .tar.gz")
    parser.add_argument("--skip-sdk", action="store_true", help="Run developer-mode tests only")
    args = parser.parse_args()
    if os.name != "posix" or not (sys.platform == "linux" or sys.platform.startswith("freebsd")):
        parser.error("This test supports Linux and FreeBSD only")
    if args.seconds <= 0 or args.shutdown_timeout <= 0:
        parser.error("Timeouts must be positive")
    if args.sdk_only and args.skip_sdk:
        parser.error("--sdk-only and --skip-sdk cannot be combined")

    build_type = "Release" if args.release else "Debug"
    build_flag = "--release" if args.release else "--debug"
    results = []

    def execute(name, test):
        print(f"\n=== {name} ===", flush=True)
        try:
            reason = test()
            status = "PASS"
        except (OSError, RuntimeError, tarfile.TarError) as exc:
            status, reason = "FAIL", str(exc)
        results.append((name, status, reason))
        print(f"{status}: {name}: {reason}", flush=True)

    print(f"Rendering Engine root: {ROOT}")
    print(f"Build configuration: {build_type}")
    print("Prerequisite: the engine and SDK archive are already built.", flush=True)

    if not args.sdk_only and not args.external_only:
        execute("Internal project creation", lambda: test_internal(
            build_type, build_flag, args.seconds, args.shutdown_timeout))
    if not args.sdk_only and not args.internal_only:
        execute("External project creation", lambda: test_external(
            build_type, build_flag, args.seconds, args.shutdown_timeout))

    if not (args.skip_sdk or args.internal_only or args.external_only):
        try:
            archive = discover_archive(args.sdk_archive)
            print(f"SDK archive: {archive}", flush=True)
            # All test artifacts stay under Build; the archive is not modified.
            with tempfile.TemporaryDirectory(prefix="sdk-workflow-", dir=ROOT / "Build") as temp:
                workspace = Path(temp)
                sdk = extract_sdk(archive, workspace)
                print(f"Extracted SDK: {sdk}", flush=True)
                execute("SDK project creation", lambda: test_sdk_created(
                    sdk, workspace, build_type, build_flag, args.seconds, args.shutdown_timeout))
                execute("SDK registration", lambda: test_sdk_registered(
                    sdk, workspace, build_type, build_flag, args.seconds, args.shutdown_timeout))
        except (OSError, RuntimeError, tarfile.TarError, ValueError) as exc:
            execute("SDK preparation", lambda: (_ for _ in ()).throw(RuntimeError(str(exc))))

    print("\n=== Summary ===")
    for name, status, reason in results:
        print(f"{status:4} {name}: {reason}")
    passed = sum(status == "PASS" for _, status, _ in results)
    print(f"Passed {passed}/{len(results)}")
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main())