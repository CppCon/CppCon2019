#include <macrofree_demo/c_file_funcs.h>

#include <sys/types.h>
#include <sys/stat.h>

namespace macrofree_demo
{

int64_t get_file_size(char const* filename)
{
    struct stat st;
    ::stat(filename, &st);
    return st.st_size;
}

}
