// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "render_resource_context.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace rendering_engine
{
class IRenderer;
class IApplication;
class Scene;
class TextureCache;
class ModelCache;
class MaterialCache;
class TextRenderer;

struct RenderResourceContext;

/**
 * @class SceneManager
 * @brief Manages scenes, resource caches, and scene transitions within the rendering engine.
 *
 * SceneManager handles the creation, updating, and rendering of scenes.
 * It maintains ownership of core caches (TextureCache, ModelCache, MaterialCache),
 * and provides the RenderResourceContext that allows drawable objects
 * to access shared rendering resources.
 *
 * The class also supports dynamic scene registration through factories,
 * enabling runtime creation of scenes by name.
 *
 * @see Scene, RenderResourceContext, TextureCache, ModelCache, MaterialCache
 */
class RE_API SceneManager
{
public:
	using Factory = std::function<std::unique_ptr<Scene>(SceneManager&)>;
	/**
	 * @brief Constructs a SceneManager instance.
	 * @param renderer Pointer to the active renderer instance.
	 * @param app Pointer to the owning application instance.
	 */
	SceneManager(IRenderer* renderer, IApplication* app);
	/**
	 * @brief Destructor. Cleans up resources and active scenes.
	 */
	~SceneManager();
	/**
	 * @brief Initializes the current scene and related caches.
	 */
	virtual void Initialize();
	/**
	 * @brief Updates the currently active scene.
	 * @param deltaTime Time elapsed since the previous frame, in milliseconds.
	 */
	virtual void Update(float deltaTime);
	/**
	 * @brief Draws the active scene using the associated renderer.
	 */
	virtual void Draw();
	/**
	 * @brief Releases all scene-related resources and shuts down the manager.
	 */
	virtual void Shutdown();
	/**
	 * @brief Returns the application associated with this SceneManager.
	 * @return Pointer to the IApplication instance.
	 */
	IApplication* GetApplication();

	/**
	 * @brief Type alias for the factory used to create scenes.
	 */

	static std::vector<std::pair<std::string, Factory>>& GetPendingRegistrations();

	/**
	 * @brief Loads a new scene by name.
	 * @param sceneName The name of the registered scene to load.
	 *
	 * If the scene has been registered via SceneAutoRegistrar, it will be instantiated
	 * and prepared for initialization on the next update.
	 */
	void LoadScene(std::string sceneName);
	/**
	 * @brief Retrieves the current RenderResourceContext.
	 *
	 * The RenderResourceContext provides references to shared caches (material, model, texture)
	 * and allows drawable objects to access resources during initialization and rendering.
	 *
	 * @return A RenderResourceContext structure populated with shared cache pointers.
	 */
	RenderResourceContext GetRenderResourceContext() const;
	/**
	 * @brief Registers a scene type with a name string.
	 * @param name Unique string identifier for the scene.
	 * @param factory Function that constructs the scene when requested.
	 * @return True if registration succeeded, false if a scene with the same name already exists.
	 *
	 * @note Typically used via the REG_SCENE() macro.
	 */
	static bool RegisterScene(const std::string& name, Factory factory);
	/**
	 * @brief Creates a scene instance by its registered name.
	 * @param name The name of the registered scene type.
	 * @param sm Reference to the SceneManager creating the scene.
	 * @return Unique pointer to the newly created Scene instance.
	 */
	static std::unique_ptr<Scene> CreateScene(const std::string& name, SceneManager& sm);

	std::shared_ptr<TextRenderer> GetTextRenderer();

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
	std::shared_ptr<TextRenderer> mTextRenderer;
};
/**
 * @brief Template-based auto-registrar for scenes.
 *
 * Registers a scene type automatically at static initialization time.
 * Used internally by the REG_SCENE() macro.
 */
template <class TScene>
struct SceneAutoRegistrar
{
	explicit SceneAutoRegistrar(const char* name)
	{
		auto& list = SceneManager::GetPendingRegistrations();
		list.emplace_back(
			std::string{ name },
			[](SceneManager& sm) {
				return std::make_unique<TScene>(sm);
			});
	}
};
/**
 * @brief Convenience macro for registering a scene type with a name.
 * @param Type Class type of the scene.
 * @param NameStr Name string used for registration.
 */
#define REG_SCENE(Type, NameStr) \
    static rendering_engine::SceneAutoRegistrar<Type> \
        _auto_registrar_##Type##__{NameStr}

 /**
  * @brief Helper struct for automatically setting the start scene name.
  */
struct StartSceneAutoSetter
{
	explicit StartSceneAutoSetter(const char* n) { SceneManager::sStartSceneName = n; }
};
/**
 * @brief Helper macros for concatenating tokens during static registration.
 */
#define RE_CAT_INNER(a,b) a##b
#define RE_CAT(a,b) RE_CAT_INNER(a,b)
 /**
  * @brief Macro for defining the application's start scene.
  * @param NameStr Name of the scene to start with.
  *
  * Example:
  * @code
  * START_SCENE("MainMenu");
  * @endcode
  */
#define START_SCENE(NameStr) \
  static StartSceneAutoSetter RE_CAT(_start_scene_, __COUNTER__){NameStr}

} // namespace rendering_engine