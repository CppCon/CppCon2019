#include <doctest/doctest.h>

#include <macrofree_demo/c_file_funcs.h>

#include <memory>
#include <stdio.h>

using namespace macrofree_demo;

SCENARIO("open created file with unique_ptr")
{
    char fn[] = "scenario-unique_ptr-tmp.txt";
    using c_file_ptr = std::unique_ptr<FILE, c_file_deleter>;

    WHEN ("a file is created")
    {
        auto fp = c_file_ptr(xfopen(fn, "w"));
        fp.reset();

        THEN ("we can open the file")
        {
            REQUIRE_NOTHROW(c_file_ptr(xfopen(fn, "r")));
        }

        ::remove(fn);
    }

    WHEN ("the file is not created")
    {
        THEN ("we cannot open the file")
        {
            REQUIRE_THROWS_AS(c_file_ptr(xfopen(fn, "r")), std::system_error);
        }
    }
}

SCENARIO("get_file_size on files created")
{
    char fn[] = "scenario-get_file_size-tmp.txt";
    using c_file_ptr = std::unique_ptr<FILE, c_file_deleter>;

    GIVEN ("a new file")
    {
        auto fp = c_file_ptr(xfopen(fn, "wb"));

        WHEN ("we close it without writing anything")
        {
            fp.reset();

            THEN ("its file size should be zero")
            {
                REQUIRE(get_file_size(fn) == 0);
            }
        }

        WHEN ("we wrote some content before closing it")
        {
            ::fwrite("binary stream input/output", 1, 17, fp.get());
            fp.reset();

            THEN ("its file size should be the length of the content we wrote")
            {
                REQUIRE(get_file_size(fn) == 17);
            }
        }

        ::remove(fn);
    }
}
