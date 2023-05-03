#pragma once

#include <Engine/Scripting/Plugins/GamePlugin.h>
#include <Engine/Core/Math/Vector2.h>
#include <Engine/Core/Math/Color.h>
#include <Engine/Core/Log.h>
#include <Engine/Core/Types/DataContainer.h>
#include <Engine/Engine/Engine.h>
#include <Engine/Engine/Screen.h>
#include <Engine/Engine/Time.h>
#include <Engine/Graphics/GPUDevice.h>
#include <Engine/Graphics/RenderTask.h>
#include <Engine/Graphics/Async/GPUTask.h>
#include <Engine/Graphics/Textures/GPUTexture.h>
#include <Engine/Input/Input.h>
#include <Engine/Profiler/Profiler.h>
#include <Engine/Render2D/Render2D.h>
#include <Engine/Scripting/Plugins/PluginManager.h>

#include "SystemInterface.h"
#include "RenderInterface.h"
#include "FileInterface.h"

#define RMLUI_STATIC_LIB
#define RMLUI_NO_THIRDPARTY_CONTAINERS
#define RMLUI_USE_CUSTOM_RTTI

#include "FontEngine/FontEngineInterfaceDefault.h"

#include <RmlUi/Core.h>

/// <summary>
/// RmlUi plugin
/// </summary>
API_CLASS(Namespace="") class RMLUI_API RmlUiPlugin : public GamePlugin
{
    DECLARE_SCRIPTING_TYPE(RmlUiPlugin);

private:
    std::unique_ptr<SystemInterface> systemInterface;
    std::unique_ptr<RenderInterface> renderInterface;
    std::unique_ptr<FileInterface> fileInterface;
    std::unique_ptr<Rml::FontEngineInterfaceDefault> fontInterface;

    Rml::Context* context;

public:
    /// <summary>
    /// Gets the singleton instance of the RmlUi plugin.
    /// </summary>
    API_PROPERTY() static RmlUiPlugin* GetInstance();

private:
    // [GamePlugin]
    void Initialize() override;
    void Deinitialize() override;
    void OnUpdate();
    void OnLateUpdate();
    void OnPostRender(GPUContext* gpuContext, RenderContext& renderContext);
};
