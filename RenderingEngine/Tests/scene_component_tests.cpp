// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "scene_component.hpp"
#include "glm_test_helpers.hpp"

using namespace rendering_engine;
using rendering_engine::test::ExpectNear;

TEST(SceneComponent, DefaultsToIdentityAndEngineAxes)
{
    SceneComponent component;
    ExpectNear(component.GetPosition(), glm::vec3(0));
    ExpectNear(component.GetScale(), glm::vec3(1));
    ExpectNear(component.GetRotation(), glm::vec3(0));
    ExpectNear(glm::mat4_cast(component.GetRotationQuat()), glm::mat4(1));
    ExpectNear(component.GetWorldMatrix(), glm::mat4(1));
    ExpectNear(component.GetForward(), glm::vec3(1, 0, 0));
    ExpectNear(component.GetRight(), glm::vec3(0, 1, 0));
    ExpectNear(component.GetUp(), glm::vec3(0, 0, 1));
}

TEST(SceneComponent, PositionAndScaleUpdateAnEvaluatedRoot)
{
    SceneComponent component;
    component.GetWorldMatrix();
    component.SetPosition(glm::vec3(3, -4, 5));
    component.SetScale(glm::vec3(2, 3, 4));
    ExpectNear(component.GetPosition(), glm::vec3(3, -4, 5));
    ExpectNear(component.GetWorldPosition(), glm::vec3(3, -4, 5));
    ExpectNear(component.GetScale(), glm::vec3(2, 3, 4));
    ExpectNear(component.GetWorldScale(), glm::vec3(2, 3, 4));
    glm::mat4 expected(1);
    expected[0][0] = 2;
    expected[1][1] = 3;
    expected[2][2] = 4;
    expected[3] = glm::vec4(3, -4, 5, 1);
    ExpectNear(component.GetWorldMatrix(), expected);
}

TEST(SceneComponent, IndependentEulerRotationsUseDegreesAndEngineSigns)
{
    struct Case 
    { 
        glm::vec3 angles;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up; 
    };
    const Case cases[] = {
        {{90, 0, 0}, {0, 0, 1}, {0, 1, 0}, {-1, 0, 0}},
        {{0, 90, 0}, {0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
        {{0, 0, 90}, {1, 0, 0}, {0, 0, -1}, {0, 1, 0}}
    };
    SceneComponent component;
    for (const auto& item : cases)
    {
        SCOPED_TRACE(::testing::Message() << "Euler " << item.angles.x << ", "
                     << item.angles.y << ", " << item.angles.z);
        component.SetRotation(item.angles);
        ExpectNear(component.GetRotation(), item.angles);
        ExpectNear(component.GetForward(), item.forward);
        ExpectNear(component.GetRight(), item.right);
        ExpectNear(component.GetUp(), item.up);
    }
}

TEST(SceneComponent, QuaternionSetterControlsOrientation)
{
    SceneComponent component;
    const auto rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1));
    component.SetRotation(rotation);
    // Compare orientations, allowing the equivalent quaternion signs q and -q.
    EXPECT_NEAR(glm::abs(glm::dot(component.GetRotationQuat(), rotation)), 1.0f, 1e-5f);
    ExpectNear(component.GetForward(), glm::vec3(0, 1, 0));
    ExpectNear(component.GetRight(), glm::vec3(-1, 0, 0));
    ExpectNear(component.GetUp(), glm::vec3(0, 0, 1));
    ExpectNear(glm::mat4_cast(component.GetWorldRotation()), glm::mat4_cast(rotation));
    // Quaternion-to-Euler conversion is deferred pending its public convention.
}

TEST(SceneComponent, ScalesThenRotatesThenTranslates)
{
    SceneComponent component;
    component.SetPosition(glm::vec3(10, 20, 30));
    component.SetRotation(glm::vec3(0, 90, 0));
    component.SetScale(glm::vec3(2, 3, 4));
    // (1,2,3) -> (2,6,12) -> (6,-2,12) -> (16,18,42).
    ExpectNear(component.GetWorldMatrix() * glm::vec4(1, 2, 3, 1), glm::vec4(16, 18, 42, 1));
    ExpectNear(component.GetWorldMatrix() * glm::vec4(1, 2, 3, 0), glm::vec4(6, -2, 12, 0));
}

