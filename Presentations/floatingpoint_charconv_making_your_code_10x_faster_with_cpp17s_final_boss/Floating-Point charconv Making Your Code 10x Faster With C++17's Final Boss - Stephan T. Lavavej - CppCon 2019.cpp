// cl /std:c++17 /EHsc /nologo /W4 /MT /O2 cppcon.cpp && cppcon
// clang-cl -m32 /std:c++17 /EHsc /nologo /W4 /MT /O2 cppcon.cpp && cppcon

// cl /std:c++17 /EHsc /nologo /W4 /MT /O2 cppcon.cpp && cppcon
// clang-cl -m64 /std:c++17 /EHsc /nologo /W4 /MT /O2 cppcon.cpp && cppcon

#ifndef _MSC_VER
    #define AVOID_CHARCONV
    #define AVOID_SPRINTF_S
#endif // _MSC_VER

#ifndef AVOID_CHARCONV
#include <charconv>
#endif // AVOID_CHARCONV
#include <chrono>
#include <random>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system_error>
#include <type_traits>
#include <vector>
using namespace std;
using namespace std::chrono;

void verify(const bool b) {
    if (!b) {
        puts("FAIL");
        exit(EXIT_FAILURE);
    }
}

enum class RoundTrip { Sci, Fix, Gen, Hex, Lossy };

constexpr size_t N = 2'000'000; // how many floating-point values to test

constexpr size_t K = 5; // how many times to repeat the test, for cleaner timing

constexpr size_t BufSize = 2'000; // more than enough

unsigned int global_dummy = 0;

template <typename Floating> int sprintf_wrapper(char (& buf)[BufSize], const char * const fmt, const Floating elem) {
#ifdef AVOID_SPRINTF_S
    return sprintf(buf, fmt, elem);
#else // AVOID_SPRINTF_S
    return sprintf_s(buf, BufSize, fmt, elem);
#endif // AVOID_SPRINTF_S
}

template <RoundTrip RT, typename Floating> void test_sprintf(const char * const str, const vector<Floating>& vec,
    const char * const fmt) {

    char buf[BufSize];

    const auto start = steady_clock::now();
    for (size_t k = 0; k < K; ++k) {
        for (const auto& elem : vec) {
            const int ret = sprintf_wrapper(buf, fmt, elem);

            global_dummy += static_cast<unsigned int>(ret);
            global_dummy += static_cast<unsigned int>(buf[0]);
        }
    }
    const auto finish = steady_clock::now();

    printf("%6.1f ns | %s\n", duration<double, nano>{finish - start}.count() / (N * K), str);

    for (const auto& elem : vec) {
        verify(sprintf_wrapper(buf, fmt, elem) != -1);

        if constexpr (RT == RoundTrip::Lossy) {
            // skip lossy conversions
        } else if constexpr (is_same_v<Floating, float>) {
            verify(strtof(buf, nullptr) == elem);
        } else {
            verify(strtod(buf, nullptr) == elem);
        }
    }
}

#ifndef AVOID_CHARCONV
constexpr chars_format chars_format_from_RoundTrip(const RoundTrip rt) {
    switch (rt) {
    case RoundTrip::Sci:
        return chars_format::scientific;
    case RoundTrip::Fix:
        return chars_format::fixed;
    case RoundTrip::Gen:
        return chars_format::general;
    case RoundTrip::Hex:
        return chars_format::hex;
    case RoundTrip::Lossy:
    default:
        puts("FAIL");
        exit(EXIT_FAILURE);
    }
}

template <RoundTrip RT, typename Floating, typename... Args> void test_to_chars(const char * const str,
    const vector<Floating>& vec, const Args&... args) {

    char buf[BufSize];

    const auto start = steady_clock::now();
    for (size_t k = 0; k < K; ++k) {
        for (const auto& elem : vec) {
            const auto result = to_chars(buf, buf + BufSize, elem, args...);

            global_dummy += static_cast<unsigned int>(result.ptr - buf);
            global_dummy += static_cast<unsigned int>(buf[0]);
        }
    }
    const auto finish = steady_clock::now();

    printf("%6.1f ns | %s\n", duration<double, nano>{finish - start}.count() / (N * K), str);

    for (const auto& elem : vec) {
        const auto result = to_chars(buf, buf + BufSize, elem, args...);
        verify(result.ec == errc{});

        if constexpr (RT == RoundTrip::Lossy) {
            // skip lossy conversions
        } else {
            Floating round_trip;
            const auto from_result = from_chars(buf, result.ptr, round_trip, chars_format_from_RoundTrip(RT));
            verify(from_result.ec == errc{});
            verify(from_result.ptr == result.ptr);
            verify(round_trip == elem);
        }
    }
}
#endif // AVOID_CHARCONV

