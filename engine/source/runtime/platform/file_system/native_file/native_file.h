#pragma once
#include "runtime/platform/file_system/basic/file.h"

#include <fstream>

namespace Piccolo
{
    class NativeFile : public File
    {
    public:
        NativeFile(const std::string& vpath, const std::string& rpath);
        virtual ~NativeFile();

        virtual bool open(uint32_t mode);
        virtual bool close();

        virtual bool   isOpened() const;
        virtual bool   isReadOnly() const;
        virtual size_t size() const;
        virtual size_t seek(size_t offset, Origin origin);
        virtual size_t tell();
        // If Read Text, read will read [size - 1] bytes,
        // because last will be ['\0'] for string
        virtual size_t read(std::vector<std::byte>& data);
        virtual size_t write(const std::vector<std::byte>& data);

    private:
        std::fstream m_stream;
        bool         m_read_only {false};
    };
} // namespace Piccolo