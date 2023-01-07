#include "runtime/platform/file_system/basic/file_system.h"

#include "runtime/core/base/crc.h"
#include "runtime/core/base/macro.h"
#include "runtime/core/base/string_utils.h"

namespace Piccolo
{
    FileSystem::FileSystem(const std::filesystem::path& real_path, const std::filesystem::path& virtual_path)
    {
        m_real_path    = replace_all(real_path.string(), "\\", "/");
        m_virtual_path = replace_all(virtual_path.string(), "\\", "/");

        m_root         = std::make_shared<VBlock>();
        m_root->parent = nullptr;

        std::vector<std::string> dir_stack;
        split_single_char(m_virtual_path.string(), '/', dir_stack);
        if (dir_stack.size() > 0)
            m_root->name = dir_stack[dir_stack.size() - 1];
        else
            m_root->name = "";

        m_root->path        = m_virtual_path;
        m_root->rpath       = m_real_path;
        m_root->file_system = FileSystem::shared_from_this();
        m_root->crc         = g_crc32.calculate(m_root->path.c_str());

        LOG_DEBUG("FS Root Block Path: {}", m_root->path.string());

        m_changed = true;
    }

    bool FileSystem::createFile(const std::filesystem::path& file_path)
    {
        LOG_WARN("Create File Not Supported {}", file_path.string());
        return false;
    }

    bool FileSystem::deleteFile(const std::filesystem::path& file_path)
    {
        LOG_WARN("Delete File Not Supported {}", file_path.string());
        return false;
    }

    bool FileSystem::moveFile(const std::filesystem::path& src, const std::filesystem::path& dst)
    {
        LOG_WARN("Move File Not Supported {} : {}", src.string(), dst.string());
        return false;
    }

    bool FileSystem::copyFile(const std::filesystem::path& src, const std::filesystem::path& dst)
    {
        LOG_WARN("Copy File Not Supported {} : {}", src.string(), dst.string());
        return false;
    }

    bool FileSystem::createDir(const std::filesystem::path& dir_path)
    {
        LOG_WARN("Create Dir Not Supported {}", dir_path.string());
        return false;
    }

    bool FileSystem::deleteDir(const std::filesystem::path& dir_path)
    {
        LOG_WARN("Delete Dir Not Supported {}", dir_path.string());
        return false;
    }

    bool FileSystem::moveDir(const std::filesystem::path& src, const std::filesystem::path& dst)
    {
        LOG_WARN("Move Dir Not Supported {} : {}", src.string(), dst.string());
        return false;
    }

    bool FileSystem::copyDir(const std::filesystem::path& src, const std::filesystem::path& dst)
    {
        LOG_WARN("Copy Dir Not Supported {} : {}", src.string(), dst.string());
        return false;
    }

} // namespace Piccolo
