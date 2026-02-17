# Debugging Guide

## Logging During Debugging

Engine logging level is controlled via the application configuration file:
```json
"logLevel": "Debug"
```

Available levels:

- Debug
- Info
- Warning
- Error


During debugging, "Debug" level is recommended to expose
timing, memory, and asset-loading details.

For performance testing without excessive console output,
consider using "Info".

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
            "target": "${workspaceFolder}/Build/UserApplications/HelloUnixApp/Release/HelloUnixApp",
            "cwd": "${workspaceFolder}/Build/UserApplications/HelloUnixApp/Release",
            "valuesFormatting": "parseText"
        }
    ]
}
```
2. Important Note: target and cwd

The engine resolves shader and asset paths relative to the executable’s working directory.
Therefore, cwd must point to the folder where the executable normally runs, e.g.:
```bash
Build/UserApplications/<ProjectName>/Release
```

<- [Back to Developer Guide Page](developer_guide.md)