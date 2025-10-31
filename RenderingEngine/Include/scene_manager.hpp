// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "render_resource_context.hpp"

#include <memory>
#include <functional>

namespace rendering_engine
{
class IRenderer;
class IApplication;
class Scene;
class TextureCache;
class ModelCache;
class MaterialCache;

struct RenderResourceContext;

class SceneManager
{
public:
	SceneManager(IRenderer* renderer, IApplication* app);
	~SceneManager();

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Shutdown();

	IApplication* GetApplication();

	void LoadScene(std::string sceneName);

	RenderResourceContext GetRenderResourceContext() const;

	using Factory = std::function<std::unique_ptr<Scene>(SceneManager&)>;

	static bool RegisterScene(const std::string& name, Factory factory);

	static std::unique_ptr<Scene> CreateScene(const std::string& name, SceneManager& sm);


private:

	static std::unordered_map<std::string, Factory>& GetMap()
	{
		static std::unordered_map<std::string, Factory> map;
		return map;
	}
public:
	inline static std::string sStartSceneName;

private:
	IRenderer* mRenderer;
	IApplication* mApp;

	std::unique_ptr<Scene> mCurrentScene;
	std::unique_ptr<Scene> mNextScene;

	std::shared_ptr<TextureCache> mTextureCache;
	std::shared_ptr<ModelCache> mModelCache;
	std::shared_ptr<MaterialCache> mMaterialCache;
};

template <class TScene>
struct SceneAutoRegistrar
{
explicit SceneAutoRegistrar(const char* name)
{
	SceneManager::RegisterScene(name, [](SceneManager& sm) {
		return std::make_unique<TScene>(sm);
		});
}
};

#define REG_SCENE(Type, NameStr) \
    static rendering_engine::SceneAutoRegistrar<Type> \
        _auto_registrar_##Type##__{NameStr}


struct StartSceneAutoSetter
{
	explicit StartSceneAutoSetter(const char* n) { SceneManager::sStartSceneName = n; }
};

#define RE_CAT_INNER(a,b) a##b
#define RE_CAT(a,b) RE_CAT_INNER(a,b)

#define START_SCENE(NameStr) \
  static StartSceneAutoSetter RE_CAT(_start_scene_, __COUNTER__){NameStr}

} // namespace rendering_engine