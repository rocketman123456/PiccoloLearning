#pragma once
#include "runtime/platform/file_system/basic/file.h"
#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/memory_file/memory_file.h"
#include "runtime/platform/file_system/memory_file/memory_file_system.h"
#include "runtime/platform/file_system/native_file/native_file.h"
#include "runtime/platform/file_system/native_file/native_file_system.h"
#include "runtime/platform/file_system/zip_file/zip_file.h"
#include "runtime/platform/file_system/zip_file/zip_file_system.h"

#include "runtime/platform/file_system/vfs_config.h"

#include <unordered_map>

namespace Piccolo
{
    class VFS
    {
    public:
        void mount(const VFSConfig& config);

    private:
        void mountFS(const FSConfig& fs);
        void unmountFS(const FSConfig& fs);
        void unmountAll();

        void buildVFSCache();

    private:
        std::vector<FileSystemPtr> m_fs;
        std::unordered_map<std::string, FileSystemPtr> m_fileCache;
        std::unordered_map<std::string, FileSystemPtr> m_dirCache;
    };
} // namespace Piccolo