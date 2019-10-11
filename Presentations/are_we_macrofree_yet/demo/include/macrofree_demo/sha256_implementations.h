#pragma once

#include <macrofree_demo/build_config.h>
#include <macrofree_demo/sha256_openssl.h>
#include <macrofree_demo/sha256_cng.h>

#include <type_traits>
#include <tuple>

namespace macrofree_demo
{

template<bool crypto, bool cng>
struct sha256_implementations_table
{
    using type = std::tuple<>;
};

template<>
struct sha256_implementations_table<false, true>
{
    using type = std::tuple<sha256_cng>;
};

template<>
struct sha256_implementations_table<true, false>
{
    using type = std::tuple<sha256_openssl>;
};

template<>
struct sha256_implementations_table<true, true>
{
    using type = std::tuple<sha256_openssl, sha256_cng>;
};

using sha256_implementations =
    typename sha256_implementations_table<have_crypto, have_cng>::type;

}
