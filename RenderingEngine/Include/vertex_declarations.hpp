// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <vector>
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace rendering_engine
{
/**
 * @brief Contains model, view, and projection matrices for 3D rendering.
 */
struct Transformations3D
{
    glm::mat4 model; ///< Model transformation matrix
    glm::mat4 view;  ///< View (camera) transformation matrix
    glm::mat4 proj;  ///< Projection transformation matrix
};

/**
 * @brief Contains transformation matrices for 2D rendering.
 */
struct Transformations2D
{
    glm::mat4 model; ///< Model transformation matrix
    glm::mat4 view;  ///< View (camera) transformation matrix
    glm::mat4 proj;  ///< Projection transformation matrix
};

/**
 * @brief Vertex format for 2D UI/Overlay elements.
 *
 * Used for sprites, UI widgets, etc.
 */
struct Vertex2D
{
    glm::vec2 position;           ///< 2D position in screen or world space
    glm::vec4 color;              ///< Vertex color (RGBA)
    glm::vec2 textureCoordinates; ///< UV coordinates for texturing

    /// Default constructor.
    Vertex2D() = default;

    /**
     * @brief Construct with position, color, and texture coordinates.
     * @param pos 2D position
     * @param col RGBA color
     * @param uv  Texture coordinates
     */
    Vertex2D(const glm::vec2& pos, 
             const glm::vec4& col, 
             const glm::vec2& uv)
        : 
        position(pos), 
        color(col), 
        textureCoordinates(uv) 
    {}
};

/**
 * @brief Vertex format for unlit 3D geometry.
 *
 * Used for 3D models/materials that don't require lighting calculations,
 * such as particle systems, billboards, or flat-shaded meshes.
 */
    struct VertexPositionColorTexture
    {
        glm::vec3 position;           ///< 3D position
        glm::vec4 color;              ///< Vertex color (RGBA)
        glm::vec2 textureCoordinates; ///< UV coordinates for texturing

        VertexPositionColorTexture() = default;

        /**
         * @brief Construct with position, UV, and color.
         * @param pos 3D position
         * @param col RGBA color
         * @param uv  Texture coordinates
         */
        VertexPositionColorTexture(const glm::vec3& pos,
                                   const glm::vec4& col,
                                   const glm::vec2& uv
                                   )
            : 
            position(pos), 
            color(col),
            textureCoordinates(uv)
        {}
    };

/**
 * @brief Vertex format for lit 3D geometry (normal and tangent support).
 *
 * Used for 3D meshes with normal mapping or lighting (Phong, Blinn-Phong, PBR, etc.).
 */
struct VertexPositionColorTextureNormalTangent
{
    glm::vec3 position;           ///< 3D position
    glm::vec4 color;              ///< Vertex color (RGBA)
    glm::vec2 textureCoordinates; ///< UV coordinates for texturing
    glm::vec3 normal;             ///< Surface normal (for lighting)
    glm::vec3 tangent;            ///< Tangent (for normal mapping)

    VertexPositionColorTextureNormalTangent() = default;

    /**
     * @brief Construct with all vertex attributes.
     * @param pos 3D position
     * @param col RGBA color
     * @param uv  Texture coordinates
     * @param nrm Normal vector
     * @param tan Tangent vector
     */
    VertexPositionColorTextureNormalTangent(const glm::vec3& pos,
                                            const glm::vec4& col,
                                            const glm::vec2& uv,
                                            const glm::vec3& nrm,
                                            const glm::vec3& tan)
        : 
        position(pos), 
        color(col), 
        textureCoordinates(uv), 
        normal(nrm), 
        tangent(tan) 
    {}
};

/**
 * @brief Vertex format for skinned meshes (skeletal animation).
 *
 * Supports up to 4 bones per vertex. Used for character models and animated geometry.
 */
struct VertexSkinnedPositionTextureNormalTangent 
{
    glm::vec3 position;      ///< 3D position
    glm::vec2 textureCoordinates; ///< UV coordinates for texturing
    glm::vec3 normal;        ///< Surface normal (for lighting)
    glm::vec3 tangent;       ///< Tangent (for normal mapping)
    glm::uvec4 boneIndices;  ///< Indices of influencing bones (up to 4)
    glm::vec4 boneWeights;   ///< Blend weights for each bone (should sum to 1.0)

    VertexSkinnedPositionTextureNormalTangent() = default;

    /**
     * @brief Construct with all vertex attributes and bone data.
     * @param pos 3D position
     * @param uv  Texture coordinates
     * @param nrm Normal vector
     * @param tan Tangent vector
     * @param indices Indices of influencing bones
     * @param weights Blend weights for each bone
     */
    VertexSkinnedPositionTextureNormalTangent(const glm::vec3& pos,
        const glm::vec2& uv,
        const glm::vec3& nrm,
        const glm::vec3& tan,
        const glm::uvec4& indices,
        const glm::vec4& weights)
        : 
        position(pos), 
        textureCoordinates(uv), 
        normal(nrm), 
        tangent(tan),
        boneIndices(indices), 
        boneWeights(weights) 
    {}
};


}
