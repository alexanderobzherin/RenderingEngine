#include "test_button.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"
#include "color_space.hpp"
#include "interaction_router_2d.hpp"

using namespace rendering_engine;

namespace application
{
const glm::vec3 TestButton::sRectInactiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.227f, 0.455f, 0.545f));
const glm::vec3 TestButton::sRectActiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.851f, 0.573f, 0.0f));
const glm::vec2 TestButton::sBaseButtonSize = { 50.0f, 50.0f };

const glm::vec3 TestButton::sTextInactiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.376f, 0.827f, 1.0f));
const glm::vec3 TestButton::sTextActiveColor = ColorSpace::SRGBToLinear(glm::vec3(0.0f, 0.373f, 0.894f));
const std::string TestButton::sLabelFontName = "RobotoMono-Bold";
const unsigned int TestButton::sLabelFontSize = 20U;

TestButton::TestButton(Scene& scene)
	:
	Actor2D(scene),
	mRectangle2D(nullptr),
	mTextBlock2D(nullptr)
{}

void TestButton::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;

	SetButtonShape();
	SetButtonLabel();

	auto* geometry = mRectangle2D->GetPhysicalGeometry();

	// Bind button interaction with mouse
	// on pressed action
	auto onButtonPressed = [this]() {OnButtonPressed(); };
	// on released action
	auto onButtonReleased = [this](bool releaseInside) {OnButtonReleased(); };
	// on cancelled action
	auto onCancelled = [this]() {OnButtonReleased(); };
	GetScene().GetInteractionRouter2D().RegisterTarget(
		geometry, {onButtonPressed, onButtonReleased, onCancelled}
	);
}

void TestButton::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);
}

void TestButton::OnButtonPressed()
{
	mRectangle2D->SetColor({ sRectActiveColor , 1.0f });
	mTextBlock2D->SetTextColor({ sTextActiveColor , 1.0f });
}

void TestButton::OnButtonReleased()
{
	mRectangle2D->SetColor({ sRectInactiveColor , 1.0f });
	mTextBlock2D->SetTextColor({ sTextInactiveColor , 1.0f });
}

void TestButton::SetButtonShape()
{
	if (mRectangle2D)
	{
		mRectangle2D->Destroy();
	}

	Rectangle2D::Properties rectangleProp;
	rectangleProp.size = sBaseButtonSize;
	rectangleProp.color = { sRectInactiveColor, 1.0f };
	rectangleProp.physical.collisionEnabled = true;
	rectangleProp.physical.geometryType = PhysicalGeometryType::Rectangle;
	rectangleProp.physical.mobility = Mobility::Static;

	mRectangle2D = CreateSubobject<Rectangle2D>(rectangleProp);
	mRectangle2D->GetTransform().AttachTo(&mRootComponent);

	mRectangle2D->SetScale(sBaseButtonSize * mButtonSize);
}

void TestButton::SetButtonLabel()
{
	if (mTextBlock2D)
	{
		mTextBlock2D->Destroy();
	}

	TextBlock2D::Properties tbProp;
	tbProp.fontName = sLabelFontName;
	tbProp.fontSize = sLabelFontSize;
	mTextBlock2D = CreateSubobject<TextBlock2D>(tbProp);
	mTextBlock2D->GetTransform().AttachTo(&mRootComponent);

	mTextBlock2D->SetText(mButtonLabel);
	mTextBlock2D->SetTextColor({ sTextInactiveColor, 1.0f });
	const auto textDimensions = mTextBlock2D->GetDimensions();
	mTextBlock2D->SetPosition({ -textDimensions.x / 2.0f, textDimensions.y / 4.0f });
}

} // namespace application