template <RoundTrip RT, typename Floating> vector<char> prepare_strings(const vector<Floating>& vec) {
    vector<char> output;

    char buf[BufSize];

    for (const auto& elem : vec) {
        int ret;

        if constexpr (RT == RoundTrip::Sci) {
            if constexpr (is_same_v<Floating, float>) {
                ret = sprintf_wrapper(buf, "%.8e", elem);
            } else {
                ret = sprintf_wrapper(buf, "%.16e", elem);
            }
        } else {
            static_assert(RT == RoundTrip::Hex);
            if constexpr (is_same_v<Floating, float>) {
                ret = sprintf_wrapper(buf, "%.6a", elem);
            } else {
                ret = sprintf_wrapper(buf, "%.13a", elem);
            }
        }

        verify(ret != -1);

        output.insert(output.end(), buf, buf + ret + 1); // include null terminator
    }

    return output;
}

template <typename Floating> void test_strtox(const char * const str, const vector<Floating>& original,
    const vector<char>& strings) {

    vector<Floating> round_trip(N);

    const auto start = steady_clock::now();
    for (size_t k = 0; k < K; ++k) {
        const char * ptr = strings.data();
        char * endptr = nullptr;
        for (size_t n = 0; n < N; ++n) {
            if constexpr (is_same_v<Floating, float>) {
                round_trip[n] = strtof(ptr, &endptr);
            } else {
                round_trip[n] = strtod(ptr, &endptr);
            }

            ptr = endptr + 1; // advance past null terminator
        }
    }
    const auto finish = steady_clock::now();

    printf("%6.1f ns | %s\n", duration<double, nano>{finish - start}.count() / (N * K), str);

    verify(round_trip == original);
}

#ifndef AVOID_CHARCONV
vector<char> erase_0x(const vector<char>& strings) {
    vector<char> output;
    output.reserve(strings.size() - 2 * N);

    for (auto i = strings.begin(); i != strings.end(); ) {
        if (*i == '-') {
            output.push_back('-');
            i += 3; // advance past "-0x";
        } else {
            i += 2; // advance past "0x";
        }

        for (;;) {
            const char c = *i++;
            output.push_back(c);
            if (c == '\0') {
                break;
            }
        }
    }

    return output;
}

template <RoundTrip RT, typename Floating> void test_from_chars(const char * const str, const vector<Floating>& original,
    const vector<char>& strings) {

    const char * const last = strings.data() + strings.size();

    vector<Floating> round_trip(N);

    const auto start = steady_clock::now();
    for (size_t k = 0; k < K; ++k) {
        const char * first = strings.data();
        for (size_t n = 0; n < N; ++n) {
            const auto from_result = from_chars(first, last, round_trip[n], chars_format_from_RoundTrip(RT));
            first = from_result.ptr + 1; // advance past null terminator
        }
    }
    const auto finish = steady_clock::now();

    printf("%6.1f ns | %s\n", duration<double, nano>{finish - start}.count() / (N * K), str);

    verify(round_trip == original);
}
#endif // AVOID_CHARCONV

