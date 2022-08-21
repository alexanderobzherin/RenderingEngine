#include "text_renderer.hpp"
#include "texture_atlas_maker.hpp"

namespace rendering_engine
{

TextRenderer::TextRenderer( std::string const pathToFont, unsigned int const size )
    :
    mErrorResult(FT_Err_Ok),
    mLibrary(0),
    mFace(0)
{
    boost::filesystem::path path{ pathToFont };
    if( boost::filesystem::is_regular_file(pathToFont) )
    {
        std::cout << "Exo-Medium.otf" << std::endl;
    }
    else
    {
        std::cout << "Font file not found." << std::endl;
    }

    mErrorResult = FT_Init_FreeType(&mLibrary);
    if( mErrorResult )
    {
        throw std::runtime_error{ "Failed to initialize FreeType library!" };
    }

    mErrorResult = FT_New_Face(mLibrary, pathToFont.c_str(), 0, &mFace);
    if( mErrorResult )
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    mErrorResult = FT_Set_Char_Size(mFace, size << 6, size << 6, 90, 90);
    if( mErrorResult )
    {
        throw std::runtime_error{ "Failed to set char size!" };
    }


    for( int i = 32; i < 128; i++ )
    {
        auto glyphBitmap = CreateGlyphBitmap((char)(i));
        mGlyphBitmaps.emplace((char)(i), glyphBitmap);
    }

    TextureAtlasMaker texAtlasMaker(mGlyphBitmaps);
    texAtlasMaker.CreateTextureAtlas(mTexAtlasData, mGlyphTextureAtlas);

    mGlyphTextureAtlas.WritePngFile("GlyphTextureAtlas.png");
}

TextRenderer::~TextRenderer()
{
    FT_Done_Face(mFace);
    FT_Done_FreeType(mLibrary);
}

ImageData TextRenderer::CreateGlyphBitmap( char const character )
{
    FT_UInt glyph_index = 0;
    glyph_index = FT_Get_Char_Index(mFace, (std::uint16_t)(character));
    mErrorResult = FT_Load_Glyph(mFace, glyph_index, FT_LOAD_DEFAULT);
    if( mErrorResult )
    {
        throw std::runtime_error{ "Failed to load glyph!" };
    }

    mErrorResult = FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_NORMAL);
    if( mErrorResult )
    {
        throw std::runtime_error{ "Failed to render glyph!" };
    }

    std::vector<unsigned int> buffer;
    for( int i = 0; i < (mFace->glyph->bitmap.width * mFace->glyph->bitmap.rows); ++i )
    {
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(mFace->glyph->bitmap.buffer[i]));
    }

    ImageData imageData(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows, buffer);

    return imageData;
}

ImageData TextRenderer::CreateStringBitmap(std::string text)
{
    //Store image data and x offset for each letter in the string.
    std::vector<std::pair<unsigned int, ImageData>> glyphBitmaps;
    unsigned int stringWidthPx = 0U;
    unsigned int stringHeighPx = 0U;
    for( auto glypth : text )
    {
        auto glBitmap = CreateGlyphBitmap(glypth);
        auto const offsetX = stringWidthPx;
        stringWidthPx += glBitmap.GetWidth();
        if( glBitmap.GetHeight() > stringHeighPx )
        {
            stringHeighPx = glBitmap.GetHeight();
        }
        glyphBitmaps.push_back(std::pair<unsigned int, ImageData>(offsetX, glBitmap));
    }

    ImageData stringBitmap(stringWidthPx, stringHeighPx);
    for( auto it : glyphBitmaps )
    {
        unsigned int const xPos = it.first;
        unsigned int const yPos = stringHeighPx - it.second.GetHeight();
        ImageData::DrawImageOnImageAtPos( xPos, yPos, stringBitmap, it.second);
    }
    
    return stringBitmap;
}

ImageData TextRenderer::CreateGlyphBitmap( std::string const pathToFont, char const character )
{
    boost::filesystem::path path{ pathToFont };
    if( boost::filesystem::is_regular_file(pathToFont) )
    {
        std::cout << "Exo-Medium.otf" << std::endl;
    }
    else
    {
        std::cout << "Font file not found." << std::endl;
    }
    
    FT_Error error = FT_Err_Ok;
    FT_Library library = 0;
    FT_Face face = 0;

    error = FT_Init_FreeType( &library );
    if( error )
    {
        throw std::runtime_error{ "Failed to initialize FreeType library!" };
    }

    error = FT_New_Face( library, pathToFont.c_str(), 0, &face);
    if( error )
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    error = FT_Set_Char_Size(face, 10 << 6, 10 << 6, 90, 90);
    if( error )
    {
        throw std::runtime_error{ "Failed to set char size!" };
    }

    FT_UInt glyph_index = 0;
    glyph_index = FT_Get_Char_Index(face, (std::uint16_t)(character));
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if( error )
    {
        throw std::runtime_error{"Failed to load glyph!"};
    }

    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if( error )
    {
        throw std::runtime_error{ "Failed to render glyph!" };
    }

    std::vector<unsigned int> buffer;
    for( int i = 0; i < (face->glyph->bitmap.width * face->glyph->bitmap.rows); ++i )
    {
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(255));
        buffer.push_back((unsigned int)(face->glyph->bitmap.buffer[i]));
    }

    ImageData imageData(face->glyph->bitmap.width, face->glyph->bitmap.rows, buffer);         

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return imageData;
}

} //namespace rendering_engine