#include "RenderInterface.h"

RenderInterface::RenderInterface()
{
    defaultTexture = GPUDevice::Instance->CreateTexture(TEXT("RmlUi.DefaultTexture"));
    if (defaultTexture->Init(GPUTextureDescription::New2D(1, 1, 1, PixelFormat::R8G8B8A8_UNorm, GPUTextureFlags::ShaderResource)))
    {
        return;
    }
    //byte pixels[4] = { 255 };
    //BytesContainer data(pixels, 4);
    //auto uploadTask = defaultTexture->UploadMipMapAsync(data, 0, true);
    //if (uploadTask)
    //{
    //    uploadTask->Start();
    //    uploadTask->Wait();
    //}
}

RenderInterface::~RenderInterface()
{
    SAFE_DELETE_GPU_RESOURCE(defaultTexture);

    Array<uintptr_t> keys;
    textures.GetKeys(keys);
    for (const auto& handle : keys)
    {
        ReleaseTexture(handle);
    }
}

void RenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
    tempOrigVertices.Clear();
    tempTransVertices.Clear();
    tempColors.Clear();
    tempUvs.Clear();
    tempIndices.Clear();

    for (int i = 0; i < num_vertices; ++i)
    {
        Rml::Vertex vertex = vertices[i];
        tempOrigVertices.Push(Float2({ vertex.position.x, vertex.position.y }));
        tempTransVertices.Push(Float2({ vertex.position.x + translation.x, vertex.position.y + translation.y }));
        tempColors.Push(Color::FromBytes(vertex.colour.red, vertex.colour.green, vertex.colour.blue, vertex.colour.alpha));
        tempUvs.Push(Float2({ vertex.tex_coord.x, vertex.tex_coord.y }));
    }

    for (int i = 0; i < num_indices; ++i)
    {
        int indicy = indices[i];
        tempIndices.Push(indicy);
    }

    Geometry newDynamicGeometry;
    newDynamicGeometry.original_vertices = { tempOrigVertices.Get(), tempOrigVertices.Count() };
    newDynamicGeometry.transformed_vertices = { tempTransVertices.Get(), tempTransVertices.Count() };
    newDynamicGeometry.colors = { tempColors.Get(), tempColors.Count() };
    newDynamicGeometry.uvs = { tempUvs.Get(), tempUvs.Count() };
    newDynamicGeometry.indices = { tempIndices.Get(), tempIndices.Count() };
    newDynamicGeometry.texture = texture;

    handleCounter++;
    dynamic[handleCounter] = newDynamicGeometry;
    render.Push(handleCounter);
}

Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture)
{
    tempOrigVertices.Clear();
    tempTransVertices.Clear();
    tempColors.Clear();
    tempUvs.Clear();
    tempIndices.Clear();

    for (int i = 0; i < num_vertices; ++i)
    {
        Rml::Vertex vertex = vertices[i];
        tempOrigVertices.Push(Float2({ vertex.position.x, vertex.position.y }));
        tempTransVertices.Push(Float2({ vertex.position.x, vertex.position.y }));
        tempColors.Push(Color::FromBytes(vertex.colour.red, vertex.colour.green, vertex.colour.blue, vertex.colour.alpha));
        tempUvs.Push(Float2({ vertex.tex_coord.x, vertex.tex_coord.y }));
    }

    for (int i = 0; i < num_indices; ++i)
    {
        int indicy = indices[i];
        tempIndices.Push(indicy);
    }

    Geometry newCompiledGeometry;
    newCompiledGeometry.original_vertices = { tempOrigVertices.Get(), tempOrigVertices.Count() };
    newCompiledGeometry.transformed_vertices = { tempTransVertices.Get(), tempTransVertices.Count() };
    newCompiledGeometry.colors = { tempColors.Get(), tempColors.Count() };
    newCompiledGeometry.uvs = { tempUvs.Get(), tempUvs.Count() };
    newCompiledGeometry.indices = { tempIndices.Get(), tempIndices.Count() };
    newCompiledGeometry.texture = texture;

    handleCounter++;
    compiled[handleCounter] = newCompiledGeometry;
    return handleCounter;
}

void RenderInterface::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
{
    for (int i = 0; i < compiled[geometry].original_vertices.Length(); ++i)
    {
        Float2& original = compiled[geometry].original_vertices[i];
        Float2& target = compiled[geometry].transformed_vertices[i];
        target.X = original.X + translation.x;
        target.Y = original.Y + translation.y;
    }

    render.Push(geometry);
}

