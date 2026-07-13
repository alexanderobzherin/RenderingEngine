[![Validate SDK Build](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/build.yml/badge.svg)](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/build.yml)

[![Documentation](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/docs.yml/badge.svg?branch=main)](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/docs.yml)

[![Publish SDK Release](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/release-sdk.yml/badge.svg)](https://github.com/alexanderobzherin/RenderingEngine/actions/workflows/release-sdk.yml)

[![Latest Release](https://img.shields.io/github/v/release/alexanderobzherin/RenderingEngine?label=Latest%20Release)](https://github.com/alexanderobzherin/RenderingEngine/releases/latest)

<p align="center">
    <img src="./Doc/Images/RE_Banner.png" alt="Rendering Engine">
</p>

# Rendering Engine

**Rendering Engine** is a modular, cross-platform graphics engine designed to be portable and backend-agnostic.

It is provided as a **library and a set of supporting tools** for creating graphics applications such as:
- video games
- HMI (Human–Machine Interface) applications
- simulators
- visualisation and rendering tools

The engine currently supports Windows and Unix-like systems, including Linux and FreeBSD.

---

## Documentation

Comprehensive documentation, including Getting Started guides, SDK downloads, and full API reference, is available here:

[https://docs.rendering-engine.alexander-obzherin.info/](https://docs.rendering-engine.alexander-obzherin.info/)

---

## Supported Platforms

- Windows
- Linux
- FreeBSD

---

## Dependencies

The following libraries are required:

- [Vulkan SDK](https://vulkan.lunarg.com/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [Boost::Filesystem](https://www.boost.org/)
- [GTest](https://github.com/google/googletest)
- [libjpeg-turbo](https://libjpeg-turbo.org/)
- [libpng](http://www.libpng.org/)
- [Assimp](https://github.com/assimp/assimp)
- [FreeType](https://freetype.org/)
- [HarfBuzz](https://harfbuzz.github.io/)
- [nlohmann/json](https://json.nlohmann.me/)

All listed dependencies use permissive or industry-standard licenses suitable for commercial and non-commercial use.

Detailed build and setup instructions are provided in the online documentation.

---

## Portability notes
This engine builds and runs successfully on:
- Windows 10
- Ubuntu 24.04
- FreeBSD 14.3

## License

This rendering engine is licensed under the [Zlib License](LICENSE.md).

It is free to use, modify, and include in commercial or non-commercial projects, provided that the license terms are followed. You are welcome to build your own applications or systems on top of it.

Please note: Platform-specific integrations (e.g., for consoles) may require additional SDKs or agreements which are not part of this repository.