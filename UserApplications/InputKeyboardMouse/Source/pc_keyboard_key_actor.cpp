#include "pc_keyboard_key_actor.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"
#include "scene_manager.hpp"
#include "i_application.hpp"
#include "input_controller.hpp"
#include "input_event.hpp"
#include "color_space.hpp"
#include "input_event.hpp"

using namespace rendering_engine;

namespace application
{
const glm::vec3 PCKeyboardKeyActor::sRectInactiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.227f, 0.455f, 0.545f));
const glm::vec3 PCKeyboardKeyActor::sRectActiveColor   = ColorSpace::SRGBToLinear(glm::vec3(0.851f, 0.573f, 0.0f));
const glm::vec2 PCKeyboardKeyActor::sBaseButtonSize    = { 50.0f, 50.0f };

const glm::vec3 PCKeyboardKeyActor::sTextInactiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.376f, 0.827f, 1.0f));
const glm::vec3 PCKeyboardKeyActor::sTextActiveColor   = ColorSpace::SRGBToLinear(glm::vec3(0.0f, 0.373f, 0.894f));
const std::string PCKeyboardKeyActor::sLabelFontName   = "RobotoMono-Bold";
const unsigned int PCKeyboardKeyActor::sLabelFontSize  = 20U;

PCKeyboardKeyActor::PCKeyboardKeyActor(Scene& scene)
	:
	Actor2D(scene),
	mTextBlock2D(nullptr),
	mRectangle2D(nullptr)
{}

void PCKeyboardKeyActor::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;
}

void PCKeyboardKeyActor::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);

	const auto* input = GetScene().GetInputController();

	if (mKeyDefinition.key == Key::Unknown)
	{
		return;
	}

	if (input->IsKeyPressed(mKeyDefinition.key))
	{
		OnKeyPressed();
	}

	if (input->IsKeyReleased(mKeyDefinition.key))
	{
		OnKeyReleased();
	}
}

void PCKeyboardKeyActor::OnKeyPressed()
{
	mRectangle2D->SetColor({ sRectActiveColor , 1.0f});
	mTextBlock2D->SetTextColor({ sTextActiveColor , 1.0f });
}

void PCKeyboardKeyActor::OnKeyReleased()
{
	mRectangle2D->SetColor({ sRectInactiveColor , 1.0f });
	mTextBlock2D->SetTextColor({ sTextInactiveColor , 1.0f });
}

void PCKeyboardKeyActor::ConfigureKey(const KeyDefinition& keyDefinition)
{
	mKeyDefinition = keyDefinition;
	SetButtonShape();
	SetButtonLabel();
}

void PCKeyboardKeyActor::SetButtonShape()
{
	if (mRectangle2D)
	{
		mRectangle2D->Destroy();
	}

	Rectangle2D::Properties rectangleProp;
	rectangleProp.size = sBaseButtonSize;
	rectangleProp.color = { sRectInactiveColor, 1.0f };

	mRectangle2D = CreateSubobject<Rectangle2D>(rectangleProp);
	mRectangle2D->GetTransform().AttachTo(&mRootComponent);

	mRectangle2D->SetScale(sBaseButtonSize * mKeyDefinition.scale);
}

void PCKeyboardKeyActor::SetButtonLabel()
{
	if(mTextBlock2D)
	{
		mTextBlock2D->Destroy();
	}

	TextBlock2D::Properties tbProp;
	tbProp.fontName = sLabelFontName;
	tbProp.fontSize = mKeyDefinition.fontSize.has_value() ? mKeyDefinition.fontSize.value() : sLabelFontSize;
	mTextBlock2D = CreateSubobject<TextBlock2D>(tbProp);
	mTextBlock2D->GetTransform().AttachTo(&mRootComponent);

	mTextBlock2D->SetText(mKeyDefinition.label);
	mTextBlock2D->SetTextColor({ sTextInactiveColor, 1.0f });
	const auto textDimensions = mTextBlock2D->GetDimensions();
	mTextBlock2D->SetPosition({-textDimensions.x / 2.0f, textDimensions.y / 4.0f });
}

} // namespace application
