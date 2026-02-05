#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "text_block_2d.hpp"
#include "i_application.hpp"

using namespace rendering_engine;

namespace application
{

StartLevel::StartLevel(SceneManager& sceneManager)
	:
	Scene(sceneManager)
{}

void StartLevel::Initialize()
{
	Scene::Initialize();

	auto screenSettings = GetSceneManager().GetApplication()->GetScreenSettings();
	
	const float horizDelta = 250.0f;
	const float vertDelta = 50.0f;
	const float column_0 = -350.0f;
	const float column_1 = column_0 + horizDelta;
	const float column_2 = column_1 + horizDelta;

	const float row_0 = -(static_cast<float>(screenSettings.height) / 2.0f) + vertDelta;
	const float row_1 = row_0 + vertDelta;
	const float row_2 = row_1 + vertDelta;
	const float row_3 = row_2 + vertDelta;
	const float row_4 = row_3 + vertDelta;
	const float row_5 = row_4 + vertDelta;
	const float row_6 = row_5 + vertDelta;
	const float row_7 = row_6 + vertDelta;
	const float row_8 = row_7 + vertDelta;
	const float row_9 = row_8 + vertDelta;
	const float row_10 = row_9 + vertDelta;
	const float row_11 = row_10 + vertDelta;
	const float row_12 = row_11 + vertDelta;
	const float row_13 = row_12 + vertDelta;
	const float row_14 = row_13 + vertDelta;
	const float row_15 = row_14 + vertDelta;
	const float row_16 = row_15 + vertDelta;
	const float row_17 = row_16 + vertDelta;

	const std::string fontName = "RobotoMono-Regular";

	AddTextBlock(fontName, "Shaped text:", glm::vec2(column_1, row_0), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	AddTextBlock(fontName, "Not shaped text:", glm::vec2(column_2, row_0), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));

	/// Aramaic
	const std::string aramaicFontName = "NotoSansSyriac-Regular";
	const std::string textRendering_Aramaic = "ܡܚܙܘܝܘܬܐ ܕܟܬܒܐ";
	AddTextBlock(fontName, "Aramaic", glm::vec2(column_0, row_1), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(aramaicFontName, textRendering_Aramaic, glm::vec2(column_1, row_1));
	// not shaped
	AddTextBlock(aramaicFontName, textRendering_Aramaic, glm::vec2(column_2, row_1), false);

	/// Hebrew
	const std::string hebrewFontName = "FrankRuhlLibre-Regular";
	const std::string textRendering_Hebrew = "עיבוד טקסט";
	AddTextBlock(fontName, "Hebrew", glm::vec2(column_0, row_2), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(hebrewFontName, textRendering_Hebrew, glm::vec2(column_1, row_2));
	// not shaped
	AddTextBlock(hebrewFontName, textRendering_Hebrew, glm::vec2(column_2, row_2), false);

	/// Arabic
	const std::string arabicFontName = "Amiri-Regular";
	const std::string textRendering_Arabic = "عرض النص";
	AddTextBlock(fontName, "Arabic", glm::vec2(column_0, row_3), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(arabicFontName, textRendering_Arabic, glm::vec2(column_1, row_3));
	// not shaped
	AddTextBlock(arabicFontName, textRendering_Arabic, glm::vec2(column_2, row_3), false);

	/// Devanagari
	const std::string devanagariFontName = "NotoSansDevanagari-Regular";
	const std::string textRendering_Devanagari = "पाठ प्रतिपादन";
	AddTextBlock(fontName, "Devanagari", glm::vec2(column_0, row_4), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(devanagariFontName, textRendering_Devanagari, glm::vec2(column_1, row_4));
	// not shaped
	AddTextBlock(devanagariFontName, textRendering_Devanagari, glm::vec2(column_2, row_4), false);

	/// Bengali
	const std::string bengaliFontName = "NotoSansBengali-Regular";
	const std::string textRendering_Bengali = "পাঠ রেন্ডারিং";
	AddTextBlock(fontName, "Bengali", glm::vec2(column_0, row_5), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(bengaliFontName, textRendering_Bengali, glm::vec2(column_1, row_5));
	// not shaped
	AddTextBlock(bengaliFontName, textRendering_Bengali, glm::vec2(column_2, row_5), false);

	/// Gurmukhi
	const std::string gurmukhiFontName = "NotoSansGurmukhi-Regular";
	const std::string textRendering_Gurmukhi = "ਪਾਠ ਰੈਂਡਰਿੰਗ";
	AddTextBlock(fontName, "Gurmukhi", glm::vec2(column_0, row_6), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(gurmukhiFontName, textRendering_Gurmukhi, glm::vec2(column_1, row_6));
	// not shaped
	AddTextBlock(gurmukhiFontName, textRendering_Gurmukhi, glm::vec2(column_2, row_6), false);

	/// Gujarati
	const std::string gujaratiFontName = "NotoSansGujarati-Regular";
	const std::string textRendering_Gujarati = "ટેક્સ્ટ રેન્ડરિંગ";
	AddTextBlock(fontName, "Gujarati", glm::vec2(column_0, row_7), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(gujaratiFontName, textRendering_Gujarati, glm::vec2(column_1, row_7));
	// not shaped
	AddTextBlock(gujaratiFontName, textRendering_Gujarati, glm::vec2(column_2, row_7), false);

	/// Tamil
	const std::string tamilFontName = "NotoSansTamil-Regular";
	const std::string textRendering_Tamil = "உரை வரைதல்";
	AddTextBlock(fontName, "Tamil", glm::vec2(column_0, row_8), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(tamilFontName, textRendering_Tamil, glm::vec2(column_1, row_8));
	// not shaped
	AddTextBlock(tamilFontName, textRendering_Tamil, glm::vec2(column_2, row_8), false);

	/// Telugu
	const std::string teluguFontName = "NotoSansTelugu-Regular";
	const std::string textRendering_Telugu = "పాఠ్య చిత్రీకరణ";
	AddTextBlock(fontName, "Telugu", glm::vec2(column_0, row_9), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(teluguFontName, textRendering_Telugu, glm::vec2(column_1, row_9));
	// not shaped
	AddTextBlock(teluguFontName, textRendering_Telugu, glm::vec2(column_2, row_9), false);

	/// Kannada
	const std::string kannadaFontName = "NotoSansKannada-Regular";
	const std::string textRendering_Kannada = "ಪಠ್ಯ ರೆಂಡರಿಂಗ್";
	AddTextBlock(fontName, "Kannada", glm::vec2(column_0, row_10), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(kannadaFontName, textRendering_Kannada, glm::vec2(column_1, row_10));
	// not shaped
	AddTextBlock(kannadaFontName, textRendering_Kannada, glm::vec2(column_2, row_10), false);

	/// Malayalam
	const std::string malayalamFontName = "NotoSansMalayalam-Regular";
	const std::string textRendering_Malayalam = "ടെക്സ്റ്റ് റെൻഡറിംഗ്";
	AddTextBlock(fontName, "Malayalam", glm::vec2(column_0, row_11), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(malayalamFontName, textRendering_Malayalam, glm::vec2(column_1, row_11));
	// not shaped
	AddTextBlock(malayalamFontName, textRendering_Malayalam, glm::vec2(column_2, row_11), false);

	/// Thai
	const std::string thaiFontName = "NotoSansThai-Regular";
	const std::string textRendering_Thai = "การแสดงผลข้อความ";
	AddTextBlock(fontName, "Thai", glm::vec2(column_0, row_12), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(thaiFontName, textRendering_Thai, glm::vec2(column_1, row_12));
	// not shaped
	AddTextBlock(thaiFontName, textRendering_Thai, glm::vec2(column_2, row_12), false);

	/// Khmer
	const std::string khmerFontName = "NotoSansKhmer-Regular";
	const std::string textRendering_Khmer = "ការបង្ហាញអត្ថបទ";
	AddTextBlock(fontName, "Khmer", glm::vec2(column_0, row_13), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(khmerFontName, textRendering_Khmer, glm::vec2(column_1, row_13));
	// not shaped
	AddTextBlock(khmerFontName, textRendering_Khmer, glm::vec2(column_2, row_13), false);

	/// Lao
	const std::string laoFontName = "NotoSansLao-Regular";
	const std::string textRendering_Lao = "ການສະແດງຂໍ້ຄວາມ";
	AddTextBlock(fontName, "Lao", glm::vec2(column_0, row_14), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(laoFontName, textRendering_Lao, glm::vec2(column_1, row_14));
	// not shaped
	AddTextBlock(laoFontName, textRendering_Lao, glm::vec2(column_2, row_14), false);

	/// Myanmar (Burmese)
	const std::string myanmarFontName = "NotoSansMyanmar-Regular";
	const std::string textRendering_Myanmar = "စာသားပြသမှု";
	AddTextBlock(fontName, "Myanmar (Burmese)", glm::vec2(column_0, row_15), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(myanmarFontName, textRendering_Myanmar, glm::vec2(column_1, row_15));
	// not shaped
	AddTextBlock(myanmarFontName, textRendering_Myanmar, glm::vec2(column_2, row_15), false);

	/// Sinhala
	const std::string sinhalaFontName = "NotoSansSinhala-Regular";
	const std::string textRendering_Sinhala = "පෙළ ප්‍රදර්ශනය";
	AddTextBlock(fontName, "Sinhala", glm::vec2(column_0, row_16), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(sinhalaFontName, textRendering_Sinhala, glm::vec2(column_1, row_16));
	// not shaped
	AddTextBlock(sinhalaFontName, textRendering_Sinhala, glm::vec2(column_2, row_16), false);

	/// Tibetan
	const std::string tibetanFontName = "NotoSerifTibetan-Regular";
	const std::string textRendering_Tibetan = "ཡིག་རྟགས་འཆར་སྟོན།";
	AddTextBlock(fontName, "Tibetan", glm::vec2(column_0, row_17), false, glm::vec4(0.4f, 0.4, 0.4f, 1.0f));
	// shaped
	AddTextBlock(tibetanFontName, textRendering_Tibetan, glm::vec2(column_1, row_17));
	// not shaped
	AddTextBlock(tibetanFontName, textRendering_Tibetan, glm::vec2(column_2, row_17), false);
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
void StartLevel::AddTextBlock(const std::string& fontName, const std::string& text, glm::vec2 pos, bool textShapeEnabled, glm::vec4 color)
{
	TextBlock2D::Properties tbprop;
	tbprop.fontName = fontName;
	tbprop.textShapeEnabled = textShapeEnabled;
	tbprop.fontSize = 16;

	TextBlock2D* notShapedTextBlock = Spawn<TextBlock2D>(tbprop);

	notShapedTextBlock->SetText(text);

	notShapedTextBlock->SetPosition(pos);
	notShapedTextBlock->SetTextColor(color);
}
}