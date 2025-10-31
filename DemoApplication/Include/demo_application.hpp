/* Old architecture
#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "boost/filesystem.hpp"
#include "vulkan_renderer.hpp"
#include "utility.hpp"
#include "vulkan_drawable_component.hpp"
#include "scene_component.hpp"

using namespace rendering_engine;
using namespace vulkan_renderer;

namespace demo_application
{
class DemoApplication : public VulkanRenderer
{
public:
	DemoApplication(char const* appName);
	DemoApplication(int const width, int const height, char const* appName);
	~DemoApplication() override;

protected:
	void Shutdown() override;

	void Initialize() override;
	void Update(float const delta) override;
};


} // demo_application
*/ // Old architecture

/*

#include "core_application.hpp"

using namespace rendering_engine;

namespace demo_application
{
class DemoApplication : public CoreApplication
{
public:
	DemoApplication(char const* appName);
	DemoApplication(int const width, int const height, char const* appName);

	void Initialize() override;
	virtual void Run() override;
	virtual void Shutdown() override;
};
}


*/