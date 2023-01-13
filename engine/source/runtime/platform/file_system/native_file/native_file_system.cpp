#include "runtime/platform/file_system/native_file/native_file_system.h"
#include "runtime/platform/file_system/native_file/native_file.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/thread/thread_pool.h"

#include <exception>
#include <filesystem>
#include <stdexcept>

namespace Piccolo
{
    NativeFileSystem::NativeFileSystem(const std::string& vpath, const std::string rpath) : FileSystem(vpath, rpath)
    {
        if (!std::filesystem::exists(rpath))
        {
            LOG_ERROR("Native File System {} Not Exist", rpath);
            throw std::invalid_argument("Native File System Not Exist");
        }
        if (!std::filesystem::is_directory(rpath))
        {
            LOG_ERROR("Native File System {} Not a Dir Path", rpath);
            throw std::invalid_argument("Native File System Not A Path");
        }
    }

    void NativeFileSystem::buildFSCache()
    {
        // iterate all file in current path
        for (auto const& directory_entry : std::filesystem::recursive_directory_iterator {m_rpath})
        {
            if (directory_entry.is_regular_file()) // build file cache
            {
                std::filesystem::path path = directory_entry;
                m_files.push_back(path.string());
            }
            else if (directory_entry.is_directory()) // build dir cache
            {
                std::filesystem::path path = directory_entry;
                m_dirs.push_back(path.string());
            }
        }
    }

    FilePtr NativeFileSystem::open(const std::string& vpath, uint32_t mode)
    {
        // normalize vpath
        // remove native file system vpath prefix
        // get real path
        std::string   rpath = m_rpath + "/" + vpath;
        NativeFilePtr file = std::make_shared<NativeFile>(vpath, rpath);
        if (file->open(mode))
            return file;
        else
            return nullptr;
    }

    bool NativeFileSystem::close(FilePtr file) { return file->close(); }
} // namespace Piccolo
