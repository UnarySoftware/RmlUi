#pragma once

#include <Engine/Scripting/Script.h>
#include <Engine/Content/Content.h>
#include <Engine/Content/AssetReference.h>
#include <Engine/Content/Assets/RawDataAsset.h>
#include <Engine/Core/Types/BaseTypes.h>
#include <Engine/Core/Math/Vector2.h>
#include <Engine/Core/Types/CommonValue.h>
#include <Engine/Core/Collections/Dictionary.h>

#define RMLUI_STATIC_LIB
#define RMLUI_NO_THIRDPARTY_CONTAINERS
#define RMLUI_USE_CUSTOM_RTTI

#undef RadiansToDegrees
#undef DegreesToRadians

#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/StringUtilities.h>

// Converts radians to degrees.
#define RadiansToDegrees (180.0f / PI)
// Converts degrees to radians.
#define DegreesToRadians (PI / 180.0f)

class FileInterface : public Rml::FileInterface
{
private:

    struct File
    {
        AssetReference<RawDataAsset> Asset;
        uint64_t Offset = 0;
    };

    uint64 handleCounter = 0;
    Dictionary<uintptr_t, File> files;

public:
    FileInterface();
    virtual ~FileInterface();
    Rml::FileHandle Open(const std::string& path) override;
    void Close(Rml::FileHandle file) override;
    size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;
    bool Seek(Rml::FileHandle file, long offset, int origin) override;
    size_t Tell(Rml::FileHandle file) override;
    size_t Length(Rml::FileHandle file) override;
    bool LoadFile(const std::string& path, std::string& out_data) override;
};
