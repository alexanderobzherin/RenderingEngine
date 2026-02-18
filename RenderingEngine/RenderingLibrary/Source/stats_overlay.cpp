#include "stats_overlay.hpp"
#include "spawn_drawables.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"
#include "utility.hpp"
#include "scene.hpp"
#include "i_application.hpp"

#include <cstdio>

namespace rendering_engine
{
StatsOverlay::StatsOverlay(Scene& scene)
	:
	Actor2D(scene),
	mFPSLabel(nullptr),
	mFPSValue(nullptr),
	mFrameLabel(nullptr),
	mFrameValue(nullptr),
	mUpdateLabel(nullptr),
	mUpdateValue(nullptr),
	mDrawLabel(nullptr),
	mDrawValue(nullptr)
{
	bUseSmoothedFPS = Utility::ReadConfigFile().useSmoothedFPS;
}

void StatsOverlay::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;

	TextBlock2D::Properties tbProp;
	tbProp.fontName = "RobotoMono-Medium";

	const float vertDelta = 25.0f;
	const float width = 140.0f;
	const float pad = 5.0f;
	const float height = vertDelta * 4;

	Rectangle2D::Properties rectangleProp;
	rectangleProp.color = mBgColor;
	rectangleProp.size = { width, height };
	mBackGround = CreateSubobject<Rectangle2D>(rectangleProp);
	mBackGround->GetTransform().AttachTo(&mRootComponent);

	const float middleX = width / 2.0f;
	const float middleY = height / 2.0f;
	const float column_0 = -middleX + pad;
	const float column_1 = column_0 + middleX + pad;

	const float row_0 = -middleY + tbProp.fontSize + pad;
	const float row_1 = row_0 + vertDelta;
	const float row_2 = row_1 + vertDelta;
	const float row_3 = row_2 + vertDelta;
	
	mFPSLabel = CreateSubobject<TextBlock2D>(tbProp);
	mFPSLabel->GetTransform().AttachTo(&mRootComponent);
	mFPSLabel->SetText("FPS:");
	mFPSLabel->SetPosition(glm::vec2(column_0, row_0));

	mFPSValue = CreateSubobject<TextBlock2D>(tbProp);
	mFPSValue->GetTransform().AttachTo(&mRootComponent);
	mFPSValue->SetPosition(glm::vec2(column_1, row_0));

	mFrameLabel = CreateSubobject<TextBlock2D>(tbProp);
	mFrameLabel->GetTransform().AttachTo(&mRootComponent);
	mFrameLabel->SetText("Frame:");
	mFrameLabel->SetPosition(glm::vec2(column_0, row_1));

	mFrameValue = CreateSubobject<TextBlock2D>(tbProp);
	mFrameValue->GetTransform().AttachTo(&mRootComponent);
	mFrameValue->SetPosition(glm::vec2(column_1, row_1));

	mUpdateLabel = CreateSubobject<TextBlock2D>(tbProp);
	mUpdateLabel->GetTransform().AttachTo(&mRootComponent);
	mUpdateLabel->SetText("Update:");
	mUpdateLabel->SetPosition(glm::vec2(column_0, row_2));

	mUpdateValue = CreateSubobject<TextBlock2D>(tbProp);
	mUpdateValue->GetTransform().AttachTo(&mRootComponent);
	mUpdateValue->SetPosition(glm::vec2(column_1, row_2));

	mDrawLabel = CreateSubobject<TextBlock2D>(tbProp);
	mDrawLabel->GetTransform().AttachTo(&mRootComponent);
	mDrawLabel->SetText("Draw:");
	mDrawLabel->SetPosition(glm::vec2(column_0, row_3));

	mDrawValue = CreateSubobject<TextBlock2D>(tbProp);
	mDrawValue->GetTransform().AttachTo(&mRootComponent);
	mDrawValue->SetPosition(glm::vec2(column_1, row_3));
}

void StatsOverlay::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);

	mTimeAccumulator += deltaTime;
	if (mTimeAccumulator < mUpdateInterval)
		return;

	mTimeAccumulator = 0.0f;

	const auto frameMetrics = GetScene().GetSceneManager().GetApplication()->GetFrameMetrics();
	
	float fps = 0.0f;
	if (bUseSmoothedFPS)
	{
		fps = frameMetrics.fpsSmoothed;
	}
	else
	{
		fps = frameMetrics.fpsRaw;
	}

	char bufferFPS[16];
	std::snprintf(bufferFPS, sizeof(bufferFPS), "%.2f", fps);
	mFPSValue->SetText(bufferFPS);

	char bufferFrame[16];
	std::snprintf(bufferFrame, sizeof(bufferFrame), "%.2f", frameMetrics.frameDurationMs);
	mFrameValue->SetText(bufferFrame);

	char bufferUpdate[16];
	std::snprintf(bufferUpdate, sizeof(bufferUpdate), "%.2f", frameMetrics.updateTimeMs);
	mUpdateValue->SetText(bufferUpdate);

	char bufferDraw[16];
	std::snprintf(bufferDraw, sizeof(bufferDraw), "%.2f", frameMetrics.drawTimeMs);
	mDrawValue->SetText(bufferDraw);
}

} // namespace rendering_engine
	