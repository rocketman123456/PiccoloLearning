#include "runtime/platform/file_system/memory_file/memory_file_system.h"
#include "runtime/platform/file_system/basic/file_utils.h"

namespace Piccolo
{
    void MemoryFileSystem::buildFSCache() {}

    FilePtr MemoryFileSystem::open(const std::string& vpath_, uint32_t mode)
    {
        auto vpath = getNormalizedPath(vpath_);
        return std::make_shared<MemoryFile>(vpath, vpath);
    }

    bool MemoryFileSystem::close(FilePtr file) { return file->close(); }
} // namespace Piccolo
