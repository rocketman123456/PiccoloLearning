#include "runtime/platform/file_system/vfs.h"

#include <algorithm>

namespace Piccolo
{
    void VFS::mount(const VFSConfig& config)
    {
        for (auto& fs : config.m_configs)
        {
            mountFS(fs);
        }
        buildVFSCache();
    }

    void VFS::mountFS(const FSConfig& fs)
    {
        if (fs.m_type == "native")
        {
            m_fs.emplace_back(std::make_shared<NativeFileSystem>(fs.m_vpath, fs.m_rpath));
        }
        else if (fs.m_type == "memory")
        {
            // TODO
        }
        else if (fs.m_type == "zip")
        {
            // TODO
        }
    }

    void VFS::unmountFS(const FSConfig& config)
    {
        auto iter =
            std::find_if(m_fs.begin(), m_fs.end(), [config](FileSystemPtr fs) { return fs->m_rpath == config.m_rpath && fs->m_vpath == config.m_vpath; });
        if (iter != m_fs.end())
        {
            m_fs.erase(iter);
        }
    }

    void VFS::unmountAll()
    {
        m_fs.clear();
        m_fileCache.clear();
        m_dirCache.clear();
    }

    void VFS::buildVFSCache()
    {
        for (auto fs : m_fs)
        {
            for (auto file : fs->m_vfiles)
            {
                m_fileCache.emplace(file, fs);
            }
            for (auto file : fs->m_vdirs)
            {
                m_dirCache.emplace(file, fs);
            }
        }
    }
} // namespace Piccolo
