#pragma once

#include <Engine/Platform/Platform.h>
#include <Engine/Platform/Clipboard.h>
#include <Engine/Platform/Window.h>
#include <Engine/Engine/Engine.h>
#include <Engine/Localization/Localization.h>
#include <Engine/Engine/Time.h>
#include <Engine/Debug/DebugLog.h>

#define RMLUI_STATIC_LIB
#define RMLUI_NO_THIRDPARTY_CONTAINERS
#define RMLUI_USE_CUSTOM_RTTI

#undef RadiansToDegrees
#undef DegreesToRadians

#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/StringUtilities.h>

// Converts radians to degrees.
#define RadiansToDegrees (180.0f / PI)
// Converts degrees to radians.
#define DegreesToRadians (PI / 180.0f)

class SystemInterface : public Rml::SystemInterface
{
public:
    SystemInterface();
    virtual ~SystemInterface();
    double GetElapsedTime() override;
    int TranslateString(std::string& translated, const std::string& input) override;
    bool LogMessage(Rml::Log::Type type, const std::string& message) override;
    void SetMouseCursor(const std::string& cursor_name) override;
    void SetClipboardText(const std::string& text) override;
    void GetClipboardText(std::string& text) override;
};
