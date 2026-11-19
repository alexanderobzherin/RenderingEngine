#include "rectangle_2d.hpp"
#include "camera_2d.hpp"
#include "scene.hpp"
#include "physical_geometry_factory_2d.hpp"

namespace rendering_engine
{
Rectangle2D::Rectangle2D(RenderResourceContext renderContext, Scene& scene, Properties properties)
	:
	Drawable2D(renderContext, scene),
	mProperties(properties)
{}

void Rectangle2D::Initialize()
{
	const std::string meshName = "Quad2D";
	const std::string materialName = "Rectangle2D";

	AddRenderBatch(meshName, materialName);

	Drawable2D::Initialize();

	SetColor(mProperties.color);

	InitializePhysicalGeometry();
}

void Rectangle2D::Update(float deltaTime)
{
	Drawable2D::Update(deltaTime);
}

void Rectangle2D::Draw(const Camera2D& camera)
{
	Transformations2D transformations;
	transformations.model = GetTransform().GetWorldMatrix();
	transformations.view = camera.GetWorldView();
	transformations.proj = camera.GetProjectionMatrix();

	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
	}
}

void Rectangle2D::SetColor(glm::vec4 color)
{
	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.materialParameters.SetMaterialVec4("Color", color);
	}
}

void Rectangle2D::InitializePhysicalGeometry()
{
	SetScale(mProperties.size);

	// The drawable scales a unit quad to the requested size.
	// Therefore, physical geometry also uses unit dimensions
	// and shares the same world transform.
	auto geometry = PhysicalGeometryFactory2D::Create(
		GetTransform(),
		mProperties.physical,
		{ 1.0f, 1.0f });

	SetPhysicalGeometry(std::move(geometry));
}


} // namespace rendering_engine