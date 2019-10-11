#pragma once

#include <memory>

namespace macrofree_demo
{

class sha256_cng
{
  public:
    sha256_cng();
    ~sha256_cng();

    void update(void const* data, size_t len);
    void final(unsigned char* md) const;

  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
