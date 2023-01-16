#include "runtime/resource/asset_manager/asset_manager.h"

#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"

#include <filesystem>

namespace Piccolo
{
    std::filesystem::path AssetManager::getFullPath(const std::string& relative_path) const
    {
        return std::filesystem::absolute(g_runtime_global_context.m_config_manager->getRootFolder() / relative_path);
    }

    void AssetManager::readTextFile(const std::filesystem::path& file_path, std::string& content) const
    {
        std::ifstream fin(file_path, std::ios::in);
        if(!fin)
        {
            LOG_ERROR("open file: {} failed!", file_path.generic_string());
            return;
        }
        content = {std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};
    }

    void AssetManager::readBinaryFile(const std::filesystem::path& file_path, std::vector<std::byte>& content) const
    {
        std::ifstream fin(file_path, std::ios::in | std::ios::binary);
        if(!fin)
        {
            LOG_ERROR("open file: {} failed!", file_path.generic_string());
            return;
        }

        size_t file_size = fin.tellg();
        content.resize(file_size);

        fin.seekg(0);
        fin.read(reinterpret_cast<char*>(content.data()), sizeof(file_size));
        fin.close();
    }
} // namespace Piccolo