#pragma once

#include <memory>

namespace rendering_engine
{
class RendererBase;
class DrawableComponent;
class SceneComponent;
//This class contains main kinematik staff like position, rotations etc. to be able to live in 3D scene
class Actor
{
public:
	Actor(RendererBase* renderer);
	virtual ~Actor();

	virtual void Initialize();
	virtual void Update(float const delta);
	virtual void Draw();
	virtual void Shudtown();

private:
	Actor(Actor const& rhs);
	Actor& operator=(Actor const& rhs);

protected:
	RendererBase* mRenderer;
};

} //rendering_engine