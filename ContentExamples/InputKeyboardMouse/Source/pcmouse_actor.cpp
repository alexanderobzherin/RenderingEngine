#include "pcmouse_actor.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"
#include "sprite_2d.hpp"
#include "color_space.hpp"
#include "text_block_2d.hpp"

#include "scene.hpp"
#include "input_controller.hpp"
#include "input_event.hpp"

#include <algorithm>
#include <string>

using namespace rendering_engine;

namespace application
{
const glm::vec3 PCMouseActor::sInactiveColor{ColorSpace::SRGBToLinear(glm::vec3(0.473f, 0.91f, 0.972f))};
const glm::vec3 PCMouseActor::sActiveColor{ColorSpace::SRGBToLinear(glm::vec3(1.0f, 0.873f, 0.531f))};

PCMouseActor::PCMouseActor(Scene& scene)
	:
	Actor2D(scene)
{}

void PCMouseActor::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;

	TextBlock2D::Properties tbProp;
	tbProp.fontName = "RobotoMono-Bold";
	tbProp.fontSize = 22U;
	mCursorPosTextLabel = CreateSubobject<TextBlock2D>(tbProp);
	mCursorPosTextLabel->SetText("Cursor Screen Position:");
	mCursorPosTextLabel->SetTextColor({ ColorSpace::SRGBToLinear(glm::vec3(0.376f, 0.827f, 1.0f)), 1.0f });
	mCursorPosTextLabel->SetPosition({ -(mCursorPosTextLabel->GetDimensions().x / 2) - 100.0f, -300.0f });
	
	const int cursorPosX = 0;
	const int cursorPosY = 0;
	std::string cursCoordStr = std::to_string(cursorPosX) + " , " + std::to_string(cursorPosY);
	mCursorPosCoordinates = CreateSubobject<TextBlock2D>(tbProp);
	mCursorPosCoordinates->SetText(cursCoordStr);
	mCursorPosCoordinates->SetTextColor({ ColorSpace::SRGBToLinear(glm::vec3(0.376f, 0.827f, 1.0f)), 1.0f });
	mCursorPosCoordinates->SetPosition({(mCursorPosTextLabel->GetPosition().x + 
		mCursorPosTextLabel->GetDimensions().x + 10), -300.0f});

	const std::string textureMouseBottom = "MouseBottom";
	mMouseBottom = CreateSubobject<Sprite2D>(textureMouseBottom);
	mMouseBottom->GetTransform().AttachTo(&mRootComponent);
	mMouseBottom->SetColor({ sInactiveColor , 1.0f});

	const std::string textureMouseLB = "MouseLB";
	mMouseLB = CreateSubobject<Sprite2D>(textureMouseLB);
	mMouseLB->GetTransform().AttachTo(&mRootComponent);
	mMouseLB->SetColor({ sInactiveColor , 1.0f });

	const std::string textureMouseRB = "MouseRB";
	mMouseRB = CreateSubobject<Sprite2D>(textureMouseRB);
	mMouseRB->GetTransform().AttachTo(&mRootComponent);
	mMouseRB->SetColor({ sInactiveColor , 1.0f });

	const std::string textureWheelDown = "MouseWheelDown";
	mMouseWheelDown = CreateSubobject<Sprite2D>(textureWheelDown);
	mMouseWheelDown->GetTransform().AttachTo(&mRootComponent);
	mMouseWheelDown->SetColor({ sInactiveColor , 1.0f });

	const std::string textureWheelUp = "MouseWheelUp";
	mMouseWheelUp = CreateSubobject<Sprite2D>(textureWheelUp);
	mMouseWheelUp->GetTransform().AttachTo(&mRootComponent);
	mMouseWheelUp->SetColor({ sInactiveColor , 1.0f });

}

void PCMouseActor::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);

    const auto* input = GetScene().GetInputController();

    if (!input)
    {
        return;
    }

    // Left mouse button
    if (input->IsMouseButtonPressed(MouseButton::Left))
    {
        OnLeftButtonPressed();
    }

    if (input->IsMouseButtonReleased(MouseButton::Left))
    {
        OnLeftButtonReleased();
    }

    // Right mouse button
    if (input->IsMouseButtonPressed(MouseButton::Right))
    {
        OnRightButtonPressed();
    }

    if (input->IsMouseButtonReleased(MouseButton::Right))
    {
        OnRightButtonReleased();
    }

    // Cursor position
    const int cursorX =
        static_cast<int>(input->GetMouseX());

    const int cursorY =
        static_cast<int>(input->GetMouseY());

    const std::string coordinates =
        std::to_string(cursorX) + " , " +
        std::to_string(cursorY);

    mCursorPosCoordinates->SetText(coordinates);

    // Mouse wheel highlight duration (seconds)
    constexpr float highlightDuration = 0.15f;

    mWheelUpHighlightTime =
        std::max(0.0f, mWheelUpHighlightTime - deltaTime);

    mWheelDownHighlightTime =
        std::max(0.0f, mWheelDownHighlightTime - deltaTime);

    const double scrollY = input->GetMouseScrollY();

    if (scrollY > 0.0)
    {
        OnWheelUp();
        mWheelUpHighlightTime = highlightDuration;
    }
    else if (scrollY < 0.0)
    {
        OnWheelDown();
        mWheelDownHighlightTime = highlightDuration;
    }

    mMouseWheelUp->SetColor({
        mWheelUpHighlightTime > 0.0f
            ? sActiveColor : sInactiveColor,
        1.0f
        });

    mMouseWheelDown->SetColor({
        mWheelDownHighlightTime > 0.0f
            ? sActiveColor : sInactiveColor,
        1.0f
        });
}

void PCMouseActor::OnLeftButtonPressed()
{
	mMouseLB->SetColor({ sActiveColor , 1.0f});
}

void PCMouseActor::OnLeftButtonReleased()
{
	mMouseLB->SetColor({ sInactiveColor , 1.0f });
}

void PCMouseActor::OnRightButtonPressed()
{
	mMouseRB->SetColor({ sActiveColor , 1.0f });
}

void PCMouseActor::OnRightButtonReleased()
{
	mMouseRB->SetColor({ sInactiveColor , 1.0f });
}

void PCMouseActor::OnWheelUp()
{
	mMouseWheelUp->SetColor({ sActiveColor , 1.0f });
}

void PCMouseActor::OnWheelDown()
{
	mMouseWheelDown->SetColor({ sActiveColor , 1.0f });
}

} // namespace application
