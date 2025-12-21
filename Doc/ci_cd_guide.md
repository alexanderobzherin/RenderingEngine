# Continuous Integration & Continuous Deployment Guide

## Overview

Continuous Integration (CI) and Continuous Deployment (CD) in this project define a structured workflow for building the Rendering Engine SDK for all supported platforms, generating project documentation, and publishing the resulting artifacts for download and online availability.

The CI/CD system ensures that:
SDKs are built in a reproducible way for all supported platforms
Documentation is generated from the current codebase
Resulting artifacts are automatically uploaded to public endpoints

## CI/CD Infrastructure

CI/CD for this project is handled via Bitbucket Pipelines, defined in the repository configuration file:
bitbucket-pipelines.yml

The pipelines are dedicated to building the following deliverables:
```
- Documentation
- SDK (Linux)
- SDK (Windows)
- SDK (FreeBSD)
```
The resulting artifacts are published to:
Documentation: https://docs.rendering-engine.alexander-obzherin.info
SDK downloads: https://downloads.rendering-engine.alexander-obzherin.info

Documentation and the Linux SDK are built using Bitbucket’s internal Docker infrastructure. Windows and FreeBSD SDKs are built using self-hosted runners.

## CI Pipelines and Git Branch Mapping

Each CI pipeline is mapped to a dedicated Git branch:
docs - documentation build
sdk-linux - Linux SDK build
sdk-windows - Windows SDK build
sdk-freebsd - FreeBSD SDK build

A CI build is triggered automatically when changes are pushed to the corresponding CI branch.

Important: CI branches are build-only branches and must not be used for direct development or merged back into main or devel.

## Testing Feature Branches with CI

If a feature branch needs to be tested using a CI pipeline, replay the feature commits on top of the appropriate CI branch.

Example (Windows SDK):
```bash
git checkout sdk-windows
git rebase feature/branch-name
git push --force-with-lease
```
Notes:
CI branches are intentionally rebased
--force-with-lease is required and safe in this context
CI branches should be treated as disposable build triggers

## Updating CI Artifacts After Main Branch Changes

After the main branch is updated with recent changes, the CI build branches should also be updated to trigger their build jobs and refresh downloadable artifacts.

This guarantees that published SDKs and documentation always reflect the latest stable state of the project.



## Repository Configuration
### Required Repository Variable

To enable artifact uploads to Google Cloud Storage, a repository-level variable must be configured:
```bash
Repository settings -> Pipelines -> Repository variables -> Add
```
Name: ```GCP_SERVICE_ACCOUNT_JSON_B64```
Value: Base64-encoded contents of gcp-key.json
This variable is used by CI pipelines to authenticate with Google Cloud Storage.


# Setting Up a Self-Hosted Runner (Windows)
1. Create Runner in Bitbucket UI
```bash
Repository settings -> Runners -> Add runner
```
System and architecture: ```Windows (64-bit)```
Runner name: ```windows-msvc-runner```
Runner labels: ```bashself.hosted```, ```windows```

Bitbucket will provide instructions to download and start the runner. Save this information for future runner restarts.

2. Prepare the Windows Environment

Ensure that the Windows machine has all required dependencies installed.
See [Prepare Environment](prepare_environment.md)

3. Required Components for Windows Runner

The following tools must be available in PATH:
```bash
cl (MSVC)
git
cmake
gsutil
Java 11 or newer
```

4. Start the Runner

Unpack and execute the runner provided by Bitbucket (from step 1).


# Setting Up a Self-Hosted Runner (FreeBSD)
1. Create Runner in Bitbucket UI
```bash
Repository settings -> Runners -> Add runner
```
System and architecture: ```Linux Shell```
Runner name: ```freebsd-runner```
Runner labels: ```self.hosted```, ```linux.shell```

Bitbucket will provide instructions to download and start the runner. Save this information for future runner restarts.

2. Prepare the FreeBSD Environment

Ensure that the FreeBSD system has all required build dependencies installed.
See [Prepare Environment](prepare_environment.md)

3. Required Components for FreeBSD Runner
Install required tools:
```bash
git
cmake
curl
java
python3
gcloud
sudo pkg install -y google-cloud-sdk
```
Bash Compatibility
Bitbucket runner expects ```/bin/bash```. On FreeBSD, Bash is typically located elsewhere.
```bash
which bash
sudo ln -s /usr/local/bin/bash /bin/bash
```

setsid Compatibility Shim
Bitbucket runner also expects setsid, which is not available on FreeBSD by default. Create a lightweight compatibility wrapper:
```bash
sudo tee /usr/local/bin/setsid >/dev/null <<'EOF'
#!/bin/sh
exec "$@"
EOF

sudo chmod +x /usr/local/bin/setsid
```
4. Start the Runner
Unpack and execute the runner provided by Bitbucket (from step 1).

# Release Process Tracking
The process of preparing and publishing releases is formalized in Jira.
[RE-45 Rendering Engine Development Process and Versioning Rules](https://alexander-obzherin.atlassian.net/browse/RE-45)

<- [Back to Developer Guide Page](developer_guide.md)