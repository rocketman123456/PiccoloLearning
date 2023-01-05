#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <cstdint>
#include <cstdlib>
#include <cstring> // std::memcpy
#include <memory>
#include <string>

namespace Piccolo
{
    enum class FileOperateMode : int8_t
    {
        EMPTY_OPERATION = 0,
        READ_BINARY,
        WRITE_BINARY,
        READWRITE_BINARY,
        READ_TEXT,
        WRITE_TEXT,
        READWRITE_TEXT,
        OPERATION_COUNT,
    };

    class FileInfo
    {
    public:
        void*       file_pointer    = nullptr;
        void*       extra_file_info = nullptr;
        size_t      total_size;
        std::string file_path;
        std::string file_name;
        std::string full_name;
    };

    class FileBuffer
    {
    public:
        int64_t uuid   = 0; // hash code for check, not in use
        int64_t size   = 0; // bytes
        void*   buffer = nullptr;

        FileBuffer() = default;
        ~FileBuffer()
        {
            if (buffer)
                delete[] buffer;
        } // Auto Delete

        FileBuffer(const FileBuffer& buffer)            = delete;
        FileBuffer& operator=(const FileBuffer& buffer) = delete;

        // Move
        FileBuffer(FileBuffer&& buffer)
        {
            this->uuid    = buffer.uuid;
            this->size    = buffer.size;
            buffer.buffer = nullptr;
            buffer.size   = 0;
        }

        // Move
        FileBuffer& operator=(FileBuffer&& buffer)
        {
            if (this == &buffer)
                return *this;
            if (this->buffer)
                delete[] this->buffer;
            this->uuid    = buffer.uuid;
            this->size    = buffer.size;
            buffer.buffer = nullptr;
            buffer.size   = 0;
            return *this;
        }
    };
} // namespace Piccolo