void RenderInterface::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
{
    compiled.Remove(geometry);
}

void RenderInterface::EnableScissorRegion(bool enable)
{
    useScissors = enable;
}

void RenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
    scissors = Rectangle(x, y, width, height);
}

void RenderInterface::OnTextureLoaded(Asset* texture)
{
    AssetReference<Texture> target = static_cast<Texture*>(texture);
    if (!asyncLoad.ContainsKey(target))
    {
        return;
    }

    BytesContainer data;
    target->GetMipData(0, data);

    uintptr_t handle = asyncLoad[target];

    auto uploadTask = textures[handle]->UploadMipMapAsync(data, 0, true);
    if (uploadTask)
    {
        uploadTask->Start();
    }

    asyncLoad.Remove(target);
}

bool RenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const std::string& source)
{
    int bufferSize;
    String resultPath = StringUtils::ConvertUTF82UTF16(source.data(), source.size(), bufferSize);

    AssetInfo info;
    if (!Content::GetAssetInfo(resultPath, info) || info.TypeName != TEXT("Texture"))
    {
        return false;
    }

    GPUTexture* newTexture = GPUDevice::Instance->CreateTexture();
    if (newTexture->Init(GPUTextureDescription::New2D(texture_dimensions.x, texture_dimensions.y, 1, PixelFormat::R8G8B8A8_UNorm, GPUTextureFlags::ShaderResource)))
    {
        return false;
    }

    handleCounter++;
    textures[handleCounter] = newTexture;

    AssetReference<Texture> target = Content::LoadAsync<Texture>(info.ID);
    target->OnLoaded.Bind<RenderInterface, &RenderInterface::OnTextureLoaded>(this);

    asyncLoad[target] = handleCounter;

    return true;
}

bool RenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const byte* source, const Rml::Vector2i& source_dimensions)
{
    GPUTexture* newTexture = GPUDevice::Instance->CreateTexture();
    if (newTexture->Init(GPUTextureDescription::New2D(source_dimensions.x, source_dimensions.y, 1, PixelFormat::R8G8B8A8_UNorm, GPUTextureFlags::ShaderResource)))
    {
        return false;
    }

    handleCounter++;
    textures[handleCounter] = newTexture;

    BytesContainer data;
    data.Link(source, source_dimensions.x * source_dimensions.y * 4);
    auto uploadTask = newTexture->UploadMipMapAsync(data, 0, true);
    if (uploadTask)
    {
        uploadTask->Start();
    }
    return true;
}

void RenderInterface::ReleaseTexture(Rml::TextureHandle texture)
{
    if (!textures.ContainsKey(texture))
    {
        return;
    }
    SAFE_DELETE_GPU_RESOURCE(textures[texture]);
    textures.Remove(texture);
}

void RenderInterface::SetTransform(const Rml::Matrix4f* transform)
{
    float values[16];
    Platform::MemoryCopy(&values, transform->data(), sizeof(float) * 16);
    globalTransform = Matrix(values);
}

void RenderInterface::OnPostRender(GPUContext* context, RenderContext& renderContext)
{
    const Viewport viewport = renderContext.Task->GetOutputViewport();
    Render2D::Begin(context, renderContext.Task->GetOutputView(), nullptr, viewport);

    for (int i = 0; i < render.Count(); ++i)
    {
        uintptr_t renderTarget = render[i];
        if (useScissors)
        {
            
        }

        //Render2D::PushClip(scissors);

        //Render2D::FillRectangle(Rectangle(10.0f, 10.0f, 20.0f, 20.0f), Color::White);

        Render2D::DrawTexture(defaultTexture, Rectangle(10.0f, 10.0f, 20.0f, 20.0f), Color::White);

        Geometry targetGeometry;

        if (dynamic.ContainsKey(renderTarget))
        {
            targetGeometry = dynamic[renderTarget];
        }
        else
        {
            targetGeometry = compiled[renderTarget];
        }

        if (targetGeometry.texture == 0)
        {
            Render2D::DrawTexturedTriangles(defaultTexture, targetGeometry.indices, targetGeometry.transformed_vertices, targetGeometry.uvs, targetGeometry.colors);
        }
        else
        {
            Render2D::DrawTexturedTriangles(textures[targetGeometry.texture], targetGeometry.indices, targetGeometry.transformed_vertices, targetGeometry.uvs, targetGeometry.colors);
        }

        //Render2D::PopClip();

        if (useScissors)
        {
            
        }
    }

    useScissors = false;

    render.Clear();

    Render2D::End();
}
