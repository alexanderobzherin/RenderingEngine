#pragma once

namespace rendering_engine
{
class IRenderResources;
class ITextureRenderResources;
class IMaterialRenderResources;
class IMeshRenderResources;
class IRenderResources;
class IRendererObserver;

class IRenderer 
{
public:
    virtual void InitializeRenderer() = 0;
    virtual void DrawFrame() = 0;

    virtual bool BeginFrame() = 0;
    virtual void BeginRenderPass() = 0;
    virtual void EndRenderPass() = 0;
    virtual void EndFrame() = 0;

    virtual void WaitIdle() = 0;
    virtual void ShutdownRenderer() = 0;

    virtual void RegisterObserver(IRendererObserver* notifier) = 0;
    virtual void UnregisterObserver(IRendererObserver* notifier) = 0;

    virtual IRenderResources* ProvideRenderResources() const = 0;
    virtual ITextureRenderResources* ProvideTextureRenderResources() const = 0;
    virtual IMaterialRenderResources* ProvideMaterialRenderResources() const = 0;
    virtual IMeshRenderResources* ProvideMeshRenderResources() const = 0;

    virtual ~IRenderer() = default;
};

} //rendering_engine