int main() {
#if defined(__clang__) && defined(_M_IX86)
    const char * const toolset = "Clang/LLVM x86 + MSVC STL";
#elif defined(__clang__) && defined(_M_X64)
    const char * const toolset = "Clang/LLVM x64 + MSVC STL";
#elif !defined(__clang__) && defined(_M_IX86)
    const char * const toolset = "C1XX/C2 x86 + MSVC STL";
#elif !defined(__clang__) && defined(_M_X64)
    const char * const toolset = "C1XX/C2 x64 + MSVC STL";
#else
    const char * const toolset = "Unknown Toolset";
#endif
    puts(toolset);


    vector<float> vec_flt;
    vector<double> vec_dbl;

    {
        mt19937_64 mt64;

        vec_flt.reserve(N);
        while (vec_flt.size() < N) {
            const uint32_t val = static_cast<uint32_t>(mt64());
            constexpr uint32_t inf_nan = 0x7F800000U;
            if ((val & inf_nan) == inf_nan) {
                continue; // skip INF/NAN
            }
            float flt;
            static_assert(sizeof(flt) == sizeof(val));
            memcpy(&flt, &val, sizeof(flt));
            vec_flt.push_back(flt);
        }

        vec_dbl.reserve(N);
        while (vec_dbl.size() < N) {
            const uint64_t val = mt64();
            constexpr uint64_t inf_nan = 0x7FF0000000000000ULL;
            if ((val & inf_nan) == inf_nan) {
                continue; // skip INF/NAN
            }
            double dbl;
            static_assert(sizeof(dbl) == sizeof(val));
            memcpy(&dbl, &val, sizeof(dbl));
            vec_dbl.push_back(dbl);
        }
    }

    test_sprintf<RoundTrip::Sci>("CRT float scientific 8", vec_flt, "%.8e");
    test_sprintf<RoundTrip::Sci>("CRT double scientific 16", vec_dbl, "%.16e");

    test_sprintf<RoundTrip::Lossy>("CRT float fixed 6 (lossy)", vec_flt, "%f");
    test_sprintf<RoundTrip::Lossy>("CRT double fixed 6 (lossy)", vec_dbl, "%f");

    test_sprintf<RoundTrip::Gen>("CRT float general 9", vec_flt, "%.9g");
    test_sprintf<RoundTrip::Gen>("CRT double general 17", vec_dbl, "%.17g");

    test_sprintf<RoundTrip::Hex>("CRT float hex 6", vec_flt, "%.6a");
    test_sprintf<RoundTrip::Hex>("CRT double hex 13", vec_dbl, "%.13a");

#ifndef AVOID_CHARCONV
    test_to_chars<RoundTrip::Gen>("STL float plain shortest", vec_flt);
    test_to_chars<RoundTrip::Gen>("STL double plain shortest", vec_dbl);

    test_to_chars<RoundTrip::Sci>("STL float scientific shortest", vec_flt, chars_format::scientific);
    test_to_chars<RoundTrip::Sci>("STL double scientific shortest", vec_dbl, chars_format::scientific);

    test_to_chars<RoundTrip::Fix>("STL float fixed shortest", vec_flt, chars_format::fixed);
    test_to_chars<RoundTrip::Fix>("STL double fixed shortest", vec_dbl, chars_format::fixed);

    test_to_chars<RoundTrip::Gen>("STL float general shortest", vec_flt, chars_format::general);
    test_to_chars<RoundTrip::Gen>("STL double general shortest", vec_dbl, chars_format::general);

    test_to_chars<RoundTrip::Hex>("STL float hex shortest", vec_flt, chars_format::hex);
    test_to_chars<RoundTrip::Hex>("STL double hex shortest", vec_dbl, chars_format::hex);

    test_to_chars<RoundTrip::Sci>("STL float scientific 8", vec_flt, chars_format::scientific, 8);
    test_to_chars<RoundTrip::Sci>("STL double scientific 16", vec_dbl, chars_format::scientific, 16);

    test_to_chars<RoundTrip::Lossy>("STL float fixed 6 (lossy)", vec_flt, chars_format::fixed, 6);
    test_to_chars<RoundTrip::Lossy>("STL double fixed 6 (lossy)", vec_dbl, chars_format::fixed, 6);

    test_to_chars<RoundTrip::Gen>("STL float general 9", vec_flt, chars_format::general, 9);
    test_to_chars<RoundTrip::Gen>("STL double general 17", vec_dbl, chars_format::general, 17);

    test_to_chars<RoundTrip::Hex>("STL float hex 6", vec_flt, chars_format::hex, 6);
    test_to_chars<RoundTrip::Hex>("STL double hex 13", vec_dbl, chars_format::hex, 13);
#endif // AVOID_CHARCONV

    puts("----------");

    const vector<char> strings_sci_flt = prepare_strings<RoundTrip::Sci>(vec_flt);
    const vector<char> strings_sci_dbl = prepare_strings<RoundTrip::Sci>(vec_dbl);

    const vector<char> strings_hex_flt = prepare_strings<RoundTrip::Hex>(vec_flt);
    const vector<char> strings_hex_dbl = prepare_strings<RoundTrip::Hex>(vec_dbl);

    test_strtox("CRT strtof float scientific", vec_flt, strings_sci_flt);
    test_strtox("CRT strtod double scientific", vec_dbl, strings_sci_dbl);

    test_strtox("CRT strtof float hex", vec_flt, strings_hex_flt);
    test_strtox("CRT strtod double hex", vec_dbl, strings_hex_dbl);

#ifndef AVOID_CHARCONV
    test_from_chars<RoundTrip::Sci>("STL from_chars float scientific", vec_flt, strings_sci_flt);
    test_from_chars<RoundTrip::Sci>("STL from_chars double scientific", vec_dbl, strings_sci_dbl);

    test_from_chars<RoundTrip::Hex>("STL from_chars float hex", vec_flt, erase_0x(strings_hex_flt));
    test_from_chars<RoundTrip::Hex>("STL from_chars double hex", vec_dbl, erase_0x(strings_hex_dbl));
#endif // AVOID_CHARCONV

    printf("global_dummy: %u\n", global_dummy);
}
