#include <macrofree_demo/sha256_openssl.h>

#include <openssl/sha.h>

namespace macrofree_demo
{

struct sha256_openssl::impl
{
    impl() noexcept { SHA256_Init(&ctx); }

    SHA256_CTX ctx;
};

sha256_openssl::sha256_openssl()
    : impl_(std::make_unique<sha256_openssl::impl>())
{
}

sha256_openssl::~sha256_openssl() = default;

void sha256_openssl::update(void const* data, size_t len)
{
    SHA256_Update(&impl_->ctx, data, len);
}

void sha256_openssl::final(unsigned char* md) const
{
    auto ctx = impl_->ctx;
    SHA256_Final(md, &ctx);
}

}
