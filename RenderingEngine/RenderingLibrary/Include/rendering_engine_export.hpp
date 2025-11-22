#pragma once

#if defined(_WIN32)
    #if defined(RENDERING_ENGINE_BUILD)
        #define RE_API __declspec(dllexport)
    #else
        #define RE_API __declspec(dllimport)
    #endif
#else
    #define RE_API
#endif