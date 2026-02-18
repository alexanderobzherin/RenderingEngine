# Performance Profiling and Analysis Guide

This document describes how to configure and use performance profiling and analysis tools when working with the Rendering Engine.

It explains:
- Runtime configuration parameters
- FPS and frame pacing controls
- Logging levels
- Performance measurement workflow
- Asset loading analysis
- Best practices for production builds

## 1. Runtime Configuration

Performance-related settings are configured via the application configuration file:

```json
{
  "appName": "ActorDrawableAttach",
  "isFullScreen": false,
  "screenWidth": 1920,
  "screenHeight": 1024,

  "text": {
    "scripts": ["Latin"],
    "fontSizePreload": [10]
  },

  "logLevel": "Debug",

  "useSmoothedFPS": false,
  "targetFPS": 60,
  "showStatsOverlay": true
}
```

### 1.1 logLevel

Controls verbosity of engine logging.

Available values:
```
"Error"
"Warning"
"Info"
"Debug"
```

#### Recommended usage

|Build Type	| logLevel |
|:----:|:----:|
|Development| Debug |
| Testing | Info |
| Production | Warning or Error |

Debug level provides detailed timing and memory metrics.

### 1.2 useSmoothedFPS

Controls how FPS values are reported by the engine.

When enabled:

- FPS values are exponentially smoothed.
- Displayed statistics appear more stable.
- Useful for user-facing builds or demonstrations.

When disabled:

- Raw, unsmoothed FPS values are used.
- Preferred for precise performance profiling and regression tracking.

Note:
This option affects only statistical reporting.
It does not change frame pacing or frame limiting behavior.

#### Recommended usage

- Disable (false) when profiling performance bottlenecks
- Enable (true) when testing user experience stability

### 1.3 targetFPS

Specifies the desired frame rate cap.

Examples:

| targetFPS | Behavior |
|:----:|:----:|
| 0 | Uncapped |
|60 | Cap at 60 FPS |
| 120 | Cap at 120 FPS |

### 1.4 Text Preloading Configuration
```json
"text": {
  "scripts": ["Latin"],
  "fontSizePreload": [10],
  "showStatsOverlay": true
}
```

Controls font atlas preloading at startup.

Limiting scripts and sizes:
- Reduces startup time
- Reduces GPU memory usage
- Reduces RAM usage

For production builds, preload only required scripts and sizes.

## 2. Using the Stats Overlay

The engine supports a runtime statistics overlay.
Enable it via:
```json
"showStatsOverlay": true
```
When enabled, it displays:
- FPS
- Frame time (ms)
- Update time
- Render time
- RAM and GPU memory usage (if available)

## 3. Analyzing Logs

Engine logs provide detailed performance data.

Example:
```
Texture uploaded to GPU: test_cube_color (6422528 bytes, 16.603701 ms)
Loaded 7 textures from folder in 336.087799 ms.
Scene 'StartLevel' loaded in 10.191300 ms
CoreApplication initialized in 985 ms.
```

Log entries provide timing and memory information for:
- Application startup
- Scene loading
- Texture uploads
- RAM usage
- GPU usage
- Font atlas generation

## 4. Asset Loading Performance

All assets inside the Content folder are processed during application initialization.

This includes:
- Textures
- Models
- Fonts
- Shaders

#### Important

In production builds, it is recommended to include only assets that are required by the application.
Unused assets:
- Increase startup time
- Increase RAM usage
- Increase GPU memory usage
- Slow down iteration

## 5. Measuring Startup Cost

Look for these entries:
```
CoreApplication initialized in XXX ms
SceneManager initialized in XXX ms
Loaded N textures in XXX ms
Font atlas preloading completed in XXX ms
```

These metrics allow you to:

- Track regression between commits
- Compare builds
- Measure impact of new systems

## 6. Production Readiness Checklist

Before shipping:
- logLevel set to Warning or Error
- useSmoothedFPS enabled
- targetFPS set appropriately (60 recommended)
- Unused assets removed
- Startup time measured
- Scene load time measured
- GPU memory usage verified

## 7. GPU Profiling with RenderDoc

For deep GPU-level inspection, the Rendering Engine can be analyzed using RenderDoc, an open-source graphics debugger:

[RenderDoc](https://renderdoc.org/)

RenderDoc can be used to:
- Capture a single rendered frame
- Inspect draw calls
- Analyze bound pipelines and descriptor sets
- View vertex buffers and index buffers
- Inspect textures and framebuffer attachments
- Step through the graphics pipeline stages
- Examine shader inputs/outputs
- Detect overdraw and redundant state changes