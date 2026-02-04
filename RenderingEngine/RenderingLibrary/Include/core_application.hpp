// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "rendering_engine_export.hpp"
#include "i_application.hpp"
#include "i_window_system.hpp"
#include "i_renderer.hpp"

#include <memory>
#include <vector>

namespace rendering_engine
{
class AppTime;
class SceneManager;

/**
 * @class CoreApplication
 * @brief Default implementation of the IApplication interface.
 *
 * Coordinates the main execution flow of a rendering-based application,
 * including initialization, update, rendering, and shutdown. It acts as
 * the central manager that connects the renderer, window system, and
 * scene management subsystems.
 *
 * @see IApplication
 * @see IWindowSystem
 * @see IRenderer
 */
class RE_API CoreApplication : public IApplication
{
public:
	/**
	 * @brief Constructs a CoreApplication using settings from the config file.
	 *
	 * Loads AppConfig and applies its values to window setup.
	 */
	CoreApplication();
	/**
	 * @brief Constructs a CoreApplication with full-screen window size.
	 * @param appName Name of the application, used for window caption and logging.
	 */
	CoreApplication(char const* appName);
	/**
	 * @brief Constructs a CoreApplication with explicit window size.
	 * @param width    Window width in pixels.
	 * @param height   Window height in pixels.
	 * @param appName  Name of the application.
	 */
	CoreApplication(unsigned int width, unsigned int height, char const* appName);
	/**
	 * @brief Constructs a CoreApplication with injected subsystems.
	 * @param width         Window width in pixels.
	 * @param height        Window height in pixels.
	 * @param appName       Name of the application.
	 * @param windowSystem  Shared pointer to a custom window system implementation.
	 * @param renderer      Shared pointer to a custom renderer implementation.
	 */
	CoreApplication(unsigned int width, 
					unsigned int height, 
					char const* appName,
					std::shared_ptr<IWindowSystem> windowSystem,
					std::shared_ptr<IRenderer> renderer);

					
	/**
	 * @brief Constructs a CoreApplication with custom systems but default window size.
	 * @param appName       Name of the application.
	 * @param windowSystem  Shared pointer to a custom window system implementation.
	 * @param renderer      Shared pointer to a custom renderer implementation.
	 */
	CoreApplication(char const* appName,
					std::shared_ptr<IWindowSystem> windowSystem,
					std::shared_ptr<IRenderer> renderer);
	virtual ~CoreApplication();
						
	/** @copydoc IApplication::Initialize */
	void Initialize() override;
	/** @copydoc IApplication::Run */
	void Run() override;
	/** @copydoc IApplication::Update */
	void Update(float deltaTime) override;
	/** @copydoc IApplication::Draw */
	void Draw() override;
	/** @copydoc IApplication::Shutdown */
	void Shutdown() override;
	/** @copydoc IApplication::GetScreenSettings */
	ScreenSettings GetScreenSettings() const override;

protected:
	bool bIsFullScreen;
	unsigned int mWidth;
	unsigned int mHeight;
	std::string mAppName;
	
	std::shared_ptr<AppTime> mAppTime;
	std::shared_ptr<IWindowSystem> mWindowSystem;
	std::shared_ptr<IRenderer> mRenderer;

	std::shared_ptr<SceneManager> mSceneManager;
};
}