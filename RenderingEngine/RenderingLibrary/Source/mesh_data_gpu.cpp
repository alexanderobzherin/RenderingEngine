#include "mesh_data_gpu.hpp"
#include "model.hpp"
#include "i_renderer.hpp"
#include "i_mesh_render_resources.hpp"

namespace rendering_engine
{
MeshDataGpu::MeshDataGpu(IRenderer* renderer)
	:
	mRenderer(renderer),
	mGpuHandle(nullptr),
	mSizeOfVerticesBytes(0),
	mSizeOfIndicesBytes(0),
	mMeshType(MeshType::None)
{
}
	MeshDataGpu::MeshDataGpu(const std::string& filename, IRenderer* renderer)
	:
	mRenderer(renderer),
	mGpuHandle(nullptr),
	mSizeOfVerticesBytes(0),
	mSizeOfIndicesBytes(0),
	mMeshType(MeshType::None)
{
	LoadModel(filename);
}

MeshDataGpu::MeshDataGpu(std::vector<uint8_t> const& fileBytes, IRenderer* renderer)
	:
	mRenderer(renderer),
	mGpuHandle(nullptr),
	mSizeOfVerticesBytes(0),
	mSizeOfIndicesBytes(0),
	mMeshType(MeshType::None)
{
	LoadModel(fileBytes);
}

MeshDataGpu::~MeshDataGpu()
{
	ReleaseFromGPU();
}

void MeshDataGpu::UploadToGPU()
{
	if (!mRenderer)
	{
		return;
	}
	if (mGpuHandle)
	{
		if (mGpuHandle->IsOnGPU())
		{
			ReleaseFromGPU();
		}
	}

	mGpuHandle = std::unique_ptr<IMeshRenderResources>(mRenderer->ProvideMeshRenderResources());

	switch (mMeshType)
	{
		case MeshType::None:
		{
			break;
		}
		case MeshType::Sprite2D:
		{	
			mGpuHandle->CreateVertexBuffer(ComposeVertex2DBuffer());
			mGpuHandle->CreateIndexBuffer(mIndices);
			break;
		}
		case MeshType::Billboard:
		{
			mGpuHandle->CreateVertexBuffer(ComposeUnlitBuffer());
			mGpuHandle->CreateIndexBuffer(mIndices);
			break;
		}
		case MeshType::Surface:
		{
			mGpuHandle->CreateVertexBuffer(ComposeLitBuffer());
			mGpuHandle->CreateIndexBuffer(mIndices);
			break;
		}
	}
}

void MeshDataGpu::ReleaseFromGPU()
{
	if (!mRenderer)
	{
		return;
	}
	if (!mGpuHandle)
	{
		return;
	}

	mGpuHandle->Shutdown();
	mGpuHandle.release();
	mGpuHandle = nullptr;
}

bool MeshDataGpu::IsOnGPU() const
{
	bool result = false;

	if (mGpuHandle)
	{
		result = mGpuHandle->IsOnGPU();
	}

	return result;
}

void MeshDataGpu::LoadModel(std::string path)
{
	std::unique_ptr<Model> model = std::make_unique<Model>(path, true);

	if (model)
	{
		if (model->HasMeshes())
		{
			mPositions = model->Meshes()[0]->Vertices();
			
			if (model->Meshes()[0]->VertexColors().size() >= 1)
				mColor = model->Meshes()[0]->VertexColors()[0];

			mNormals = model->Meshes()[0]->Normals();
			for (const auto& texCoord : model->Meshes()[0]->TextureCoordinates()[0])
			{
				mTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
			}
			mTangents = model->Meshes()[0]->Tangents();
			mIndices = model->Meshes()[0]->Indices();
		}
	}

	CalculateMeshParameters();

	mMeshType = MeshType::Surface;
}

void MeshDataGpu::LoadModel(std::vector<uint8_t> const& fileBytes)
{
	std::unique_ptr<Model> model = std::make_unique<Model>(fileBytes, true);

	if (model)
	{
		if (model->HasMeshes())
		{
			mPositions = model->Meshes()[0]->Vertices();

			if (model->Meshes()[0]->VertexColors().size() >= 1)
				mColor = model->Meshes()[0]->VertexColors()[0];

			mNormals = model->Meshes()[0]->Normals();
			for (const auto& texCoord : model->Meshes()[0]->TextureCoordinates()[0])
			{
				mTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
			}
			mTangents = model->Meshes()[0]->Tangents();
			mIndices = model->Meshes()[0]->Indices();
		}
	}

	CalculateMeshParameters();

	mMeshType = MeshType::Surface;
}

void MeshDataGpu::CreateQuad2D()
{
	mPositions2D = { {-0.5f, -0.5f}, { 0.5f, -0.5f}, { 0.5f,  0.5f}, {-0.5f,  0.5f} };
	
	// TOP-LEFT origin UVs
	mTexCoords = {
		{0.0f, 0.0f}, // bottom-left
		{1.0f, 0.0f}, // bottom-right
		{1.0f, 1.0f}, // top-right
		{0.0f, 1.0f}  // top-left
	};

	mColor = { {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	mIndices = {
			0, 1, 2,  // first triangle
			2, 3, 0   // second triangle
	};

	CalculateMeshParameters();

	mMeshType = MeshType::Sprite2D;
}

void MeshDataGpu::LoadCustomMesh(std::vector<glm::vec2> positions2D, std::vector<glm::vec2> texCoords, std::vector<glm::vec4> colors, std::vector<std::uint32_t> indices)
{
	mPositions2D = positions2D;
	mTexCoords = texCoords;
	mColor = colors;
	mIndices = indices;

	CalculateMeshParameters();

	mMeshType = MeshType::Sprite2D;
}

size_t MeshDataGpu::GetGpuVertexBufferSize() const
{
	size_t result = 0;

	if (mGpuHandle)
	{
		result = mGpuHandle->GetVertexBufferSize();
	}

	return result;
}

size_t MeshDataGpu::GetGpuIndexBufferSize() const
{
	size_t result = 0;

	if (mGpuHandle)
	{
		result = mGpuHandle->GetIndexBufferSize();
	}

	return result;
}

size_t MeshDataGpu::GetCpuVertexBufferSize() const
{
	return mSizeOfVerticesBytes;
}

size_t MeshDataGpu::GetCpuIndexBufferSize() const
{
	return mSizeOfIndicesBytes;
}

IMeshRenderResources* MeshDataGpu::GetMeshRenderResources()
{
	if (!IsOnGPU())
	{
		UploadToGPU();
	}
	return mGpuHandle.get();
}

void MeshDataGpu::CalculateMeshParameters()
{
	size_t sizeOfVerticesBytes = 0;
	size_t sizeOfIndicesBytes = 0;

	sizeOfVerticesBytes += (mPositions2D.size() * sizeof(glm::vec2));
	sizeOfVerticesBytes += (mPositions.size() * sizeof(glm::vec3));
	sizeOfVerticesBytes += (mColor.size() * sizeof(glm::vec4));
	sizeOfVerticesBytes += (mNormals.size() * sizeof(glm::vec3));
	sizeOfVerticesBytes += (mTexCoords.size() * sizeof(glm::vec2));
	sizeOfVerticesBytes += (mTangents.size() * sizeof(glm::vec3));
	
	sizeOfIndicesBytes += (mIndices.size() * sizeof(uint32_t));

	mSizeOfVerticesBytes = sizeOfVerticesBytes;
	mSizeOfIndicesBytes = sizeOfIndicesBytes;
}

std::vector<Vertex2D> MeshDataGpu::ComposeVertex2DBuffer()
{
	std::vector<Vertex2D> result;

	unsigned int index = 0;
	for (const auto& position : mPositions2D)
	{
		Vertex2D vert;
		vert.position = position;
		vert.color = mColor.empty() ? glm::vec4(1.0f) : mColor[index];
		vert.textureCoordinates = mTexCoords.empty() ? glm::vec2(0.0f) : mTexCoords[index];

		result.push_back(vert);
		++index;
	}

	return result;
}

std::vector<VertexPositionColorTexture> MeshDataGpu::ComposeUnlitBuffer()
{
	std::vector<VertexPositionColorTexture> result;

	unsigned int index = 0;
	for (const auto& position : mPositions)
	{
		VertexPositionColorTexture vert;
		vert.position = position;
		vert.color = mColor.empty() ? glm::vec4(1) : mColor[index];
		vert.textureCoordinates = mTexCoords.empty() ? glm::vec2(0.0f) : mTexCoords[index];

		result.push_back(vert);
		++index;
	}

	return result;
}

std::vector<VertexPositionColorTextureNormalTangent> MeshDataGpu::ComposeLitBuffer()
{
	std::vector<VertexPositionColorTextureNormalTangent> result;

	unsigned int index = 0;
	for (const auto& position : mPositions)
	{
		VertexPositionColorTextureNormalTangent vert;
		vert.position = position;
		vert.color = mColor.empty() ? glm::vec4(1.0f) : mColor[index];
		vert.textureCoordinates = mTexCoords.empty() ? glm::vec2(0.0f) : mTexCoords[index];
		vert.normal = mNormals.empty() ? glm::vec3(0.0f, 0.0f, 1.0f) : mNormals[index];
		vert.tangent = mTangents.empty() ? glm::vec3(1.0f, 0.0f, 0.0f) : mTangents[index];

		result.push_back(vert);
		++index;
	}

	return result;
}

} // namespace rendering_engine
