# Getting Started

#### Rendering Engine is a modular, cross-platform graphics engine designed for creating applications such as video games, HMI (Human–Machine Interface) applications, simulators, visualisation and rendering tools.

The engine is provided as a library and a set of supporting tools, enabling application creation, configuration, packaging, and engine-level extension.

## Supported Platforms

Development platforms:
- Windows
- FreeBSD
- Linux

Target platforms:
- Windows
- FreeBSD
- Linux

Note:
Cross-compilation is not yet supported.

## Typical Workflow

The typical workflow when working with the Rendering Engine consists of the following steps:

### 1. Project Creation
Create a new project using the provided project creation tools and templates.  
See: [Project Creation Guide](project_creation_guide.md)

### 2. Application Configuration
Configure your application settings, such as screen size and other runtime parameters.  
See: [Application Config Guide](app_config_guide.md)

### 3. Application Development

Implement your application logic by:
- Creating ```Actor``` and ```Actor2D``` based classes
- Attaching drawable subobjects (such as ```StaticMesh```, ```Rectangle2D```, ```TextBlock2D```)
- Defining application flow and behaviour by deriving from ```Scene```

See: [Application Development Guide](application_development_guide.md)

See also: **Classes** for full API documentation.

### 4. Project Packaging
Package your application for distribution, for example:

uploading to platforms like Steam
deploying to embedded or HMI environments
distributing standalone builds
See: [Project Packaging Guide](project_packaging_guide.md)

## Working Modes

The workflow described above is supported in two modes:

#### SDK User Mode

Use this mode if you want to:
- quickly create applications using a precompiled SDK
- focus on application development rather than engine internals

See: [Downloads](downloads.md)

#### Developer Mode

Use this mode if you:
- need deeper control over engine features
- plan to modify or extend the engine itself
- contribute to engine development

In this mode, you:

- work directly from the engine source tree
- build the engine and tools locally
- develop applications and engine features side-by-side

See: [Developer guide](developer_guide.md)

## Further Reading

- Full API reference is available under Classes
- Detailed setup instructions are available in:

- Prepare Environment
- Project Creation Guide
- Project Packaging Guide