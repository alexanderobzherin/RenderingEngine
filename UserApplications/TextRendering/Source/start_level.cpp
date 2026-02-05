#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "text_block_2d.hpp"
#include "text_renderer.hpp"
#include "i_application.hpp"

#include "spawn_drawables.hpp"

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

	const std::string textEng_TextRendering = "Text Rendering";
	const std::string textEng_FeatureDesc =
		"Text Rendering is a feature of\n"
		"the rendering engine that\n"
		"provides the ability to draw\n" 
		"strings containing text, digits,\n" 
		"and characters. It is a necessary \n"
		"feature for displaying blocks of\n"
		"text, widgets, buttons,\n"
		"annotations, et cetera.";

	const float screenWidth = static_cast<float>(GetSceneManager().GetApplication()->GetScreenSettings().width);
	// x:0.0f - center
	const float thirdPart = screenWidth / 3.0f;
	const float leftColumnStart = 0.0f - (screenWidth / 2.0f);
	const float centerColumnStart = leftColumnStart + thirdPart;
	const float rightColumnStart = leftColumnStart + (thirdPart * 2.0f);

	TextBlock2D::Properties lefttopbp;
	lefttopbp.fontSize = 16;
	
	TextBlock2D* textBlock_Left = Spawn<TextBlock2D>(lefttopbp);
	textBlock_Left->SetText(textEng_FeatureDesc);
	textBlock_Left->SetPosition(glm::vec2(leftColumnStart, -280.0f));
	textBlock_Left->SetTextColor(glm::vec4(1.0f, 0.0, 0.0f, 1.0f));

	// Updatable text block - counter
	mTextBlockUpdating = Spawn<TextBlock2D>(lefttopbp);
	mTextBlockUpdating->SetText(std::to_string(mCounter));

	mTextBlockUpdating->SetPosition(glm::vec2(0.0f, 200.0f));
	mTextBlockUpdating->SetTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Outline
	TextBlock2D::Properties olts54;
	olts54.fontSize = 54;
	olts54.outlineThicknessPx = 2.0f;
	TextBlock2D* outlineTextSize32 = Spawn<TextBlock2D>(olts54);
	outlineTextSize32->SetText("Outline Text Size 54");
	
	outlineTextSize32->SetPosition(glm::vec2(0.0f - (outlineTextSize32->GetDimensions().x / 2.0f), 250.0f));
	outlineTextSize32->SetTextColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	outlineTextSize32->SetOutlineColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	TextBlock2D::Properties chtb;
	chtb.fontName = "NotoSansSC-Regular";
	chtb.textAlign = TextAlign::Center;
	chtb.fontSize = 18;
	const std::string textChn_FeatureDesc =
		"文本渲染是渲染引擎的一项功能，\n"
		"它能够绘制包含文字、数字和符号\n"
		"的字符串。这是显示文本块、控件、\n"
		"按钮、注 释等内容所必需的功能。";
	TextBlock2D* textBlock_LeftBottom = Spawn<TextBlock2D>(chtb);
	textBlock_LeftBottom->SetText(textChn_FeatureDesc);
	textBlock_LeftBottom->SetPosition(glm::vec2(leftColumnStart, 0.0f));
	textBlock_LeftBottom->SetTextColor(glm::vec4(1.0f, 0.0, 0.0f, 1.0f));

	// Center column
	TextBlock2D::Properties centtb;
	centtb.textAlign = TextAlign::Center;
	centtb.fontSize = 16;
	TextBlock2D* textBlock_Center = Spawn<TextBlock2D>(centtb);
	textBlock_Center->SetText(textEng_FeatureDesc);
	textBlock_Center->SetPosition(glm::vec2(centerColumnStart, -280.0f));
	textBlock_Center->SetTextColor(glm::vec4(0.0f, 1.0, 0.0f, 1.0f));

	// Right column
	TextBlock2D::Properties rtb;
	rtb.textAlign = TextAlign::Right;
	rtb.fontSize = 16;
	TextBlock2D* textBlock_Right = Spawn<TextBlock2D>(rtb);
	textBlock_Right->SetText(textEng_FeatureDesc);
	textBlock_Right->SetPosition(glm::vec2(rightColumnStart, -280.0f));
	textBlock_Right->SetTextColor(glm::vec4(0.0f, 0.0, 1.0f, 1.0f));


	// Multi material text block
	std::string multiMaterialText = 
		"This is multi-material text block. \n"
		"Он отображает разные системы письма, \n"
		"а значит uses mutiple batches with \n"
		"differrent materials and font atlases.";
	TextBlock2D::Properties mmtb;
	mmtb.textAlign = TextAlign::Center;
	mmtb.fontSize = 16;
	TextBlock2D* multimaterialTextBlock = Spawn<TextBlock2D>(mmtb);
	multimaterialTextBlock->SetText(multiMaterialText);
	multimaterialTextBlock->SetPosition(glm::vec2(centerColumnStart, 0.0f));
	multimaterialTextBlock->SetTextColor(glm::vec4(1.0f, 1.0, 0.0f, 1.0f));

	const std::string hebrewFontName = "FrankRuhlLibre-Regular";
	const std::string arabicFontName = "Amiri-Regular";
	const std::string aramaicFontName = "NotoSansSyriac-Regular";
	const std::string textHebrew_FeatureDesc =
		"עיבוד טקסט הוא יכולת של מנוע \n"
		"הגרפיקה המאפשרת שרטוט מחרוזות \n"
		"הכוללות אותיות, ספרות ותווים. זוהי \n"
		"יכולת חיונית להצגת בלוקי טקסט, \n"
		"ווידג'טים, כפתורים, הערות וכדומה. \n";

	const std::string textArabic_FeatureDesc =
		"عرض النص هو ميزة في محرك الرسوميات \n"
		"تتيح رسم سلاسل تحتوي على حروف وأرقام \n"
		"ورموز. تُعد هذه الميزة ضرورية لعرض كتل \n"
		"نصية، وعناصر واجهة، وأزرار، وتعليقات، \n"
		"وغيرها. \n";

	const std::string textAramaic_FeatureDesc =
		"ܡܚܙܘܝܘܬܐ ܕܟܬܒܐ ܗ̱ܝ ܝܘܠܦܢܐ ܕܡܥܢܓܐ \n"
		"ܕܓܪܦܝܩܐ ܕܡܬܠܝܦܐ ܠܡܚܙܘܝܘ ܫܪ̈ܫܝܐ  \n"
		"ܕܒܗܘܢ ܐܘܬܘܬ̈ܐ، ܡܢ̈ܝ̈ܢܐ ܘܣܝܡ̈ܐ. ܗ̱ܝ  \n"
		"ܝܘܠܦܢܐ ܚܫܝܚܬܐ ܠܡܚܙܘܝܘ ܓܘܫܡ̈ܐ  \n"
		"ܕܟܬܒܐ، ܡܕܒܪ̈ܢܐ، ܟܘܦ̈ܝܐ، ܡܦܪ̈ܫܢܘܬܐ  \n"
		"ܘܐܚܪ̈ܢܐ. \n";

	TextBlock2D::Properties htdProp;
	htdProp.fontSize = 20;
	htdProp.fontName = hebrewFontName;
	htdProp.textAlign = TextAlign::Right;
	htdProp.textShapeEnabled = true;
	htdProp.maxLineLength = thirdPart;

	TextBlock2D* textBlock_Shaped_FeatureDesc = Spawn<TextBlock2D>(htdProp);

	textBlock_Shaped_FeatureDesc->SetText(textHebrew_FeatureDesc);
	textBlock_Shaped_FeatureDesc->SetPosition(glm::vec2(rightColumnStart, 0.0f));
	textBlock_Shaped_FeatureDesc->SetTextColor(glm::vec4(1.0f, 1.0, 1.0f, 1.0f));
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	mTicking++;
	if (mTicking > 100)
	{
		mTicking = 0;
		++mCounter;
		mTextBlockUpdating->SetText(std::to_string(mCounter));
	}
}
}