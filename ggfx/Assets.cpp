#include <dirent.h>
#include <direct.h>

#include "Assets.h"
#include "Log.h"

using namespace ggfx;

std::unordered_map<std::string, std::string> Assets::assets;

void Assets::scanAssetDirectory(const std::string& path, const std::string& directory)
{
    DIR* dir = opendir(path.c_str());

    if (dir)
    {
        while (dirent* entry = readdir(dir))
        {
            if (entry->d_name[0] != '.')
            {
                if (entry->d_type == DT_DIR)
                {
                    std::string newDirectory = std::string(entry->d_name) + '/';
                    std::string newPath = path + newDirectory;
                    scanAssetDirectory(newPath, directory + newDirectory);
                }
                else
                {
                    Log::info("%s - %d\n", entry->d_name, entry->d_type);
                  
                    std::string filename(entry->d_name);
                    assets[directory + filename] = path + filename;
                }
            }
        }
    }

    closedir(dir);
}

const char* ggfx::Assets::getPath(char* assetName)
{
    auto asset = assets.find(assetName);
    if (asset == assets.end())
    {
        Log::warning("Asset not found! - %s\n", assetName);
        return "";
    }
    return asset->second.c_str();
}