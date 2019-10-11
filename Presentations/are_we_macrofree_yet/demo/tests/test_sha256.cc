#include <doctest/doctest.h>

#include <macrofree_demo/sha256.h>
#include <macrofree_demo/sha256_implementations.h>

#include <string_view>

using namespace macrofree_demo;
using namespace std::literals;

TYPE_TO_STRING(sha256_openssl);
TYPE_TO_STRING(sha256_cng);

SCENARIO_TEMPLATE_DEFINE("feed sha256 inputs", T, test_sha256)
{
    auto h2 = sha256(std::in_place_type<T>);

    GIVEN ("a sha256 function in initial state")
    {
        auto h = sha256(std::in_place_type<T>);

        WHEN ("we feed no input")
        {
            THEN ("it gives a known message digest")
            {
                REQUIRE(h.hexdigest() == "e3b0c44298fc1c149afbf4c8996fb92427ae"
                                         "41e4649b934ca495991b7852b855");
            }

            AND_THEN ("the message digest is that of an empty string")
            {
                h2.update("");

                REQUIRE(h.digest() == h2.digest());
            }
        }

        WHEN ("we feed some input")
        {
            h.update("Atelier Ryza");

            THEN ("the message digest is not as same as that of no input")
            {
                REQUIRE(h.digest() != h2.digest());
            }

            AND_THEN ("nor as same as that of a different input")
            {
                h2.update("Ever Darkness & the Secret Hideout");

                REQUIRE(h.digest() != h2.digest());
            }
        }

        WHEN ("we feed multiple inputs")
        {
            h.update("macrofree");
            h.update("-");
            h.update("demo");

            THEN ("the input is as-if concatenated")
            {
                h2.update("macrofree-demo");

                REQUIRE(h.digest() == h2.digest());
            }
        }

        WHEN ("we feed input with embedded zero")
        {
            h.update("Re:\0 - Starting Life in Another World"sv);

            THEN ("it gives a known message digest")
            {
                REQUIRE(h.hexdigest() == "c3e6cddc055c28268d52b9046db8b3d28a81"
                                         "912c434b17d25514507db0eef6d0");
            }

            AND_THEN ("the message is not treated as truncated")
            {
                h2.update("Re:"sv);

                REQUIRE(h.digest() != h2.digest());
            }
        }
    }
}

TEST_CASE_TEMPLATE_APPLY(test_sha256, sha256_implementations);

SCENARIO_TEMPLATE_DEFINE("retrieve sha256 message digest multiple times", T,
                         test_sha256_multiple)
{
    auto h2 = sha256(std::in_place_type<T>);

    GIVEN ("a sha256 function in initial state")
    {
        auto h = sha256(std::in_place_type<T>);

        WHEN ("we've accessed its message digest")
        {
            auto md = h.digest();

            THEN ("we can access it again and get the same result")
            {
                REQUIRE(h.digest() == md);
            }
        }

        WHEN ("we've feed some input and accessed its message digest")
        {
            h.update("macrofree");
            h.update("-");

            auto md = h.digest();

            THEN ("we can add more input and get the whole result")
            {
                h.update("demo");
                h2.update("macrofree-demo"sv);

                REQUIRE(h.digest() == h2.digest());
            }

            AND_THEN ("the intermediate result was correct")
            {
                h2.update("macrofree-"sv);

                REQUIRE(md == h2.digest());
            }
        }
    }
}

TEST_CASE_TEMPLATE_APPLY(test_sha256_multiple, sha256_implementations);
