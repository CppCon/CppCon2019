#pragma once

#include <macrofree_demo/build_config.h>

#include <stdio.h>
#include <system_error>

extern "C"
{
    using errno_t = int;
    errno_t fopen_s(FILE**, const char* filename, const char* mode);
}

namespace macrofree_demo
{

inline FILE* xfopen(char const* fn, char const* mode)
{
    FILE* fp;
    errno_t ec;

    if constexpr (have_annex_k)
    {
        ec = fopen_s(&fp, fn, mode);
    }
    else
    {
        fp = ::fopen(fn, mode);
        ec = errno;
    }

    if (fp == nullptr)
        throw std::system_error(ec, std::system_category());

    return fp;
}

struct c_file_deleter
{
    void operator()(FILE* fp) const { ::fclose(fp); }
};

int64_t get_file_size(char const* filename);

}
