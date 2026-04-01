#include "scene_manager.hpp"
#include "scene.hpp"
#include "i_renderer.hpp"
#include "i_application.hpp"
#include "texture_cache.hpp"
#include "model_cache.hpp"
#include "material_cache.hpp"
#include "text_renderer.hpp"
#include "utility.hpp"
#include "logger.hpp"

#include <chrono>

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
	const auto start = std::chrono::steady_clock::now();

	// Apply pending registrations once (first initialization only)
	static bool sRegistrationsApplied = false;
	if (!sRegistrationsApplied)
	{
		auto& pending = GetPendingRegistrations();
		for (auto& entry : pending)
		{
			RegisterScene(entry.first, entry.second);
		}
		pending.clear();
		sRegistrationsApplied = true;
	}
	// Innitialize resources in next order.
	// 1. Textures
	// 2. Models
	// 3. Materials
	mTextureCache = std::make_shared<TextureCache>(mRenderer);
	mModelCache = std::make_shared<ModelCache>(mRenderer);
	mModelCache->CreateQuad2D();

	mMaterialCache = std::make_shared<MaterialCache>(mRenderer);
	mMaterialCache->CreateBuildInMaterials();

	mTextRenderer = std::make_shared<TextRenderer>(GetRenderResourceContext());
	mTextRenderer->StoreFontAtlasesInFiles(false);

	if (Utility::IsPackageProvided())
	{
		LOG_INFO("Loading assets from package.");
		mTextureCache->LoadTexturesFromPackage();
		mModelCache->LoadModelsFromPackage();
		mTextRenderer->LoadFontsFromPackage();
	}
	else
	{
		LOG_INFO("Loading assets from folders.");
		const auto textureFolder = Utility::GetTextureFolderPath();
		mTextureCache->LoadTexturesFromFolder(textureFolder.string());
		const auto modelsFolder = Utility::GetModelsFolderPath();
		mModelCache->LoadModelsFromFolder(modelsFolder.string());
		const auto fontsFolder = Utility::GetFontsFolderPath();
		mTextRenderer->LoadFontsFromFolder(fontsFolder.string());
	}

	if(!GetMap().empty())
	{
		const std::string startScene = sStartSceneName;

		if(!startScene.empty())
		{
			LoadScene(startScene);
		}
	}
	const auto end = std::chrono::steady_clock::now();
	const float initMs =
		std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("SceneManager initialized in " +
		std::to_string(initMs) + " ms.");
}

void SceneManager::Update(float deltaTime)
{
	if (mNextScene)
	{
		using clock = std::chrono::steady_clock;
		auto start = clock::now();

		std::string oldSceneName = mCurrentScene ? typeid(*mCurrentScene).name() : "None";
		std::string newSceneName = typeid(*mNextScene).name();

		if (mCurrentScene)
		{
			mCurrentScene->Shutdown();
		}

		mCurrentScene = std::move(mNextScene);

		LOG_INFO("Switching scene: " + oldSceneName + " -> " + newSceneName);

		mCurrentScene->Initialize();
		mNextScene = nullptr;

		auto end = clock::now();
		auto durationMs = std::chrono::duration<float, std::milli>(end - start).count();

		LOG_INFO("Scene '" + newSceneName + "' loaded in " + std::to_string(durationMs) + " ms");
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
	mTextRenderer->Shutdown();
}

IApplication* SceneManager::GetApplication()
{
	return mApp;
}

std::vector<std::pair<std::string, SceneManager::Factory>>& SceneManager::GetPendingRegistrations()
{
	static std::vector<std::pair<std::string, Factory>> pending;
	return pending;
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

std::shared_ptr<TextRenderer> SceneManager::GetTextRenderer()
{
	return mTextRenderer;
}

} // namespace rendering_engine