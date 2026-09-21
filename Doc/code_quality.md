
# Code Quality Policy

Rendering Engine maintains a compiler-warning baseline for project-owned C++
code across its supported development platforms.

The purpose of the policy is not simply to produce a warning-free build.
Compiler diagnostics are treated as code-review signals that can expose
incorrect type assumptions, unsafe conversions, initialization problems,
dead code, portability issues, and incomplete or stale implementation logic.

## Supported Compiler Baseline

The warning policy is validated in CI with the toolchains used by the
supported SDK builds:

- MSVC on Windows
- GCC on Linux
- Clang on FreeBSD

The warning configuration is defined in
`CMake/RenderingEngineWarnings.cmake`.

## Warning Policy

Project-owned compiled targets use a strict, curated warning set.

On MSVC, the baseline is based on `/W4` with additional selected diagnostics.

On GCC and Clang, the baseline is based on:

```text
-Wall -Wextra -Wpedantic
```

with additional diagnostics covering areas such as:

- implicit and signed/unsigned conversions
- variable shadowing
- format-string correctness
- undefined preprocessor macros
- virtual-function and destructor correctness
- null dereferences
- cast correctness
- implicit fallthrough
- alignment

Compiler-specific diagnostics are enabled where they provide useful additional
coverage.

## Handling Warnings

Warnings in Rendering Engine code should be investigated before deciding how
to resolve them.

Preferred resolutions are:

1. Correct the underlying implementation when the warning exposes a defect.
2. Improve the type model when the warning exposes an unsafe or inconsistent
conversion.
3. Remove obsolete or ineffective code when the warning exposes dead logic.
4. Explicitly document intentional behavior when the diagnostic cannot
reasonably be eliminated.

Warning suppression is reserved for cases where a diagnostic represents
intentional and understood behavior. Such suppressions should be narrow and
documented with their rationale.

Warnings should not be silenced merely through casts, dummy expressions,
pragmas, or global suppression without understanding their cause.

## Third-Party Code

The Rendering Engine warning baseline applies to project-owned code.

Warnings originating from third-party libraries, platform SDKs, compiler
headers, or external build tools are kept separate from the engine's own
warning policy. Third-party source code is not modified solely to make the
overall build log warning-free.

## SDK Consumers

Rendering Engine warning options are target-local and private.

Applications linking against the installed SDK do not inherit the engine's
internal compiler-warning configuration. Consumer projects remain responsible
for defining their own warning and code-quality policies.

## Continuous Validation

The SDK CI builds validate the engine across Windows, Linux, and FreeBSD.

Changes that introduce new project-owned compiler warnings should normally be
resolved before integration. A new suppression should be introduced only when
the diagnostic has been investigated and the reason for accepting it is
documented.