TEST(SceneComponent, ParentTranslationRotationAndScaleAffectChild)
{
    SceneComponent parent; 
    SceneComponent child;
    child.SetPosition(glm::vec3(1, 2, 3));
    child.AttachTo(&parent);
    parent.SetPosition(glm::vec3(10, 20, 30));
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec3(11, 22, 33));

    parent.SetPosition(glm::vec3(0));
    parent.SetRotation(glm::vec3(0, 90, 0));
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec3(2, -1, 3));
    ExpectNear(glm::mat4_cast(child.GetWorldRotation()), glm::mat4_cast(parent.GetRotationQuat()));

    parent.SetRotation(glm::vec3(0));
    parent.SetScale(glm::vec3(2, 3, 4));
    child.SetScale(glm::vec3(3, 2, 5));
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec3(2, 6, 12));
    ExpectNear(child.GetWorldScale(), glm::vec3(6, 6, 20));
}

TEST(SceneComponent, NestedTransformsRefreshWithExplicitUpdates)
{
    SceneComponent root; 
    SceneComponent child; 
    SceneComponent leaf;
    root.SetPosition(glm::vec3(10, 20, 30));
    root.SetRotation(glm::vec3(0, 90, 0));
    root.SetScale(glm::vec3(2));
    child.SetPosition(glm::vec3(1, 0, 0));
    child.SetRotation(glm::vec3(0, 90, 0));
    child.SetScale(glm::vec3(3));
    child.AttachTo(&root);
    leaf.SetPosition(glm::vec3(1, 0, 0));
    leaf.AttachTo(&child);
    root.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    leaf.UpdateWorldMatrix();
    glm::mat4 expected(1);
    expected[0][0] = -6;
    expected[1][1] = -6;
    expected[2][2] = 6;
    expected[3] = glm::vec4(4, 18, 30, 1);
    ExpectNear(leaf.GetWorldMatrix(), expected);
    ExpectNear(leaf.GetWorldPosition(), glm::vec3(4, 18, 30));
    ExpectNear(leaf.GetWorldScale(), glm::vec3(6));

    // Actor/Drawable update flow explicitly refreshes each world transform.
    root.SetPosition(glm::vec3(20, 30, 40));
    root.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    leaf.UpdateWorldMatrix();
    expected[3] = glm::vec4(14, 28, 40, 1);
    ExpectNear(leaf.GetWorldMatrix(), expected);
}

TEST(SceneComponent, WorldRotationCombinesUnscaledParentAndChild)
{
    SceneComponent parent; 
    SceneComponent child;
    parent.SetRotation(glm::vec3(0, 90, 0));
    child.SetRotation(glm::vec3(0, 0, 90));
    child.AttachTo(&parent);
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    // Roll maps local Y to -Z and Z to Y; parent's yaw then maps Y to X.
    glm::mat4 expected(1);
    expected[0] = glm::vec4(0, -1, 0, 0);
    expected[1] = glm::vec4(0, 0, -1, 0);
    expected[2] = glm::vec4(1, 0, 0, 0);
    ExpectNear(glm::mat4_cast(child.GetWorldRotation()), expected);
    ExpectNear(child.GetWorldMatrix(), expected);
}

TEST(SceneComponent, DetachRetainsLocalTransform)
{
    SceneComponent parent; 
    SceneComponent child;
    child.SetPosition(glm::vec3(1, 2, 3));
    child.SetRotation(glm::vec3(10, 20, 30));
    child.SetScale(glm::vec3(2, 3, 4));
    const glm::mat4 local = child.GetWorldMatrix();
    parent.SetPosition(glm::vec3(10, 0, 0));
    child.AttachTo(&parent);
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec3(11, 2, 3));
    child.AttachTo(nullptr);
    ExpectNear(child.GetWorldMatrix(), local);
    ExpectNear(child.GetPosition(), glm::vec3(1, 2, 3));
    ExpectNear(child.GetRotation(), glm::vec3(10, 20, 30));
    ExpectNear(child.GetScale(), glm::vec3(2, 3, 4));
}
