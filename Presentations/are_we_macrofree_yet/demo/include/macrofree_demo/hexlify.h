#pragma once

#include <algorithm>
#include <array>
#include <string>

namespace macrofree_demo
{

template<size_t N, class OutIt>
inline OutIt hexlify_to(std::array<unsigned char, N> const& md, OutIt it)
{
    auto half_to_hex = [](int c) {
        // does not work if the source encoding is not ASCII-compatible
        return char((c > 9) ? c + 'a' - 10 : c + '0');
    };

    std::for_each(md.begin(), md.end(), [&](unsigned char c) {
        *it = half_to_hex((c >> 4) & 0xf);
        ++it;
        *it = half_to_hex(c & 0xf);
        ++it;
    });

    return it;
}

template<size_t N>
inline std::string hexlify(std::array<unsigned char, N> const& md)
{
    std::string s;
    s.resize(N * 2);
    hexlify_to(md, begin(s));

    return s;
}

}
