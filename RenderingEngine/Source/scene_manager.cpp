#include "scene_manager.hpp"
#include "scene.hpp"
#include "i_renderer.hpp"
#include "i_application.hpp"
#include "texture_cache.hpp"
#include "model_cache.hpp"
#include "material_cache.hpp"
#include "utility.hpp"

namespace rendering_engine
{

SceneManager::SceneManager(IRenderer* renderer, IApplication* app)
	:
	mRenderer(renderer),
	mApp(app),
	mCurrentScene(nullptr),
	mNextScene(nullptr)
{}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	// Innitialize resources in next order.
	// 1. Textures
	// 2. Models
	// 3. Materials
	boost::filesystem::path path;
	const auto textureFolder = path / ".." / "Intermediate" / "Textures";
	mTextureCache = std::make_shared<TextureCache>(mRenderer);
	mTextureCache->LoadTexturesFromFoloder(textureFolder.string());

	const auto modelsFolder = path / ".." / "Intermediate" / "Models";
	mModelCache = std::make_shared<ModelCache>(mRenderer);
	mModelCache->LoadModelsFromFolder(modelsFolder.string());
	mModelCache->CreateQuad2D();

	mMaterialCache = std::make_shared<MaterialCache>(mRenderer);
	mMaterialCache->CreateBuildInMaterials();

	if(!GetMap().empty())
	{
		const std::string startScene = sStartSceneName;

		if(!startScene.empty())
		{
			LoadScene(startScene);
		}
	}
}

void SceneManager::Update(float deltaTime)
{
	if (mNextScene)
	{
		if (mCurrentScene)
		{
			mCurrentScene->Shutdown();
		}

		mCurrentScene = std::move(mNextScene);
		mCurrentScene->Initialize();
		mNextScene = nullptr;
	}

	if (mCurrentScene)
	{
		mCurrentScene->Update(deltaTime);
	}
}

void SceneManager::Draw()
{
	if (mCurrentScene)
	{
		mCurrentScene->Draw();
	}
}

void SceneManager::Shutdown()
{
	if (mCurrentScene)
	{
		mCurrentScene->Shutdown();
	}
	mMaterialCache->ReleaseAll();
	mModelCache->ReleaseAll();
	mTextureCache->ReleaseAll();
}

IApplication* SceneManager::GetApplication()
{
	return mApp;
}

void SceneManager::LoadScene(std::string sceneName)
{
	mNextScene = CreateScene(sceneName, *this);
}

RenderResourceContext SceneManager::GetRenderResourceContext() const
{
	RenderResourceContext rrc;
	rrc.renderer = mRenderer;
	rrc.meshCache = mModelCache.get();
	rrc.textureCache = mTextureCache.get();
	rrc.materialCache = mMaterialCache.get();

	return rrc;
}

bool SceneManager::RegisterScene(const std::string& name, Factory factory)
{
	auto& map = GetMap();
	return map.emplace(name, std::move(factory)).second;
}

std::unique_ptr<Scene> SceneManager::CreateScene(const std::string& name, SceneManager& sm)
{
	std::unique_ptr<Scene> scene = nullptr;
	auto& map = GetMap();
	auto it = map.find(name);
	if (it != map.end())
	{
		scene = (it->second)(sm);
	}
	return scene;
}

} // namespace rendering_engine