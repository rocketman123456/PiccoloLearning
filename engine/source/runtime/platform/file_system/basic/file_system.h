#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/platform/file_system/basic/file.h"
#include "runtime/platform/file_system/basic/vblock.h"
#include "runtime/platform/file_system/basic/vnode.h"

#include <algorithm>
#include <filesystem>
#include <future>
#include <string>
#include <vector>

namespace Piccolo
{
    using VNodeList  = std::vector<VNodePtr>;
    using VBlockList = std::vector<VBlockPtr>;
    using VNodeMap   = std::unordered_map<std::string, VNodePtr>;
    using VBlockMap  = std::unordered_map<std::string, VBlockPtr>;

    class FileSystem : public std::enable_shared_from_this<FileSystem>
    {
        friend class FSUtils;

    public:
        FileSystem(const std::filesystem::path& real_path, const std::filesystem::path& virtual_path);
        virtual ~FileSystem() = default;

        inline bool isChanged() const { return m_changed; }
        // For File System
        inline const std::filesystem::path& virtualPath() const { return m_virtual_path; }
        inline const std::filesystem::path& realPath() const { return m_real_path; }

        inline void insertAdditionalFS(std::shared_ptr<FileSystem> fs) { m_additional.push_back(fs); }
        inline void removeAdditionalFS(std::shared_ptr<FileSystem> fs) { m_additional.erase(std::find(m_additional.begin(), m_additional.end(), fs)); }

        // Basic Judgement
        bool isFileExists(const std::filesystem::path& file_path) const;
        bool isDirExists(const std::filesystem::path& dir_path) const;
        bool isFile(const std::filesystem::path& file_path) const;
        bool isDir(const std::filesystem::path& file_path) const;

        // File Operation
        FilePtr openFile(const std::filesystem::path& file_path, int32_t mode) { return nullptr; }
        void    closeFile(const FilePtr& file) {}
        size_t  readFile(const FilePtr& file, FileBuffer& data) { return 0; }
        size_t  writeFile(FilePtr& file, const FileBuffer& data) { return 0; }

        // // Virtual Functions --------------------------------------------------------------
        virtual bool isReadOnly() const { return false; }
        // For File System
        virtual bool createFile(const std::filesystem::path& file_path);
        virtual bool deleteFile(const std::filesystem::path& file_path);
        virtual bool moveFile(const std::filesystem::path& src, const std::filesystem::path& dst);
        virtual bool copyFile(const std::filesystem::path& src, const std::filesystem::path& dst);

        virtual bool createDir(const std::filesystem::path& dir_path);
        virtual bool deleteDir(const std::filesystem::path& dir_path);
        virtual bool moveDir(const std::filesystem::path& src, const std::filesystem::path& dst);
        virtual bool copyDir(const std::filesystem::path& src, const std::filesystem::path& dst);

    protected:
        void buildCache();

        inline VBlockPtr getRoot() const { return m_root; }
        inline void      setRoot(VBlockPtr root) { m_root = root; }

        const VNodeList&  vnodes(const std::filesystem::path& dir) const;
        const VBlockList& vblocks(const std::filesystem::path& dir) const;

        const VNodeMap&  vnodeMap() const;
        const VBlockMap& vblockMap() const;

    protected:
        std::filesystem::path m_real_path {};
        std::filesystem::path m_virtual_path {};

        VBlockPtr m_root {nullptr};
        bool      m_changed {true};

        std::vector<std::shared_ptr<FileSystem>> m_additional;
    };

    using FSPtr = std::shared_ptr<FileSystem>;
} // namespace Piccolo
