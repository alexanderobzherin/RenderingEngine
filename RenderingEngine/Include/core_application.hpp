#pragma once

#include "i_application.hpp"
#include "i_window_system.hpp"
#include "i_renderer.hpp"

#include <memory>
#include <vector>

namespace rendering_engine
{
class AppTime;
class SceneManager;

class TestTriangle;
class TestSprite2D;

class CoreApplication : public IApplication
{
public:
	CoreApplication(char const* appName);
	CoreApplication(unsigned int width, unsigned int height, char const* appName);
	CoreApplication(unsigned int width, 
					unsigned int height, 
					char const* appName,
					std::shared_ptr<IWindowSystem> windowSystem,
					std::shared_ptr<IRenderer> renderer);
	CoreApplication(char const* appName,
					std::shared_ptr<IWindowSystem> windowSystem,
					std::shared_ptr<IRenderer> renderer);


	void Initialize() override;

	void Run() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Shutdown() override;

	ScreenSettings GetScreenSettings() const override;

protected:
	bool bIsFullScreen;
	unsigned int mWidth;
	unsigned int mHeight;
	char const* mAppName;
	
	std::shared_ptr<AppTime> mAppTime;
	std::shared_ptr<IWindowSystem> mWindowSystem;
	std::shared_ptr<IRenderer> mRenderer;

	std::shared_ptr<SceneManager> mSceneManager;

	std::shared_ptr<TestTriangle> mTestTriangle;
	std::shared_ptr<TestSprite2D> mTestSprite2D;
	
};
}