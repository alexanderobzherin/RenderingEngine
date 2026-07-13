# Continuous Integration & Continuous Deployment Guide

## Overview

Continuous Integration (CI) and Continuous Deployment (CD) in this project define a structured workflow for building the Rendering Engine SDK for all supported platforms, generating project documentation, and publishing the resulting artifacts for download and online availability.

The CI/CD system ensures that:
SDKs are built in a reproducible way for all supported platforms
Documentation is generated from the current codebase
Resulting artifacts are automatically uploaded to public endpoints

## CI/CD Infrastructure
CI/CD for this project is handled through GitHub Actions.

The workflows are defined in:

- `.github/workflows/build.yml`
- `.github/workflows/release-sdk.yml`
- `.github/workflows/docs.yml`

### Validate SDK Build

`build.yml` performs cross-platform SDK validation builds for:

- Linux using a GitHub-hosted Ubuntu runner
- Windows using a self-hosted Windows runner
- FreeBSD using a self-hosted FreeBSD runner

The workflow is started manually using `workflow_dispatch`, allowing any selected branch to be validated without publishing a release.

Build results are stored as temporary GitHub Actions artifacts.

### Publish SDK Release

`release-sdk.yml` builds, validates, and publishes versioned SDK packages for Linux, Windows, and FreeBSD.

The engine version is read from the root `CMakeLists.txt`. The workflow creates a Git tag and a GitHub Release containing the platform-specific SDK archives.

Release publication is initiated manually through:

`Repository → Actions → Publish SDK Release → Run workflow`

A release should normally be created first as a draft, reviewed, and then published.

### Generate and Deploy Documentation

`docs.yml` generates the Doxygen documentation and deploys the resulting HTML site to GitHub Pages.

The workflow runs automatically when changes are pushed to `main` and can also be started manually using `workflow_dispatch`.

The resulting artifacts are published to:
Documentation: https://docs.rendering-engine.alexander-obzherin.info
SDK downloads: https://downloads.rendering-engine.alexander-obzherin.info

### Manual Build

The workflow can also be started manually using ```workflow_dispatch```.

To run the workflow manually:

1. Open the GitHub repository.
2. Select Actions.
3. Select Build Rendering Engine from the workflow list.
4. Click Run workflow.
5. Select the branch to build.
6. Click Run workflow to start the build.

This allows feature branches to be tested without automatically triggering CI on every feature-branch push.

## Testing Feature Branches with CI

Feature branches do not automatically trigger the SDK build workflow.

To test a feature branch:

1. Push the feature branch to GitHub.
2. Open <B>Actions</B> in the GitHub repository.
3. Select <B>Build Rendering Engine</B>.
4. Click <B>Run workflow</B>.
5. Select the required feature branch.
6. Start the workflow.

## Build Artifacts

Each successful platform build produces a versioned SDK archive:

```
Build/Packages/RenderingEngine-v<X.Y.Z>-SDK-Linux.tar.gz
Build/Packages/RenderingEngine-v<X.Y.Z>-SDK-Windows.tar.gz
Build/Packages/RenderingEngine-v<X.Y.Z>-SDK-FreeBSD.tar.gz
```

## Setting Up a Self-Hosted Runner

Self-hosted runners are configured in the GitHub repository:
```
Repository → Settings → Actions → Runners
```
Select:
```
New self-hosted runner
```
GitHub then provides platform-specific commands for:

1. Downloading the runner package.
2. Verifying its checksum.
3. Extracting the package.
4. Registering the runner with the repository.
5. Starting the runner.

The registration token shown by GitHub is temporary. Use the commands provided by GitHub when registering a new runner.

### Setting Up the Windows Self-Hosted Runner

#### Create the Runner
Open:
```
Repository → Settings → Actions → Runners → New self-hosted runner
```
Select:
```
Runner image: Windows
Architecture: x64
```
Create a directory for the runner, for example:
```
E:\Development\RenderingEngine\WindowsCIBuild\GitHubRunner\actions-runner
```
Run the download, checksum verification, extraction, and configuration commands provided by GitHub.

During configuration, use the additional label:
```
sdk-windows
```
The resulting runner labels are:
```
self-hosted
Windows
X64
sdk-windows
```
The workflow selects the runner using:
```
runs-on: [self-hosted, Windows, X64, sdk-windows]
```
#### Prepare the Windows Environment

Ensure that the Windows machine has all required dependencies installed.
See [Prepare Environment](prepare_environment.md)

The following tools must be available in PATH:
```bash
cl (MSVC)
git
cmake
```

#### Start the Windows Runner

Open PowerShell and navigate to the runner directory:

```bash
cd E:\Development\RenderingEngine\WindowsCIBuild\GitHubRunner\actions-runner
```

Start the runner:
```bash
.\run.cmd
```

A successfully started runner displays:
```
Connected to GitHub
Listening for Jobs
```
Keep the terminal open while the runner is required.

### Setting Up the FreeBSD Self-Hosted Runner

GitHub does not provide a native FreeBSD runner option. The GitHub Actions Linux x64 self-hosted runner package is used on the FreeBSD build machine.

#### Create the Runner

Open:

```
Repository → Settings → Actions → Runners → New self-hosted runner
```

Select:

```
Runner image: Linux
Architecture: x64
```

Create a directory for the runner, for example:

```
~/Development/FreeBSDCIBuild/GitHubRunner/actions-runner
```

Run the download, checksum verification, extraction, and configuration commands provided by GitHub.

During configuration, use the additional label:

```
sdk-freebsd
```

The resulting runner labels are:

```
self-hosted
Linux
X64
sdk-freebsd
```

The workflow selects the runner using:

```
runs-on: [self-hosted, Linux, X64, sdk-freebsd]
```

#### Prepare the FreeBSD Environment

Ensure that all Rendering Engine build dependencies are installed.

See [Prepare Environment](prepare_environment.md)

The following tools must be available:

```
git
cmake
curl
tar
```

#### FreeBSD tar Compatibility

The GitHub Actions Linux runner may fail during action preparation on FreeBSD with:

```
Error: tar: command not found
```

This can occur even when FreeBSD's native tar is available:

```
command -v tar
```

Expected output:

```
/usr/bin/tar
```

To provide a tar command in a runner-specific location, create a compatibility directory:

```
mkdir -p ~/Development/FreeBSDCIBuild/GitHubRunner/bin
ln -sf /usr/bin/tar ~/Development/FreeBSDCIBuild/GitHubRunner/bin/tar
```

Create a dedicated runner startup script:

```
cd ~/Development/FreeBSDCIBuild/GitHubRunner/actions-runner
ee run-freebsd-runner.sh
```

Add:

```bash
#!/bin/sh
export PATH="$HOME/Development/FreeBSDCIBuild/GitHubRunner/bin:/usr/local/llvm20/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin:$HOME/bin"

exec ./run.sh
```

Make the script executable:

```bash
chmod +x run-freebsd-runner.sh
```

Start the runner using:

```bash
./run-freebsd-runner.sh
```

A successfully started runner displays:

```bash
Connected to GitHub
Listening for Jobs
```

Keep the terminal open while the runner is required.


# Development Process and Release Tracking

The standardized development workflow, branching model, versioning rules, and release process are documented in:

[Rendering Engine Development Process and Versioning Rules](development_process.md)

<- [Back to Developer Guide Page](developer_guide.md)