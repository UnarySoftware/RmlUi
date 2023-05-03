#pragma once

#include <Engine/Content/Content.h>
#include <Engine/Content/Assets/Texture.h>
#include <Engine/Content/AssetReference.h>
#include <Engine/Graphics/Textures/GPUTexture.h>
#include <Engine/Core/Types/BaseTypes.h>
#include <Engine/Core/Math/Vector2.h>
#include <Engine/Core/Types/CommonValue.h>
#include <Engine/Core/Collections/Dictionary.h>
#include <Engine/Render2D/Render2D.h>
#include <Engine/Graphics/RenderTask.h>
#include <Engine/Graphics/GPUDevice.h>
#include <Engine/Graphics/Async/GPUTask.h>

#define RMLUI_STATIC_LIB
#define RMLUI_NO_THIRDPARTY_CONTAINERS
#define RMLUI_USE_CUSTOM_RTTI

#undef RadiansToDegrees
#undef DegreesToRadians

#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/StringUtilities.h>

// Converts radians to degrees.
#define RadiansToDegrees (180.0f / PI)
// Converts degrees to radians.
#define DegreesToRadians (PI / 180.0f)

class RenderInterface : public Rml::RenderInterface
{
private:

    struct Geometry
    {
        Span<Float2> original_vertices;
        Span<Float2> transformed_vertices;
        Span<Color> colors;
        Span<Float2> uvs;
        Span<uint16> indices;
        Rml::TextureHandle texture;
    };

    Array<Float2> tempOrigVertices;
    Array<Float2> tempTransVertices;
    Array<Color> tempColors;
    Array<Float2> tempUvs;
    Array<uint16> tempIndices;

    uint64 handleCounter = 0;

    GPUTexture* defaultTexture;

    Dictionary<uintptr_t, Geometry> dynamic;
    Dictionary<uintptr_t, Geometry> compiled;
    Array<uintptr_t> render;
    Dictionary<uintptr_t, GPUTexture*> textures;
    Dictionary<AssetReference<Texture>, uintptr> asyncLoad;
    bool useScissors = false;
    Rectangle scissors;
    Matrix globalTransform;

public:
    RenderInterface();
    virtual ~RenderInterface();
    void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture) override;
    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;
    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(int x, int y, int width, int height) override;
    void OnTextureLoaded(Asset* texture);
    bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const std::string& source) override;
    bool GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;
    void SetTransform(const Rml::Matrix4f* transform) override;

    void OnPostRender(GPUContext* context, RenderContext& renderContext);
};
