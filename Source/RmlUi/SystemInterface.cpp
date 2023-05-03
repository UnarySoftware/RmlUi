#include "SystemInterface.h"

SystemInterface::SystemInterface()
{
}

SystemInterface::~SystemInterface()
{
}

double SystemInterface::GetElapsedTime()
{
    return Time::GetTimeSinceStartup();
}

int SystemInterface::TranslateString(std::string& translated, const std::string& input)
{
    int bufferSize;
    String resultText = StringUtils::ConvertUTF82UTF16(input.data(), input.size(), bufferSize);
    String resultTranslation = Localization::GetString(resultText, resultText);
    char* result = StringUtils::ConvertUTF162UTF8(resultTranslation.GetText(), resultTranslation.Length(), bufferSize);
    translated = std::string(result, bufferSize);
    return bufferSize;
}

bool SystemInterface::LogMessage(Rml::Log::Type type, const std::string& message)
{
    int bufferSize;
    String result = StringUtils::ConvertUTF82UTF16(message.data(), message.length(), bufferSize);
    switch (type)
    {
    case Rml::Log::Type::LT_ALWAYS:
    {
        LOG_STR(Info, result);
        break;
    }
    case Rml::Log::Type::LT_ERROR:
    case Rml::Log::Type::LT_ASSERT:
    {
        LOG_STR(Error, result);
        break;
    }
    case Rml::Log::Type::LT_WARNING:
    {
        LOG_STR(Warning, result);
        break;
    }
    case Rml::Log::Type::LT_INFO:
    case Rml::Log::Type::LT_DEBUG:
    {
        LOG_STR(Info, result);
        break;
    }
    }
    return true;
}

void SystemInterface::SetMouseCursor(const std::string& cursor_name)
{
    if (cursor_name.empty() || cursor_name == "arrow")
        Engine::MainWindow->SetCursor(CursorType::Default);
    else if (cursor_name == "move")
        Engine::MainWindow->SetCursor(CursorType::Hand);
    else if (cursor_name == "pointer")
        Engine::MainWindow->SetCursor(CursorType::Default);
    else if (cursor_name == "resize")
        Engine::MainWindow->SetCursor(CursorType::SizeAll);
    else if (cursor_name == "cross")
        Engine::MainWindow->SetCursor(CursorType::Cross);
    else if (cursor_name == "text")
        Engine::MainWindow->SetCursor(CursorType::IBeam);
    else if (cursor_name == "unavailable")
        Engine::MainWindow->SetCursor(CursorType::No);
    else if (Rml::StringUtilities::StartsWith(cursor_name, "rmlui-scroll"))
        Engine::MainWindow->SetCursor(CursorType::SizeNS);
}

void SystemInterface::SetClipboardText(const std::string& text)
{
    int bufferSize;
    Clipboard::SetText(StringUtils::ConvertUTF82UTF16(text.data(), text.size(), bufferSize));
}

void SystemInterface::GetClipboardText(std::string& text)
{
    String engineText = Clipboard::GetText();
    int bufferSize;
    char* result = StringUtils::ConvertUTF162UTF8(engineText.GetText(), engineText.Length(), bufferSize);
    text = std::string(result, bufferSize);
}
