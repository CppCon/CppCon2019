#include <macrofree_demo/sha256_cng.h>

#include <windows.h>
#include <bcrypt.h>

#include <system_error>
#include <type_traits>
#include <ciso646>

namespace macrofree_demo
{

class hash
{
    friend class algorithm_provider;

    struct deleter
    {
        void operator()(BCRYPT_HASH_HANDLE h) const { BCryptDestroyHash(h); }
    };

    using native_handle_t = std::remove_pointer_t<BCRYPT_HASH_HANDLE>;

    std::unique_ptr<native_handle_t, deleter> handle_;

    explicit hash(BCRYPT_ALG_HANDLE alg_handle)
        : handle_([=] {
              BCRYPT_HASH_HANDLE handle;
              auto status = BCryptCreateHash(alg_handle, &handle, nullptr, 0,
                                             nullptr, 0, 0);
              if (not BCRYPT_SUCCESS(status))
                  throw std::system_error(status, std::system_category());

              return handle;
          }())
    {
    }

  public:
    hash(hash const& other)
        : handle_([&] {
              BCRYPT_HASH_HANDLE handle;
              auto status = BCryptDuplicateHash(other.handle_.get(), &handle,
                                                nullptr, 0, 0);
              if (not BCRYPT_SUCCESS(status))
                  throw std::system_error(status, std::system_category());

              return handle;
          }())
    {
    }

    hash& operator=(hash const& other) &
    {
        auto tmp = other;
        std::swap(*this, tmp);
        return *this;
    }

    void hash_data(UCHAR const* p, ULONG len)
    {
        auto status =
            BCryptHashData(handle_.get(), const_cast<UCHAR*>(p), len, 0);
        if (not BCRYPT_SUCCESS(status))
            throw std::system_error(status, std::system_category());
    }

    void finish_hash(PUCHAR md, ULONG len)
    {
        auto status = BCryptFinishHash(handle_.get(), md, len, 0);
        if (not BCRYPT_SUCCESS(status))
            throw std::system_error(status, std::system_category());
    }
};

class algorithm_provider
{
    struct deleter
    {
        void operator()(BCRYPT_ALG_HANDLE h) const
        {
            BCryptCloseAlgorithmProvider(h, 0);
        }
    };

    using native_handle_t = std::remove_pointer_t<BCRYPT_ALG_HANDLE>;

    std::unique_ptr<native_handle_t, deleter> handle_;

  public:
    explicit algorithm_provider(LPCWSTR algid)
        : handle_([=] {
              BCRYPT_ALG_HANDLE handle;
              auto status =
                  BCryptOpenAlgorithmProvider(&handle, algid, nullptr, 0);
              if (not BCRYPT_SUCCESS(status))
                  throw std::system_error(status, std::system_category());

              return handle;
          }())
    {
    }

    auto create_hash() -> hash { return hash(handle_.get()); }
};

struct sha256_cng::impl
{
    algorithm_provider alg{ BCRYPT_SHA256_ALGORITHM };
    hash ctx = alg.create_hash();
};

sha256_cng::sha256_cng() : impl_(std::make_unique<sha256_cng::impl>())
{
}

sha256_cng::~sha256_cng() = default;

void sha256_cng::update(void const* data, size_t len)
{
    // XXX len > 4Gb unsupported
    impl_->ctx.hash_data(reinterpret_cast<UCHAR const*>(data),
                         static_cast<ULONG>(len));
}

void sha256_cng::final(unsigned char* md) const
{
    auto ctx = impl_->ctx;
    ctx.finish_hash(md, 32);
}

}
