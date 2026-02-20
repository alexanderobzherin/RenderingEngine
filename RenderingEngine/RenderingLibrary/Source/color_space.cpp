#include "color_space.hpp"

namespace rendering_engine
{

float ColorSpace::SRGBToLinear(float c)
{
	if (c <= 0.04045f)
		return c / 12.92f;
	else
		return pow((c + 0.055f) / 1.055f, 2.4f);
}

glm::vec3 ColorSpace::SRGBToLinear(const glm::vec3& color)
{
	return glm::vec3(
		SRGBToLinear(color.r),
		SRGBToLinear(color.g),
		SRGBToLinear(color.b)
	);
}

float ColorSpace::LinearToSRGB(float c)
{
	if (c <= 0.0031308f)
		return 12.92f * c;
	else
		return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

glm::vec3 ColorSpace::LinearToSRGB(const glm::vec3& color)
{
	return glm::vec3(
		LinearToSRGB(color.r),
		LinearToSRGB(color.g),
		LinearToSRGB(color.b)
	);
}

} // namespace rendering_engine
