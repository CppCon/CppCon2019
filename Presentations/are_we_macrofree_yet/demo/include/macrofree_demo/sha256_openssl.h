#pragma once

#include <memory>

namespace macrofree_demo
{

class sha256_openssl
{
  public:
    sha256_openssl();
    ~sha256_openssl();

    void update(void const* data, size_t len);
    void final(unsigned char* md) const;

  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
