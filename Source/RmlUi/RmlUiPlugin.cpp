#include "RmlUiPlugin.h"

RmlUiPlugin::RmlUiPlugin(const SpawnParams& params)
    : GamePlugin(params)
{
    _description.Category = TEXT("Debug");
#if USE_EDITOR
    _description.Description = TEXT("RmlUi plugin");
    _description.Author = TEXT("The RmlUi Team");
    _description.Version = Version(5, 1);
    _description.HomepageUrl = TEXT("https://github.com/mikke89/RmlUi");
    _description.RepositoryUrl = TEXT("https://github.com/UnarySoftware/RmlUi");
#endif
}

RmlUiPlugin* RmlUiPlugin::GetInstance()
{
    return PluginManager::GetPlugin<RmlUiPlugin>();
}

void RmlUiPlugin::Initialize()
{
    GamePlugin::Initialize();

    systemInterface = std::make_unique<SystemInterface>();
    Rml::SetSystemInterface(systemInterface.get());
    renderInterface = std::make_unique<RenderInterface>();
    Rml::SetRenderInterface(renderInterface.get());
    fileInterface = std::make_unique<FileInterface>();
    Rml::SetFileInterface(fileInterface.get());
    fontInterface = std::make_unique<Rml::FontEngineInterfaceDefault>();
    Rml::SetFontEngineInterface(fontInterface.get());

    Rml::Initialise();

    Float2 screenSize = Screen::GetSize();
    context = Rml::CreateContext("main", Rml::Vector2i(screenSize.X, screenSize.Y));

    if (!context)
    {
        LOG(Error, "[RmlUi] Failed to create context");
        Rml::Shutdown();
        return;
    }

    //Rml::Debugger::Initialise(context);

    Rml::LoadFontFace("Content/UI/NewCinemaAStd-D.flax");

    Rml::ElementDocument* document = context->LoadDocument("Content/UI/Test.flax");
    if (!document)
    {
        LOG(Error, "[RmlUi] Failed to load Test.flax");
        Rml::Shutdown();
        return;
    }

    document->Show();

    Engine::Update.Bind<RmlUiPlugin, &RmlUiPlugin::OnUpdate>(this);
    Engine::LateUpdate.Bind<RmlUiPlugin, &RmlUiPlugin::OnLateUpdate>(this);
    MainRenderTask::Instance->PostRender.Bind<RmlUiPlugin, &RmlUiPlugin::OnPostRender>(this);
}

void RmlUiPlugin::Deinitialize()
{
    MainRenderTask::Instance->PostRender.Unbind<RmlUiPlugin, &RmlUiPlugin::OnPostRender>(this);
    Engine::LateUpdate.Unbind<RmlUiPlugin, &RmlUiPlugin::OnLateUpdate>(this);
    Engine::Update.Unbind<RmlUiPlugin, &RmlUiPlugin::OnUpdate>(this);

    Rml::Shutdown();

    fontInterface.reset();
    fileInterface.reset();
    renderInterface.reset();
    systemInterface.reset();
    context = nullptr;

    GamePlugin::Deinitialize();
}

void RmlUiPlugin::OnUpdate()
{
    if (context)
    {
        context->Update();
    }
}

void RmlUiPlugin::OnLateUpdate()
{
    if (context)
    {
        context->Render();
    }
}

void RmlUiPlugin::OnPostRender(GPUContext* gpuContext, RenderContext& renderContext)
{
    renderInterface->OnPostRender(gpuContext, renderContext);
}
