#include "pckeyboard_actor.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"

#include "input_event.hpp"
#include "pc_keyboard_key_actor.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"

using namespace rendering_engine;

namespace application
{
using KD = KeyDefinition;

// Function keys
const std::vector<KD> PCKeyboardActor::sLineFunction{
    {Key::Escape, "Esc", {1.0f, 1.0f}, 12},
    {Key::F1,  "F1",  {1.0f, 1.0f}, 12},
    {Key::F2,  "F2",  {1.0f, 1.0f}, 12},
    {Key::F3,  "F3",  {1.0f, 1.0f}, 12},
    {Key::F4,  "F4",  {1.0f, 1.0f}, 12},
    {Key::F5,  "F5",  {1.0f, 1.0f}, 12},
    {Key::F6,  "F6",  {1.0f, 1.0f}, 12},
    {Key::F7,  "F7",  {1.0f, 1.0f}, 12},
    {Key::F8,  "F8",  {1.0f, 1.0f}, 12},
    {Key::F9,  "F9",  {1.0f, 1.0f}, 12},
    {Key::F10, "F10", {1.0f, 1.0f}, 12},
    {Key::F11, "F11", {1.0f, 1.0f}, 12},
    {Key::F12, "F12", {1.0f, 1.0f}, 12}
};

// Number row
const std::vector<KD> PCKeyboardActor::sLine123{
    {Key::GraveAccent, "`", {1.0f, 1.0f}},
    {Key::Num1, "1", {1.0f, 1.0f}},
    {Key::Num2, "2", {1.0f, 1.0f}},
    {Key::Num3, "3", {1.0f, 1.0f}},
    {Key::Num4, "4", {1.0f, 1.0f}},
    {Key::Num5, "5", {1.0f, 1.0f}},
    {Key::Num6, "6", {1.0f, 1.0f}},
    {Key::Num7, "7", {1.0f, 1.0f}},
    {Key::Num8, "8", {1.0f, 1.0f}},
    {Key::Num9, "9", {1.0f, 1.0f}},
    {Key::Num0, "0", {1.0f, 1.0f}},
    {Key::Minus, "+", {1.0f, 1.0f}},
    {Key::Equal, "=", {1.0f, 1.0f}},
    {Key::Backspace, "Back", {1.8f, 1.0f}, 12}
};

// QWERTY row
const std::vector<KD> PCKeyboardActor::sLineQWERTY{
    {Key::Tab, "Tab", {1.5f, 1.0f}, 12},
    {Key::Q, "Q", {1.0f, 1.0f}},
    {Key::W, "W", {1.0f, 1.0f}},
    {Key::E, "E", {1.0f, 1.0f}},
    {Key::R, "R", {1.0f, 1.0f}},
    {Key::T, "T", {1.0f, 1.0f}},
    {Key::Y, "Y", {1.0f, 1.0f}},
    {Key::U, "U", {1.0f, 1.0f}},
    {Key::I, "I", {1.0f, 1.0f}},
    {Key::O, "O", {1.0f, 1.0f}},
    {Key::P, "P", {1.0f, 1.0f}},
    {Key::LeftBracket, "[", {1.0f, 1.0f}},
    {Key::RightBracket, "]", {1.0f, 1.0f}},
    {Key::Enter, "Enter", {1.3f, 1.0f}, 12}
};

// Home row
const std::vector<KD> PCKeyboardActor::sLineASDF{
    {Key::CapsLock, "Caps", {1.8f, 1.0f}, 12},
    {Key::A, "A", {1.0f, 1.0f}},
    {Key::S, "S", {1.0f, 1.0f}},
    {Key::D, "D", {1.0f, 1.0f}},
    {Key::F, "F", {1.0f, 1.0f}},
    {Key::G, "G", {1.0f, 1.0f}},
    {Key::H, "H", {1.0f, 1.0f}},
    {Key::J, "J", {1.0f, 1.0f}},
    {Key::K, "K", {1.0f, 1.0f}},
    {Key::L, "L", {1.0f, 1.0f}},
    {Key::Semicolon, ";", {1.0f, 1.0f}},
    {Key::Apostrophe, "'", {1.0f, 1.0f}},
    {Key::Backslash, "\\", {1.0f, 1.0f}}
};

// Bottom letter row
const std::vector<KD> PCKeyboardActor::sLineZXCV{
    {Key::LeftShift, "Shift", {1.9f, 1.0f}, 12},
    {Key::World1, "<", {1.0f, 1.0f}},
    {Key::Z, "Z", {1.0f, 1.0f}},
    {Key::X, "X", {1.0f, 1.0f}},
    {Key::C, "C", {1.0f, 1.0f}},
    {Key::V, "V", {1.0f, 1.0f}},
    {Key::B, "B", {1.0f, 1.0f}},
    {Key::N, "N", {1.0f, 1.0f}},
    {Key::M, "M", {1.0f, 1.0f}},
    {Key::Comma, ",", {1.0f, 1.0f}},
    {Key::Period, ".", {1.0f, 1.0f}},
    {Key::Slash, "-", {1.0f, 1.0f}},
    {Key::RightShift, "Shift", {2.0f, 1.0f}, 12}
};

// Modifier / space row
const std::vector<KD> PCKeyboardActor::sLineSpace{
    {Key::LeftControl, "Ctrl", {1.3f, 1.0f}, 12},
    {Key::Unknown, "Fn", {1.0f, 1.0f}, 12},
    {Key::LeftSuper, "Win", {1.0f, 1.0f}, 12},
    {Key::LeftAlt, "Alt", {1.0f, 1.0f}, 12},
    {Key::Space, "", {7.62f, 1.0f}, 12},
    {Key::RightAlt, "AltGr", {1.3f, 1.0f}, 12},
    {Key::Menu, "Menu", {1.0f, 1.0f}, 12},
    {Key::RightControl, "Ctrl", {1.3f, 1.0f}, 12}
};


const std::vector<KD> PCKeyboardActor::sLineSystem{
    {Key::PrintScreen, "PrtSc",  {1.0f, 1.0f}, 10},
    {Key::ScrollLock,  "Scroll", {1.0f, 1.0f}, 10},
    {Key::Pause,       "Pause",  {1.0f, 1.0f}, 10}
};

const std::vector<KD> PCKeyboardActor::sLineNavigation1{
    {Key::Insert, "Ins",  {1.0f, 1.0f}, 12},
    {Key::Home,   "Home", {1.0f, 1.0f}, 12},
    {Key::PageUp, "PgUp", {1.0f, 1.0f}, 12}
};

const std::vector<KD> PCKeyboardActor::sLineNavigation2{
    {Key::Delete,   "Del",  {1.0f, 1.0f}, 12},
    {Key::End,      "End",  {1.0f, 1.0f}, 12},
    {Key::PageDown, "PgDn", {1.0f, 1.0f}, 12}
};

const std::vector<KD> PCKeyboardActor::sLineArrows{
    {Key::Left,  "Left",  {1.0f, 1.0f}, 12},
    {Key::Down,  "Down",  {1.0f, 1.0f}, 12},
    {Key::Right, "Right", {1.0f, 1.0f}, 12}
};

const std::vector<KD> PCKeyboardActor::sLineNumpadTop{
    {Key::NumLock,        "Num", {1.0f, 1.0f}, 12},
    {Key::KeypadDivide,   "/",   {1.0f, 1.0f}},
    {Key::KeypadMultiply, "*",   {1.0f, 1.0f}},
    {Key::KeypadSubtract, "-",   {1.0f, 1.0f}}
};

const std::vector<KD> PCKeyboardActor::sLineNumpad789{
    {Key::Keypad7, "7", {1.0f, 1.0f}},
    {Key::Keypad8, "8", {1.0f, 1.0f}},
    {Key::Keypad9, "9", {1.0f, 1.0f}}
};

const std::vector<KD> PCKeyboardActor::sLineNumpad456{
    {Key::Keypad4, "4", {1.0f, 1.0f}},
    {Key::Keypad5, "5", {1.0f, 1.0f}},
    {Key::Keypad6, "6", {1.0f, 1.0f}}
};

const std::vector<KD> PCKeyboardActor::sLineNumpad123{
    {Key::Keypad1, "1", {1.0f, 1.0f}},
    {Key::Keypad2, "2", {1.0f, 1.0f}},
    {Key::Keypad3, "3", {1.0f, 1.0f}}
};

const std::vector<KD> PCKeyboardActor::sLineNumpad0{
    {Key::Keypad0,       "0", {2.12f, 1.0f}},
    {Key::KeypadDecimal, ".", {1.0f, 1.0f}}
};

const KeyDefinition PCKeyboardActor::sNumpadAdd{
    Key::KeypadAdd, "+", {1.0f, 2.12f}
};

const KeyDefinition PCKeyboardActor::sNumpadEnter{
    Key::KeypadEnter, "Enter", {1.0f, 2.12f}, 12
};

const KeyDefinition PCKeyboardActor::sArrowUp{
    Key::Up, "Up", {1.0f, 1.0f}, 12
};

constexpr float kKeySize = 50.0f;
constexpr float kKeyGap = 6.0f;
constexpr float kRowPitch = kKeySize + kKeyGap;
constexpr float kMainNavigationGap = 40.0f;
constexpr float kNavigationNumpadGap = 35.0f;

PCKeyboardActor::PCKeyboardActor(Scene& scene)
	:
	Actor2D(scene)
{}

void PCKeyboardActor::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;

