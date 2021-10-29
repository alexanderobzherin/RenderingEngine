#include "rendering_application.hpp"

namespace rendering_engine
{

RenderingApplication::RenderingApplication( int const width, int const height )
    :
    mWidth( width ),
    mHeight( height )
{
    InitializeWindow();
}
RenderingApplication::~RenderingApplication()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void RenderingApplication::Run()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while(!glfwWindowShouldClose(mWindow)) 
    {
        glfwPollEvents();
    }
}

void RenderingApplication::InitializeWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(mWidth, mHeight, "Vulkan window", nullptr, nullptr);

}

}