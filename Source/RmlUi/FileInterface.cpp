#include "FileInterface.h"

FileInterface::FileInterface()
{
}

FileInterface::~FileInterface()
{
    Array< uintptr_t> keys;
    files.GetKeys(keys);
    for (const auto& handle : keys)
    {
        Close(handle);
    }
}

Rml::FileHandle FileInterface::Open(const std::string& path)
{
    int bufferSize;
    String resultPath = StringUtils::ConvertUTF82UTF16(path.data(), path.size(), bufferSize);

    AssetReference<RawDataAsset> asset = Content::Load<RawDataAsset>(resultPath);

    if (!asset || asset->WaitForLoaded())
    {
        return 0;
    }

    handleCounter++;
    File newFile;
    newFile.Asset = asset;
    newFile.Offset = 0;
    files[handleCounter] = newFile;
    return handleCounter;
}

void FileInterface::Close(Rml::FileHandle file)
{
    files.Remove(file);
}

size_t FileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
{
    if (!files.ContainsKey(file))
    {
        return 0;
    }
    File& target = files.At(file);
    size_t readBytes = 0;
    if (size > target.Asset->Data.Count())
    {
        readBytes = target.Asset->Data.Count();
    }
    else
    {
        readBytes = size;
    }
    Platform::MemoryCopy(buffer, target.Asset->Data.Get(), readBytes);
    target.Offset += readBytes;
    return readBytes;
}

bool FileInterface::Seek(Rml::FileHandle file, long offset, int origin)
{
    if (!files.ContainsKey(file))
    {
        return false;
    }
    File& target = files.At(file);

    uint64_t newOffset = 0;

    // SEEK_SET - the beginning of the file
    if (origin == 0)
    {
        if (offset < 0 || offset >= target.Asset->Data.Count())
        {
            return false;
        }
        target.Offset = offset;
    }
    // SEEK_CUR - the current position of the file pointer
    else if (origin == 1)
    {
        if (target.Offset + offset < 0 || target.Offset + offset >= target.Asset->Data.Count())
        {
            return false;
        }
        target.Offset += offset;
    }
    // SEEK_END - the end of the file
    else if (origin == 2)
    {
        if (offset > 0 || target.Offset + offset < 0)
        {
            return false;
        }
        target.Offset += offset;
    }
    else
    {
        return false;
    }
    return true;
}

size_t FileInterface::Tell(Rml::FileHandle file)
{
    if (!files.ContainsKey(file))
    {
        return 0;
    }
    return files[file].Offset;
}

size_t FileInterface::Length(Rml::FileHandle file)
{
    if (!files.ContainsKey(file))
    {
        return 0;
    }
    return files[file].Asset->Data.Count();
}

bool FileInterface::LoadFile(const std::string& path, std::string& out_data)
{
    int bufferSize;
    String resultPath = StringUtils::ConvertUTF82UTF16(path.data(), path.size(), bufferSize);

    AssetReference<RawDataAsset> asset = Content::Load<RawDataAsset>(resultPath);

    if (!asset || asset->WaitForLoaded())
    {
        return false;
    }

    out_data.resize(asset->Data.Count());
    Platform::MemoryCopy(&out_data[0], asset->Data.Get(), asset->Data.Count());
    return true;
}
