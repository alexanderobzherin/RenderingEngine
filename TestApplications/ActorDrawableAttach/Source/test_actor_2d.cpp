#include "test_actor_2d.hpp"
#include "create_actor_2d_subobject.hpp"
#include "scene_component_2d.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"

using namespace rendering_engine;

namespace application
{
TestActor2D::TestActor2D(Scene& scene)
	:
	Actor2D(scene)
{}

void TestActor2D::Initialize()
{
	Actor2D::Initialize();

	bUpdateOnTick = true;

	Rectangle2D::Properties rectangleProp;
	rectangleProp.size = { 300.0f, 300.0f };
	rectangleProp.color = { 0.0f, 0.0f, 1.0f, 0.4f };

	mRectangle2D = CreateSubobject<Rectangle2D>(rectangleProp);
	mRectangle2D->GetTransform().AttachTo(&mRootComponent);

	const glm::vec4 textColor = {0.0f, 0.0f, 1.0f, 1.0f};
	TextBlock2D::Properties tbProp;
	tbProp.fontSize = 16;
	mLeftTextBlock2D = CreateSubobject<TextBlock2D>(tbProp);
	mLeftTextBlock2D->GetTransform().AttachTo(&mRootComponent);
	mLeftTextBlock2D->SetText("Left Text Block");
	mLeftTextBlock2D->SetTextColor(textColor);
	mLeftTextBlock2D->SetPosition(glm::vec2(-mLeftTextBlock2D->GetDimensions().x, 0.0f));

	mRightTextBlock2D = CreateSubobject<TextBlock2D>(tbProp);
	mRightTextBlock2D->SetText("Right Text Block");
	mRightTextBlock2D->SetTextColor(textColor);
	mRightTextBlock2D->GetTransform().AttachTo(&mRootComponent);

}

void TestActor2D::Update(float deltaTime)
{
	Actor2D::Update(deltaTime);

	const glm::vec2 oldPosition = GetPosition();
	const glm::vec2 newPosition = oldPosition + (mMovementDirection * mSpeed);
	SetPosition(newPosition);

	const glm::vec2 startPos = glm::vec2(0.0f, 0.0f);
	const glm::vec2 distance = startPos - newPosition;
	const float distanceFloat = glm::length(distance);

	const float oldRotation = GetRotation();
	const float newRotation = oldRotation + mAngularVelocity;
	SetRotation(newRotation);

	if (distanceFloat > 250.0f)
	{
		mMovementDirection *= -1.0f;
		mAngularVelocity *= -1.0f;
	}
}



} // namespace application
