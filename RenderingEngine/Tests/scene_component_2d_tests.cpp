// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "scene_component_2d.hpp"
#include "glm_test_helpers.hpp"

using namespace rendering_engine;
using rendering_engine::test::ExpectNear;

TEST(SceneComponent2D, DefaultsToIdentity)
{
    SceneComponent2D component;
    ExpectNear(component.GetPosition(), glm::vec2(0));
    EXPECT_NEAR(component.GetRotation(), 0.0f, 1e-5f);
    ExpectNear(component.GetScale(), glm::vec2(1));
    ExpectNear(component.GetWorldMatrix(), glm::mat4(1));
}

TEST(SceneComponent2D, SettersComposeScaleRotationTranslation)
{
    SceneComponent2D component;
    component.GetWorldMatrix();
    component.SetPosition(glm::vec2(10, 20));
    component.SetRotation(90);
    component.SetScale(glm::vec2(2, 3));
    ExpectNear(component.GetPosition(), glm::vec2(10, 20));
    EXPECT_NEAR(component.GetRotation(), 90.0f, 1e-5f);
    ExpectNear(component.GetScale(), glm::vec2(2, 3));
    ExpectNear(component.GetWorldPosition(), glm::vec2(10, 20));
    ExpectNear(component.GetWorldScale(), glm::vec2(2, 3));
    // (1,2) -> (2,6) -> (-6,2) -> (4,22).
    ExpectNear(component.GetWorldMatrix() * glm::vec4(1, 2, 0, 1), glm::vec4(4, 22, 0, 1));
    ExpectNear(component.GetWorldMatrix() * glm::vec4(1, 2, 0, 0), glm::vec4(-6, 2, 0, 0));
    glm::mat4 expected(1);
    expected[0] = glm::vec4(0, 2, 0, 0);
    expected[1] = glm::vec4(-3, 0, 0, 0);
    expected[3] = glm::vec4(10, 20, 0, 1);
    ExpectNear(component.GetWorldMatrix(), expected);
}

TEST(SceneComponent2D, ParentTranslationRotationAndScaleAffectChild)
{
    SceneComponent2D parent; 
    SceneComponent2D child;
    child.SetPosition(glm::vec2(1, 2));
    child.AttachTo(&parent);
    parent.SetPosition(glm::vec2(10, 20));
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec2(11, 22));

    parent.SetPosition(glm::vec2(0));
    parent.SetRotation(90);
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec2(-2, 1));

    parent.SetRotation(0);
    parent.SetScale(glm::vec2(2, 3));
    child.SetScale(glm::vec2(3, 2));
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec2(2, 6));
    ExpectNear(child.GetWorldScale(), glm::vec2(6));
    // GetWorldRotation's extraction convention is deferred; matrices are tested.
}

TEST(SceneComponent2D, NestedTransformsRefreshWithExplicitUpdates)
{
    SceneComponent2D root; 
    SceneComponent2D child; 
    SceneComponent2D leaf;
    root.SetPosition(glm::vec2(10, 20));
    root.SetRotation(90);
    root.SetScale(glm::vec2(2));
    child.SetPosition(glm::vec2(1, 0));
    child.SetRotation(90);
    child.SetScale(glm::vec2(3));
    child.AttachTo(&root);
    leaf.SetPosition(glm::vec2(1, 0));
    leaf.AttachTo(&child);
    root.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    leaf.UpdateWorldMatrix();
    glm::mat4 expected(1);
    expected[0][0] = -6;
    expected[1][1] = -6;
    expected[3] = glm::vec4(4, 22, 0, 1);
    ExpectNear(leaf.GetWorldMatrix(), expected);
    ExpectNear(leaf.GetWorldPosition(), glm::vec2(4, 22));
    ExpectNear(leaf.GetWorldScale(), glm::vec2(6));

    root.SetPosition(glm::vec2(20, 30));
    root.UpdateWorldMatrix();
    child.UpdateWorldMatrix();
    leaf.UpdateWorldMatrix();
    expected[3] = glm::vec4(14, 32, 0, 1);
    ExpectNear(leaf.GetWorldMatrix(), expected);
}

TEST(SceneComponent2D, DetachRetainsLocalTransform)
{
    SceneComponent2D parent;
    SceneComponent2D child;
    child.SetPosition(glm::vec2(1, 2));
    child.SetRotation(30);
    child.SetScale(glm::vec2(2, 3));
    const glm::mat4 local = child.GetWorldMatrix();
    parent.SetPosition(glm::vec2(10, 0));
    child.AttachTo(&parent);
    child.UpdateWorldMatrix();
    ExpectNear(child.GetWorldPosition(), glm::vec2(11, 2));
    child.AttachTo(nullptr);
    ExpectNear(child.GetWorldMatrix(), local);
    ExpectNear(child.GetPosition(), glm::vec2(1, 2));
    EXPECT_NEAR(child.GetRotation(), 30.0f, 1e-5f);
    ExpectNear(child.GetScale(), glm::vec2(2, 3));
}
