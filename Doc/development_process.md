# Rendering Engine Development Process and Versioning Rules

## Purpose

This document defines the standardized workflow for Git, GitHub Issues, versioning, CI/CD validation, and release publication across Rendering Engine development activities.

The process ensures:

- consistent versioning;
- clear branching rules;
- traceable development work;
- reproducible cross-platform SDK builds;
- predictable release publication.

## Branching Model

### `main`

Stable production branch.

All public releases and version tags must be created from commits integrated into `main`.

### `devel`

Integration branch used to combine completed features and fixes before promotion to `main`.

For small and isolated changes, `devel` may be omitted when appropriate.

### Development branches

Development branches should normally be created from `devel`.

Recommended naming:

```text
<ISSUE_NUMBER>-<description>
```

Examples:
```
79-generate-and-deploy-documentation-site
102-multithreaded-resource-processing
```

Optional categorized naming may also be used:
```
feature/<ISSUE_NUMBER>-<description>
bugfix/<ISSUE_NUMBER>-<description>
```

## Development Workflow

1. Create a development branch from devel.
2. Implement the change.
3. Update documentation when required.
4. Perform relevant local and platform-specific testing.
5. Commit and push the development branch.
6. Merge the completed branch into devel.
7. Perform integration verification.
8. Merge devel into main after completing the intended milestone, sprint, or release scope.

The SDK validation workflow may be run manually for any pushed branch when full cross-platform verification is required.

## Versioning

The Rendering Engine follows Semantic Versioning:

```
MAJOR.MINOR.PATCH
```

Example:

```
0.2.9
```

The version is defined in the root ```CMakeLists.txt```:

```cmake
set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")
set(VERSION_PATCH "9")
```

These values are the single source of truth for the engine version and are propagated through generated version headers, SDK packaging, archive names, and release metadata.

The version is normally incremented once per development branch or release scope, not once per commit.

### Increment Rules

<B>MAJOR</B>
Increment for breaking public API changes or major architectural changes.
Incrementing MAJOR resets MINOR and PATCH to zero.

Example:
```
1.4.3 → 2.0.0
```

<B>PATCH</B>

Increment for bug fixes, optimizations, documentation improvements, or small non-breaking changes.

Example:
```
0.2.8 → 0.2.9
```

### Version Update Rules
- Update the version manually in the root CMakeLists.txt.
- Perform the version update at the beginning of the branch representing the new version.
- Incrementing MINOR resets PATCH to zero.
- Incrementing MAJOR resets MINOR and PATCH to zero.
- Ensure that generated SDK archive names match the intended version.

## CI/CD Workflows

The project uses GitHub Actions workflows for validation, release publication, and documentation deployment.

### Validate SDK Build

Workflow:
```
.github/workflows/build.yml
```

This workflow is started manually and validates SDK builds for:

- Linux;
- Windows;
- FreeBSD.

Windows and FreeBSD self-hosted runners must be online before starting the workflow.

### Publish SDK Release
Workflow:
```
.github/workflows/release-sdk.yml
```

This manually triggered workflow:

1. Reads the engine version from CMakeLists.txt;
2. Builds the Linux, Windows, and FreeBSD SDKs;
3. Validates the versioned archive names;
4. Collects the platform packages;
5. Creates a Git tag;
6. Creates a GitHub Release;
7. Uploads the SDK archives as release assets.

A release should normally be created as a draft first.

### Generate and Deploy Documentation

Workflow:
```
.github/workflows/docs.yml
```

This workflow generates Doxygen documentation and deploys it to GitHub Pages.

It runs automatically when changes are pushed to main and may also be started manually.

### Release Checkpoints

Before publishing a release:

- Confirm that the intended changes have been merged into main.
- Confirm that the version in CMakeLists.txt is correct.
- Confirm that relevant tests and demonstrations have passed.
- Confirm that documentation reflects the current engine state.
- Start the Windows self-hosted runner.
- Start the FreeBSD self-hosted runner.
- Run Publish SDK Release from the main branch.
- Create the release as a draft.
- Verify the generated Git tag.
- Verify the release title and notes.
- Verify that all three SDK archives are attached:

```
RenderingEngine-v<X.Y.Z>-SDK-Linux.tar.gz
RenderingEngine-v<X.Y.Z>-SDK-Windows.tar.gz
RenderingEngine-v<X.Y.Z>-SDK-FreeBSD.tar.gz
```

- Download or inspect the packages when additional verification is required.
- Publish the GitHub Release.
- Confirm that the release is visible on the repository page.
- Confirm that Doc/downloads.md links to the published release assets.
- Confirm that the documentation site was successfully deployed.
- Confirm that landing-page download and documentation links point to the correct public locations.

### Post-Release Verification

After publication:

- Verify the public GitHub Release page;
- Verify all SDK download links;
- Verify the documentation site;
- Verify build and release badges;
- Verify the landing page;
- Close the corresponding GitHub Issue or release-tracking task.