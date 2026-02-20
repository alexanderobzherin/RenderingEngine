// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "level_register.hpp"
#include "utility.hpp"
#include "core_application.hpp"


using namespace rendering_engine;

int main(int argc, char *argv[])
{
    Utility::InitializePaths(argc, argv);

    std::unique_ptr<CoreApplication> app = std::make_unique<CoreApplication>();
    try
    {
        app->Initialize();
        app->Run();
    }
    catch( std::exception const& exc )
    {
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}

/*
TextShaping Test Application

This application demonstrates the effect of text shaping on complex scripts.

Two columns are rendered:
- Left: shaping enabled
- Right: shaping disabled

The demo highlights glyph reordering, ligatures, contextual forms,
and conjunct rendering for scripts that require shaping.

Fonts used:
Syriac (Aramaic)
Noto Sans Syriac
https://fonts.google.com/noto/specimen/Noto+Sans+Syriac

Hebrew
Frank Ruhl Libre
https://fonts.google.com/specimen/Frank+Ruhl+Libre

Arabic
Amiri
https://fonts.google.com/specimen/Amiri

Devanagari
NotoSansDevanagari
https://fonts.google.com/noto/specimen/Noto+Sans+Devanagari

Bengali
NotoSansBengali
https://fonts.google.com/noto/specimen/Noto+Sans+Bengali

Gurmukhi
NotoSansGurmukhi
https://fonts.google.com/noto/specimen/Noto+Sans+Gurmukhi

Gujarati
NotoSansGujarati
https://fonts.google.com/noto/specimen/Noto+Sans+Gujarati

Tamil
NotoSansTamil
https://fonts.google.com/noto/specimen/Noto+Sans+Tamil

Telugu
NotoSansTelugu
https://fonts.google.com/noto/specimen/Noto+Sans+Telugu

Kannada
NotoSansKannada
https://fonts.google.com/noto/specimen/Noto+Sans+Kannada

Malayalam
NotoSansMalayalam
https://fonts.google.com/noto/specimen/Noto+Sans+Malayalam

Thai
NotoSansThai
https://fonts.google.com/noto/specimen/Noto+Sans+Thai

Khmer
NotoSansKhmer
https://fonts.google.com/noto/specimen/Noto+Sans+Khmer

Lao
NotoSansLao
https://fonts.google.com/noto/specimen/Noto+Sans+Lao

Myanmar
NotoSansMyanmar
https://fonts.google.com/noto/specimen/Noto+Sans+Myanmar

Sinhala
NotoSansSinhala
https://fonts.google.com/noto/specimen/Noto+Sans+Sinhala

Tibetan
NotoSerifTibetan
https://fonts.google.com/noto/specimen/Noto+Serif+Tibetan
*/