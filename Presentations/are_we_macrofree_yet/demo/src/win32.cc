#include <macrofree_demo/c_file_funcs.h>

#include <sys/types.h>
#include <sys/stat.h>

namespace macrofree_demo
{

int64_t get_file_size(char const* filename)
{
    struct _stat64 st;
    _stat64(filename, &st);
    return st.st_size;
}

}