	ComposeKeyboard();
}

void PCKeyboardActor::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);
}

void PCKeyboardActor::AddKey(const KeyDefinition& definition, const glm::vec2& position)
{
    auto* keyActor = GetScene().SpawnActor2D<PCKeyboardKeyActor>();
    keyActor->GetTransform().AttachTo(&mRootComponent);
    keyActor->ConfigureKey(definition);
    keyActor->SetPosition(position);

    mKeys.push_back(keyActor);

    // Calculate the key's bounding rectangle.
    const glm::vec2 keySize = glm::vec2(kKeySize) * definition.scale;

    const glm::vec2 keyMin = position - keySize * 0.5f;
    const glm::vec2 keyMax = position + keySize * 0.5f;

    if (!bHasLayoutBounds)
    {
        mLayoutMin = keyMin;
        mLayoutMax = keyMax;
        bHasLayoutBounds = true;
    }
    else
    {
        mLayoutMin = glm::min(mLayoutMin, keyMin);
        mLayoutMax = glm::max(mLayoutMax, keyMax);
    }

    mDimensions = mLayoutMax - mLayoutMin;
}

float PCKeyboardActor::CalculateRowWidth(const std::vector<KeyDefinition>& keys)
{
    if (keys.empty())
        return 0.0f;

    float width = 0.0f;

    for (const auto& definition : keys)
    {
        width += kKeySize * definition.scale.x;
    }

    width += kKeyGap * (keys.size() - 1);

    return width;
}

