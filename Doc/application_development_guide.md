# Application Development Guide

## Creating a 3D Actor

You can create a 3D actor using:


```bash
python3 Scripts/create_class.py Actor MyActor
```

This will generate the files:

```bash
Include/my_actor.hpp
Source/my_actor.cpp
```

In your ```MyActor``` class you can create and attach drawable components derived from ```Drawable3D```, such as ```StaticMesh```.

Example:

```cpp
class MyActor : public Actor
{
    StaticMesh* mMesh;
};
```

```cpp
void MyActor::Initialize()
{
    Actor::Initialize();

    bUpdateOnTick = true;

    StaticMeshParams cubeMeshParams;
    cubeMeshParams.meshName = "cube";
    cubeMeshParams.materials = { { 0U, "BasicTexture3D" } };

    // Create mesh subobject
    mMesh = CreateSubobject<StaticMesh>(cubeMeshParams);

    // Attach to root component to inherit actor transformations
    mMesh->GetTransform().AttachTo(&mRootComponent);

    // Set relative transformations
    mMesh->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    mMesh->SetRotation(glm::vec3(65.0f, 25.0f, 45.0f));
}
```

See full API documentation for ```Actor```, ```StaticMesh```, and ```Scene``` for more details.

---

## Creating a 2D Actor

You can create a 2D actor using:

```bash
python3 Scripts/create_class.py Actor2D MyActor2D
```

This will generate:

```bash
Include/my_actor2d.hpp
Source/my_actor2d.cpp
```

In your ```MyActor2D``` class you can create and attach drawable components derived from ```Drawable2D```, such as ```Rectangle2D```, ```Sprite2D```, or ```TextBlock2D```.

Example:

```cpp
class MyActor2D : public Actor2D
{
    Rectangle2D* mRectangle2D;
    TextBlock2D* mTextBlock2D;
};
```

```cpp
void MyActor2D::Initialize()
{
    Actor2D::Initialize();

    bUpdateOnTick = true;

    Rectangle2D::Properties rectangleProp;
    rectangleProp.size = { 300.0f, 300.0f };
    rectangleProp.color = { 0.0f, 0.0f, 1.0f, 0.4f };

    mRectangle2D = CreateSubobject<Rectangle2D>(rectangleProp);
    mRectangle2D->GetTransform().AttachTo(&mRootComponent);

    TextBlock2D::Properties tbProp;
    tbProp.fontSize = 16;

    mTextBlock2D = CreateSubobject<TextBlock2D>(tbProp);
    mTextBlock2D->GetTransform().AttachTo(&mRootComponent);

    mTextBlock2D->SetText("Text Block");
    mTextBlock2D->SetTextColor({0.0f, 0.0f, 1.0f, 1.0f});

    mTextBlock2D->SetPosition(glm::vec2(-mTextBlock2D->GetDimensions().x, 0.0f));
}
```

See full API documentation for ```Actor2D```, ```Rectangle2D```, ```TextBlock2D```, and ```Scene```.

---

## Creating a Scene (Level)

You can create a Scene (Level) using:

```bash
python3 Scripts/create_class.py Scene Level1
```

This will generate:

```bash
Include/level1.hpp
Source/level1.cpp
```

After creating the scene, register it in:

```bash
Include/level_register.hpp
```

```cpp
REG_SCENE(Level1, "Level1");
```

You can then load the scene using:

```cpp
LoadScene("Level1");
```

Inside your scene class, you can:
- Spawn actors
- Control application logic
- Transition between scenes
- Manage level lifecycle