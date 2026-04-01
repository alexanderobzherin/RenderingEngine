// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file image_codec_png.hpp
 * @brief PNG read/write backend using libpng.
 *
 * This module bridges the Rendering Engine's `ImageData` abstraction and the libpng C API.
 * It provides direct read/write routines for 8-bit RGBA images.
 *
 * @details
 * The implementation follows the official libpng examples and reference documentation:
 * - https://www.libpng.org/pub/png/libpng.html
 * - See "Simplified API for Reading and Writing" for modern usage examples.
 *
 * The function sequence mirrors the standard libpng workflow:
 * 1. Create and initialize `png_structp` and `png_infop`.
 * 2. Set up `setjmp` error handling.
 * 3. Configure image metadata with `png_set_IHDR`.
 * 4. Write/read rows with `png_write_row` / `png_image_finish_read`.
 * 5. Clean up with `png_destroy_write_struct`.
 *
 * @note
 * This file is considered an engine backend implementation detail.
 * It is not intended for use outside `ImageData` and `ImageDataGpu`.
 *
 * @see rendering_engine::ImageData
 */
#pragma once

#include <cstring>
#include <vector>
#include <stdlib.h>
#include <png.h>

/**
 * @brief Saves image data to a PNG file.
 *
 * Uses libpng to write RGBA data from ImageData into a PNG file.
 * The function performs full file initialization, metadata setup, and cleanup.
 *
 * @param imageData Source image to save.
 * @param filename Path to the output PNG file.
 */
static void SaveTextureFilePng(rendering_engine::ImageData const& imageData, char const* filename)
{
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_init_io(png_ptr, fp);

    int const bit_depth = 8;
    png_set_IHDR(png_ptr,
        info_ptr,
        imageData.GetWidth(),
        imageData.GetHeight(),
        bit_depth,
        PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);

    png_colorp palette =
        static_cast<png_colorp>(png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color)));
    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

    png_color_8 sig_bit;
    sig_bit.red = 8;
    sig_bit.green = 8;
    sig_bit.blue = 8;
    sig_bit.alpha = 8;
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);

    png_write_info(png_ptr, info_ptr);

    auto const imageDataVector = imageData.GetImageDataRGBA();
    size_t const rowLength = static_cast<size_t>(imageData.GetWidth()) * 4U;

    for (unsigned int row = 0; row < imageData.GetHeight(); ++row)
    {
        png_bytep row_pointer = const_cast<png_bytep>(
            reinterpret_cast<png_const_bytep>(imageDataVector.data() + static_cast<size_t>(row) * rowLength));

        png_write_row(png_ptr, row_pointer);
    }

    png_write_end(png_ptr, info_ptr);

    png_free(png_ptr, palette);
    palette = NULL;

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

/**
 * @brief Reads a PNG file into RGBA image data.
 *
 * Loads an image using libpng's simplified API and converts it to 8-bit RGBA format.
 *
 * @param filename Path to the PNG file.
 * @param width [out] Image width.
 * @param height [out] Image height.
 * @param rgbaImageDataVector [out] Filled with 4-byte-per-pixel RGBA data.
 * @return true if the image was successfully read, false otherwise.
 */
static bool ReadPngFile(char const* filename, unsigned int& width, unsigned int& height, std::vector<std::uint8_t>& rgbaImageDataVector)
{
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image, filename) == 0)
    {
        return false;
    }

    image.format = PNG_FORMAT_RGBA;

    size_t const imageSize = PNG_IMAGE_SIZE(image);
    png_bytep buffer = new png_byte[imageSize];

    if (buffer == NULL)
    {
        png_image_free(&image);
        return false;
    }

    if (png_image_finish_read(&image,
        NULL /* background */,
        buffer,
        0 /* row_stride */,
        NULL /* colormap */) == 0)
    {
        delete[] buffer;
        png_image_free(&image);
        return false;
    }

    width = image.width;
    height = image.height;

    rgbaImageDataVector.resize(imageSize);
    std::memcpy(rgbaImageDataVector.data(), buffer, imageSize);

    delete[] buffer;
    png_image_free(&image);

    return true;
}

/**
 * @brief Reads a PNG image from memory into RGBA image data.
 *
 * Works similarly to ReadPngFile(), but instead of loading from disk it
 * accepts a raw memory buffer containing a complete PNG file. The data
 * is decoded with libpng’s simplified API and converted to 8-bit RGBA format.
 *
 * @param memory Pointer to the PNG file data in memory.
 * @param memorySize Size of the memory buffer in bytes.
 * @param width [out] Image width in pixels.
 * @param height [out] Image height in pixels.
 * @param rgbaImageDataVector [out] Output buffer filled with RGBA pixel data
 *        (4 bytes per pixel, stored in a vector of 32-bit unsigned integers).
 * @return true if decoding succeeded, false otherwise.
 */
static bool ReadPngFromMemory(
    const unsigned char* memory,
    size_t memorySize,
    unsigned int& width,
    unsigned int& height,
    std::vector<std::uint8_t>& rgbaImageDataVector)
{
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, memory, memorySize) == 0)
    {
        return false;
    }

    image.format = PNG_FORMAT_RGBA;

    size_t const imageSize = PNG_IMAGE_SIZE(image);
    png_bytep buffer = new png_byte[imageSize];

    if (buffer == NULL)
    {
        png_image_free(&image);
        return false;
    }

    if (png_image_finish_read(&image, nullptr, buffer, 0, nullptr) == 0)
    {
        delete[] buffer;
        png_image_free(&image);
        return false;
    }

    width = image.width;
    height = image.height;

    rgbaImageDataVector.resize(imageSize);
    std::memcpy(rgbaImageDataVector.data(), buffer, imageSize);

    delete[] buffer;
    png_image_free(&image);

    return true;
}
