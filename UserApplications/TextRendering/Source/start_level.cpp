#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "text_block_2d.hpp"
#include "text_renderer.hpp"
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

	mActiveCamera3D = std::make_shared<Camera>(*mSceneManager.GetApplication());
	mActiveCamera3D->Initialize();

	mActiveCamera2D = std::make_shared<Camera2D>(*mSceneManager.GetApplication());
	mActiveCamera2D->Initialize();

	const std::string textEng_TextRendering = "Text Rendering";
	const std::string textEng_FeatureDesc = 
		"Text Rendering is a feature of the rendering engine that provides the ability to draw strings containing text, digits, and characters. It is a necessary feature for displaying blocks of text, widgets, buttons, annotations, et cetera.";
	
	const float screenWidth = static_cast<float>(GetSceneManager().GetApplication()->GetScreenSettings().width);
	// x:0.0f - center
	const float thirdPart = screenWidth / 3.0f;
	const float leftColumnStart = 0.0f - (screenWidth / 2.0f);
	const float centerColumnStart = leftColumnStart + thirdPart;
	const float rightColumnStart = leftColumnStart + (thirdPart * 2.0f);

	const std::string fontName = "RobotoMono-Regular";
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	TextBlock2D* textBlock_Left = static_cast<TextBlock2D*>(mDrawables2D.back());

	textBlock_Left->SetMaxLineLength(thirdPart);
	textBlock_Left->SetTextAlign(TextAlign::Left);
	textBlock_Left->SetText(textEng_FeatureDesc);

	textBlock_Left->SetPosition(glm::vec2(leftColumnStart, -280.0f));
	textBlock_Left->SetTextColor(glm::vec4(1.0f, 0.0, 0.0f, 1.0f));

	// Center column
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	TextBlock2D* textBlock_Center = static_cast<TextBlock2D*>(mDrawables2D.back());

	textBlock_Center->SetMaxLineLength(thirdPart);
	textBlock_Center->SetTextAlign(TextAlign::Center);
	textBlock_Center->SetText(textEng_FeatureDesc);

	textBlock_Center->SetPosition(glm::vec2(centerColumnStart, -280.0f));
	textBlock_Center->SetTextColor(glm::vec4(0.0f, 1.0, 0.0f, 1.0f));

	// Right column
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	TextBlock2D* textBlock_Right = static_cast<TextBlock2D*>(mDrawables2D.back());

	textBlock_Right->SetMaxLineLength(thirdPart);
	textBlock_Right->SetTextAlign(TextAlign::Right);
	textBlock_Right->SetText(textEng_FeatureDesc);

	textBlock_Right->SetPosition(glm::vec2(rightColumnStart, -280.0f));
	textBlock_Right->SetTextColor(glm::vec4(0.0f, 0.0, 1.0f, 1.0f));

	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	mTextBlockUpdating = static_cast<TextBlock2D*>(mDrawables2D.back());
	mTextBlockUpdating->SetText(std::to_string(mCounter));

	mTextBlockUpdating->SetPosition(glm::vec2(0.0f, 200.0f));
	mTextBlockUpdating->SetTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Multy material text block
	std::string multiMaterialText = "This is multi-material text block. Он отображает разные системы письма, а значит uses mutiple batches with differrent materials and font atlases.";
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	TextBlock2D* multimaterialTextBlock = static_cast<TextBlock2D*>(mDrawables2D.back());

	multimaterialTextBlock->SetMaxLineLength(thirdPart);
	multimaterialTextBlock->SetTextAlign(TextAlign::Center);
	multimaterialTextBlock->SetText(multiMaterialText);

	multimaterialTextBlock->SetPosition(glm::vec2(centerColumnStart, 0.0f));
	multimaterialTextBlock->SetTextColor(glm::vec4(1.0f, 1.0, 0.0f, 1.0f));


	/// Arabic
	const std::string arabicFontName = "Amiri-Regular";
	const std::string textRendering_Arabic = "عرض النص";
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), arabicFontName));
	TextBlock2D* textRendering_Arabic_TB = static_cast<TextBlock2D*>(mDrawables2D.back());

	textRendering_Arabic_TB->SetMaxLineLength(thirdPart);
	textRendering_Arabic_TB->SetTextAlign(TextAlign::Right);
	textRendering_Arabic_TB->SetText(textRendering_Arabic);

	textRendering_Arabic_TB->SetPosition(glm::vec2(leftColumnStart, 0.0f));
	textRendering_Arabic_TB->SetTextColor(glm::vec4(1.0f, 1.0, 1.0f, 1.0f));
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