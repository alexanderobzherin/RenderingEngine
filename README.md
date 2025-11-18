<p align="center">
    <img src="./Doc/Images/RE_Banner.png" alt="Rendering Engine">
</p>

# Rendering Engine

A cross-platform rendering engine designed to be portable and backend-agnostic.  
Currently supports Windows and Unix-like systems (Linux, FreeBSD).
Rendering engine - is a library and set of tools to create graphics applications like video games, HMI (Human Machine Interface) applications, simulators, visualisation etc.

### Features (Work in Progress)

- Support for 3D model loading and rendering (e.g., OBJ, FBX)
- Texture loading and binding (PNG, JPEG support)
- Customizable material and shader system (each model can use a unique shader)
- Sprite rendering support
- Text rendering (under development)
- Unit tests and demo application included

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

---

## Developer Guide

[Developer guide](Doc/developer_guide.md) describes workflow in SDK mode, how to build rendering engine from source code, how to create test/demo aplications, CI/CD notes etc.

## Portability notes
This engine builds and runs successfully on:
- Windows 10
- Ubuntu 24.04
- FreeBSD 14.3

Note on FreeBSD:
Vulkan validation layers are currently disabled on FreeBSD due to the inability to install them via pkg or build them from ports. Until this is resolved upstream (in the FreeBSD Ports collection), validation layers will remain disabled in the FreeBSD build of the rendering engine.

## License

This rendering engine is licensed under the [Zlib License](LICENSE.md).

It is free to use, modify, and include in commercial or non-commercial projects, provided that the license terms are followed. You are welcome to build your own applications or systems on top of it.

Please note: Platform-specific integrations (e.g., for consoles) may require additional SDKs or agreements which are not part of this repository.