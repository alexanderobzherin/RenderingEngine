# Unit Testing Guide

Rendering Engine maintains an automated unit-test suite for deterministic,
engine-owned logic that can be validated without a physical display or GPU.

The test infrastructure uses:

- **GoogleTest** for writing and executing C++ test cases.
- **CTest** for test discovery and build-system-level test execution.
- **GitHub Actions** for cross-platform CI validation.

Unit tests are located under:

```text
RenderingEngine/Tests
```

## Testing Strategy

Unit tests should primarily cover CPU-side Rendering Engine behavior that:

- has deterministic inputs and outputs;
- contains non-trivial engine-owned logic;
- can regress without producing compiler errors;
- can execute without creating a native window;
- does not require a physical GPU or display;
- can run reliably across supported CI platforms.

The objective is not to create a test for every class or to target an arbitrary
coverage percentage. Tests should provide meaningful regression protection for
behavior that forms part of the engine's expected contract.

GPU submission, swapchain behavior, actual rendering output, native window
creation, physical input devices, and other environment-dependent behavior
should normally be validated through integration, platform, smoke, or future
visual-regression testing instead.

## Test Structure

The unit tests are built into the executable:

```text
RenderingEngine_gtests
```

GoogleTest provides the individual test cases and assertions.

Tests are registered with CTest using GoogleTest discovery, allowing individual
GoogleTest cases to be executed and reported through CTest.

The test target links against the Rendering Engine library rather than compiling
RenderingLibrary implementation files directly into the test executable.

Two build contexts are supported:

- **In-tree build** — tests link against the existing `RenderingEngine` CMake
  target.
- **Standalone build** — tests consume the installed Rendering Engine package
  through `RenderingEngine::RenderingEngine`.

This keeps the test suite aligned with the same public library boundary used by
Rendering Engine consumers.

## Running Unit Tests on Windows

### Development Build

Running:

```bat
build_engine.bat
```

generates the normal Visual Studio development solution, including the
`RenderingEngine_gtests` project.

The test executable can then be built and run from Visual Studio or invoked
directly.

Individual GoogleTest suites or cases can be selected using standard
GoogleTest filters, for example:

```bat
RenderingEngine_gtests.exe --gtest_filter=SceneComponent.*
```

### Unit-Test Build

For an automated unit-test build, run from the repository root:

```bat
build_engine.bat --build-unit-tests
```

This configures the normal project and builds the `RenderingEngine_gtests`
target in Debug configuration. `RenderingEngine` is built automatically as a
dependency.

The build command does not execute the tests.

Run the registered tests separately with:

```bat
ctest --test-dir Build -C Debug --output-on-failure
```

`--output-on-failure` prints the output of failing tests while keeping successful
CI and local runs concise.

## Running Unit Tests on Linux and FreeBSD

On Unix-like systems, the standalone unit-test build consumes an installed
Rendering Engine package.

First build and install the engine from the repository root:

```sh
./build_engine.sh --engine-only
```

The installed engine is placed under:

```text
Build/Installed
```

Then build the unit tests:

```sh
cd RenderingEngine/Tests
./build_tests.sh
```

By default, `build_tests.sh` creates a Debug build.

The build configuration can also be selected explicitly:

```sh
./build_tests.sh --debug
```

or:

```sh
./build_tests.sh --release
```

The standalone test build is generated under:

```text
Build/Tests
```

The script cleans and rebuilds only the test build directory. It does not remove
the root `Build` directory because `Build/Installed` contains the Rendering
Engine package required by the standalone test build.

After building, run the tests from the repository root with:

```sh
ctest --test-dir Build/Tests --output-on-failure
```

Alternatively, the generated `RenderingEngine_gtests` executable can be invoked
directly when working with a specific GoogleTest suite or test case.

## CI Unit-Test Build

The unit-test workflow is defined in:

```text
.github/workflows/unit-tests.yml
```

The workflow validates the unit-test suite on the supported CI platforms:

- Windows
- Linux
- FreeBSD

On Windows, CI uses:

```bat
build_engine.bat --build-unit-tests
```

followed by:

```bat
ctest --test-dir Build -C Debug --output-on-failure
```

On Linux and FreeBSD, CI uses:

```sh
./build_engine.sh --build-unit-tests
```

This mode builds and installs the engine and then invokes the standalone unit-test
build.

Tests are subsequently executed with:

```sh
ctest --test-dir Build/Tests --output-on-failure
```

A failing test causes the corresponding CI job to fail and CTest reports the
failing test output.

## Adding Unit Tests

New deterministic RenderingLibrary functionality should include unit tests when
the behavior can reasonably be isolated from rendering hardware, native windows,
or other environment-dependent resources.

New test source files should be added under:

```text
RenderingEngine/Tests
```

and registered in the `SRC_FILES` list in:

```text
RenderingEngine/Tests/CMakeLists.txt
```

Use existing test files as examples for naming and organization.

Test names should describe observable behavior rather than implementation
details. For example:

```cpp
TEST(SceneComponent, DetachRetainsLocalTransform)
```

is preferable to a name tied to a particular internal function or member
variable.

Tests should verify externally meaningful results and invariants. Avoid making
tests dependent on implementation details unless those details intentionally
form part of the engine's contract.

## Dependency Guidelines

Unit tests should consume RenderingLibrary through the Rendering Engine CMake
target.

Do not compile individual RenderingLibrary `.cpp` files directly into the test
executable as a workaround for missing symbols or dependencies.

Do not manually link low-level implementation dependencies into the test target
when the Rendering Engine target is expected to provide the appropriate
dependency boundary.

If a public RenderingLibrary API cannot be consumed through the
`RenderingEngine` or installed `RenderingEngine::RenderingEngine` target,
investigate the library/package interface rather than bypassing it in the tests.

Tests should also avoid introducing direct dependencies on private third-party
implementation details solely to increase test coverage. Such functionality may
be more appropriate for a separate integration test when it cannot be isolated
cleanly.

## Floating-Point Comparisons

Transformation, color-space, and other floating-point calculations should not
normally be compared using exact equality.

The test suite provides helpers in:

```text
RenderingEngine/Tests/glm_test_helpers.hpp
```

for approximate comparison of GLM vectors and matrices.

Choose tolerances appropriate to the operation being tested. Tests should be
strict enough to detect meaningful regressions without depending on
platform-specific floating-point rounding.

## Test Design Guidelines

When adding or modifying tests:

- prefer deterministic inputs with explicit expected results;
- test observable engine behavior rather than reproducing the implementation;
- include boundary and failure cases where they represent meaningful behavior;
- avoid filesystem, timing, GPU, window-system, or hardware dependencies unless
  they are specifically part of the test being designed;
- avoid assumptions about unspecified ordering or implementation details;
- use explicit update flows when that is part of the engine's established
  runtime contract;
- do not change production behavior merely to make a test convenient.

When existing implementation behavior, documentation, and application usage
appear inconsistent, determine the intended public contract before encoding the
ambiguous behavior in a unit test.

## Local Validation Before Commit

Before committing changes to the unit-test suite, build and run the affected
tests locally where practical.

A complete CTest run should finish with no failed tests:

```text
100% tests passed, 0 tests failed
```

Cross-platform behavior is then validated by the Unit Tests GitHub Actions
workflow.

The CI workflow should be treated as validation of the same test contract used
during local development, rather than as a separate test implementation.