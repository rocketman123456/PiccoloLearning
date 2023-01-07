#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

namespace Piccolo
{
    using FileByte    = uint8_t;
    using FileBuffer  = std::vector<uint8_t>;
    using FileContent = std::string;

    class VNode;

    class File
    {
    public:
        enum Mode : uint32_t
        {
            read_bin        = 1 << 0,
            write_bin       = 1 << 1,
            read_write_bin  = read_bin | write_bin,
            read_text       = 1 << 2,
            write_text      = 1 << 3,
            read_write_text = read_text | write_text,
            append          = 1 << 4,
            truncate        = 1 << 5,
        };

        enum Position : uint32_t
        {
            beg = 0,
            end = 1,
            set = 2,
        };

        enum Mount : uint32_t
        {
            overwrite  = 0,
            additional = 1,
        };

        virtual ~File() = default;

        virtual std::shared_ptr<VNode> vnode() const = 0;

        virtual std::filesystem::path virtualPath() const = 0;
        virtual std::filesystem::path realPath() const    = 0;

        virtual void Open(Mode mode) = 0;
        virtual void Close()         = 0;

        virtual bool isOpened() const   = 0;
        virtual bool isReadOnly() const = 0;

        virtual size_t seek(size_t offset, Position pos) = 0;
        virtual size_t tell()                            = 0;

        virtual size_t size() const = 0;
        //  If Read Text, read will read [size - 1] bytes,
        //  because last will be ['\0'] for string
        virtual size_t read(FileBuffer& data) = 0;
        // Write will write all data to file, has no different
        virtual size_t write(const FileBuffer& data) = 0;
    };

    using FilePtr = std::shared_ptr<File>;
} // namespace Piccolo
