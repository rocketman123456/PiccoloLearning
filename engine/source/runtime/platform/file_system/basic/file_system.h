#pragma once
#include "runtime/platform/file_system/basic/file.h"

#include <algorithm>
#include <future>

namespace Piccolo
{
    class FileSystem
    {
    public:
        virtual ~FileSystem() = default;

        virtual void buildSystemCache();

        virtual std::string vpath() const { return m_vpath; }
        virtual std::string rpath() const { return m_rpath; }

        virtual const std::vector<std::string>& allFiles() const { return m_files; }
        virtual const std::vector<std::string>& allDirs() const { return m_dirs; }

        virtual bool isFileExist(const std::string& file_name) const
        {
            auto iter = std::find_if(m_files.begin(), m_files.end(), [file_name](const std::string& file) { return file_name == file; });
            return iter != m_files.end();
        }

        virtual bool isDirExist(const std::string& dir_name) const
        {
            auto iter = std::find_if(m_dirs.begin(), m_dirs.end(), [dir_name](const std::string& file) { return dir_name == file; });
            return iter != m_dirs.end();
        }

        // -------------------------------------------------------------------
        // -------------------------------------------------------------------
        // -------------------------------------------------------------------

        virtual FilePtr open(const std::string& vpath, uint32_t mode) = 0;
        virtual void    close(FilePtr file)                           = 0;

        virtual bool readSync(FilePtr file, std::vector<std::byte>& buffer) = 0;;
        virtual bool writeSync(FilePtr file, const std::vector<std::byte>& buffer) = 0;

        virtual std::future<bool> readAsync(FilePtr file, std::vector<std::byte>& buffer) = 0;
        virtual std::future<bool> writeAsync(FilePtr file, const std::vector<std::byte>& buffer) = 0;

        // virtual bool createFile(const std::string& file_path) = 0;
        // virtual bool deleteFile(const std::string& file_path) = 0;
        // virtual bool moveFile(const std::string& src, const std::string& dst) = 0;
        // virtual bool copyFile(const std::string& src, const std::string& dst) = 0;

        // virtual bool createDir(const std::string& dir_path) = 0;
        // virtual bool deleteDir(const std::string& dir_path) = 0;
        // virtual bool moveDir(const std::string& src, const std::string& dst) = 0;
        // virtual bool copyDir(const std::string& src, const std::string& dst) = 0;

    protected:
        std::string m_vpath;
        std::string m_rpath;

        std::vector<std::string> m_files;
        std::vector<std::string> m_dirs;
    };

    using FileSystemPtr = std::shared_ptr<FileSystem>;
} // namespace Piccolo
