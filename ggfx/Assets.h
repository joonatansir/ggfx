#pragma once

#include <unordered_map>
#include <string>

namespace ggfx
{
    class Assets
    {
    public:

        Assets() = delete;
        Assets(const Assets&) = delete;
        Assets& operator =(const Assets&) = delete;
        
        static void scanAssetDirectory(const std::string& path, const std::string& directoryName = "");
        static std::string& getPath(const std::string& assetName);

    private:

        static std::unordered_map<std::string, std::string> assets;

    };
}