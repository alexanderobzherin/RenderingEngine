# Debugging Guide

## Debugging Applications on Unix (VS Code + GDB)

To debug a Rendering Engine application on Unix-like systems (FreeBSD/Linux) using Visual Studio Code and GDB, it is needed to create a dedicated launch configuration.

1. Create a GDB launch configuration
In VS Code:
```bash
Run → Add Configuration… → C/C++: (gdb) Launch
```
This generates a ```launch.json``` entry.
Replace it with the following minimal working configuration:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "gdb",
            "request": "launch",
            "target": "${workspaceFolder}/Build/TestApplications/HelloUnixApp/Release/HelloUnixApp",
            "cwd": "${workspaceFolder}/Build/TestApplications/HelloUnixApp/Release",
            "valuesFormatting": "parseText"
        }
    ]
}
```
2. Important Note: target and cwd

The engine resolves shader and asset paths relative to the executable’s working directory.
Therefore, cwd must point to the folder where the executable normally runs, e.g.:
```bash
Build/TestApplications/<ProjectName>/Release
```

<- [Back to Developer Guide Page](developer_guide.md)