void PCKeyboardActor::ComposeRow(const std::vector<KeyDefinition>& keys, const glm::vec2& origin)
{
    float x = origin.x;

    for (const auto& definition : keys)
    {
        const float width = kKeySize * definition.scale.x;
        x += width * 0.5f;
        AddKey(definition, { x, origin.y });
        x += width * 0.5f + kKeyGap;
    }
}

void PCKeyboardActor::ComposeKeyboard()
{
    mDimensions = { 0.0f, 0.0f };
    mLayoutMin = { 0.0f, 0.0f };
    mLayoutMax = { 0.0f, 0.0f };
    bHasLayoutBounds = false;

    // Main keyboard block
    constexpr float mainX = 0.0f;

    ComposeRow(sLineFunction, { mainX, 0.0f });
    ComposeRow(sLine123, { mainX, 75.0f });
    ComposeRow(sLineQWERTY, { mainX, 131.0f });
    ComposeRow(sLineASDF, { mainX, 187.0f });
    ComposeRow(sLineZXCV, { mainX, 243.0f });
    ComposeRow(sLineSpace, { mainX, 299.0f });

    const float mainBlockWidth = CalculateRowWidth(sLine123);

    const float navX = mainX + mainBlockWidth + kMainNavigationGap;

    ComposeRow(sLineSystem, { navX, 0.0f });
    ComposeRow(sLineNavigation1, { navX, 75.0f });
    ComposeRow(sLineNavigation2, { navX, 131.0f });

    AddKey(sArrowUp, { navX + kRowPitch + kKeySize * 0.5f, 243.0f });

    ComposeRow(sLineArrows, { navX, 299.0f });

    const float navBlockWidth = CalculateRowWidth(sLineNavigation1);

    const float numX = navX + navBlockWidth + kNavigationNumpadGap;

    ComposeRow(sLineNumpadTop, { numX, 75.0f });
    ComposeRow(sLineNumpad789, { numX, 131.0f });
    ComposeRow(sLineNumpad456, { numX, 187.0f });
    ComposeRow(sLineNumpad123, { numX, 243.0f });
    ComposeRow(sLineNumpad0, { numX, 299.0f });

    // Tall numeric keypad keys
    const float tallKeyX = numX + 3.0f * kRowPitch + kKeySize * 0.5f;

    AddKey(sNumpadAdd, { tallKeyX, 159.0f });
    AddKey(sNumpadEnter, { tallKeyX, 271.0f });
}


} // namespace application
