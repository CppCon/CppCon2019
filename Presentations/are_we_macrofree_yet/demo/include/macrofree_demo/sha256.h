#pragma once

#include <macrofree_demo/hexlify.h>

#include <array>
#include <memory>
#include <string>
#include <utility>

namespace macrofree_demo
{

class sha256
{
  public:
    static constexpr size_t digest_size = 32;
    static constexpr size_t block_size = 64;

    using digest_type = std::array<unsigned char, digest_size>;

    sha256() = default;

    template<class T, class... Args>
    explicit sha256(std::in_place_type_t<T>, Args&&... args)
        : this_(std::make_unique<sha256_core<T>>(std::in_place,
                                                 std::forward<Args>(args)...))
    {
    }

    void update(char const* s)
    {
        update(s, std::char_traits<char>::length(s));
    }

    void update(char const* p, size_t n) { this_->update(p, n); }

    template<class StringLike>
    void update(StringLike const& bytes)
    {
        update(bytes.data(), bytes.size());
    }

    auto digest() const -> digest_type
    {
        digest_type md;
        this_->final(md.data());

        return md;
    }

    auto hexdigest() const -> std::string { return hexlify(digest()); }

  private:
    class sha256_interface
    {
      public:
        virtual void update(void const* data, size_t len) = 0;
        virtual void final(unsigned char* md) const = 0;
        virtual ~sha256_interface() = default;
    };

    template<class T>
    class sha256_core final : public sha256_interface
    {
      public:
        template<class... Args>
        sha256_core(std::in_place_t, Args&&... args)
            : self_(std::forward<Args>(args)...)
        {
        }

        void update(void const* data, size_t len) override
        {
            self_.update(data, len);
        }

        void final(unsigned char* md) const override { self_.final(md); }

      private:
        T self_;
    };

    std::unique_ptr<sha256_interface> this_;
};

}
