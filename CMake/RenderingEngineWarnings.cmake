include_guard(GLOBAL)

# Internal development policy: never attach these options to SDK usage requirements.
function(re_enable_project_warnings target_name)
    if(NOT TARGET "${target_name}")
        message(FATAL_ERROR "Warning policy requires an existing target: ${target_name}")
    endif()
    get_target_property(is_imported "${target_name}" IMPORTED)
    get_target_property(target_type "${target_name}" TYPE)
    if(is_imported OR target_type STREQUAL "INTERFACE_LIBRARY")
        message(FATAL_ERROR "Warning policy requires a project-owned compiled target: ${target_name}")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # C5038 requires VS 2017 15.3 or newer. The SDK CI uses VS 2019.
        if(MSVC_VERSION LESS 1911)
            message(FATAL_ERROR "Rendering Engine warning policy requires MSVC 19.11 or newer")
        endif()
        set(warning_options
            /W4
            /w44242 /w44254 /w44365 # Narrowing, bit-field and signedness conversions.
            /w44263 /w44264 /w34265 # Hidden virtual functions and non-virtual destruction.
            /w44296 /w14555         # Constant comparisons and no-effect expressions.
            /w44668 /w15038         # Undefined macros and initialization order.
            /wd4251                 # Exported engine classes intentionally contain STL implementation types.
            /wd4324                 # codec_error_mgr intentionally requires alignment padding for jmp_buf.
            /wd4611                 # libjpeg uses setjmp/longjmp for error recovery; no local C++ RAII objects cross the jump boundary.
            /utf-8
        )
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_WARNINGS
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
           (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
            CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU" AND
            NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
        set(warning_options
            -Wall -Wextra -Wpedantic
            -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Wundef
            -Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference -Wcast-qual
            -finput-charset=UTF-8 -fexec-charset=UTF-8
        )
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            list(APPEND warning_options
                -Wimplicit-fallthrough=3 -Wcast-align=strict
                -Wduplicated-cond -Wduplicated-branches -Wlogical-op
            )
        else()
            list(APPEND warning_options -Wimplicit-fallthrough -Wcast-align)
        endif()

        # Do not silently weaken the baseline on older/unsupported toolchains.
        include(CheckCXXCompilerFlag)
        foreach(option IN LISTS warning_options)
            string(MAKE_C_IDENTIFIER "RE_${CMAKE_CXX_COMPILER_ID}_${option}" flag_check)
            check_cxx_compiler_flag("${option}" "${flag_check}")
            if(NOT ${flag_check})
                message(FATAL_ERROR
                    "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} does not support required warning-policy option ${option}")
            endif()
        endforeach()
    else()
        message(FATAL_ERROR
            "Unvalidated Rendering Engine warning-policy compiler/front-end: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} / ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}. Add an explicit policy before using this toolchain.")
    endif()

    foreach(option IN LISTS warning_options)
        target_compile_options("${target_name}" PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${option}>")
    endforeach()
    message(STATUS "${target_name} C++ warning policy: ${warning_options}")
endfunction()
