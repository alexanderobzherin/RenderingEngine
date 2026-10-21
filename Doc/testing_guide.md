# Testing Guide

Rendering Engine uses automated tests to validate library behavior, application execution, and project workflows across supported platforms.

## Unit Tests

C++ unit tests use GoogleTest and CTest to validate deterministic engine logic without requiring a graphical environment.

Tests run on Windows, Linux, and FreeBSD and are integrated into CI.

See the [Unit Testing Guide](unit_testing.md) for details.

## Application Smoke Tests

Application smoke tests build and launch selected ContentExamples and TestApplications to verify that they start and remain running without immediate failures.

Supported platforms: Linux and FreeBSD.

A graphical environment is required.

```bash
python3 RenderingEngine/Tests/application_smoke_tests.py
```

To use an already built engine:

```bash
python3 RenderingEngine/Tests/application_smoke_tests.py --skip-engine-build
```

These tests do not verify rendered output.

## Project Workflow Tests

Project workflow tests validate project creation, registration, building, and execution in Dev Mode and SDK-User Mode.

The tests cover:
- Internal project creation
- External project creation
- SDK project creation
- SDK registration

Supported platforms: Linux and FreeBSD.

A graphical environment and a built SDK archive are required for the full suite.

```bash
./build_engine.sh --build-sdk
python3 RenderingEngine/Tests/project_workflow_tests.py
```

Both Python test scripts support `--release` to select Release builds.

## Future Test Coverage

Additional automated tests may be introduced for compiler outputs, generated assets, and other SDK components as the Rendering Engine evolves.