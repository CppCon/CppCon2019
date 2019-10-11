# macrofree-demo

This project demos the material covered in the CppCon 2019 talk "Are We Macro-free Yet?" The project includes:

- two ways of implementing a function with platform-dependent APIs, and
- how to implement a sha256 class with multiple, conditionally available backends, coexist at runtime

without any use of `#if`, `#ifdef`, or `#define`.

## Getting Start

This demo maintains build tools and third-party libraries with Anaconda.  Please go to [here](https://docs.conda.io/en/latest/miniconda.html), download and install the latest version on your platform.

After the `conda` command is available in your shell, change the working directory to the project root directory and execute:

```shell
conda env create -f environment.yml
conda activate macrofree-demo
```

The rest of the article assumes that you are in the "macrofree-demo" Conda environment.

### Linux

The build requires gcc>=7.0 or clang>=4.0.

Build and run the tests:

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug .
ninja -C build
./build/tests/run
```

The Conda environment has OpenSSL under Linux.  You can verify that the test suite is testing a single `sha256_openssl` implementation with the following command:

```
./build/tests/run -ltc
```

"-ltc" (list test cases) is a command-line option provided by [doctest](https://github.com/onqtam/doctest).

### Windows

The build requires Visual Studio 2019.

In "Native Tools Command Prompt":

```
cmake -B build .
msbuild build\macrofree_demo.sln
build\tests\Debug\run.exe
```

Alternatively, you can specify `-DCMAKE_CXX_COMPILER=cl.exe` in CMake command-line and keep using Ninja.

Usually, only [CNG](https://docs.microsoft.com/en-us/windows/win32/seccng/cng-portal) (Cryptography API: Next Generation) implementation is available at this point.  If you install OpenSSL with

```shell
conda install -c conda-forge openssl
```

reconfigure and rebuild

```shell
cmake build
msbuild build\macrofree_demo.sln
```

You will find that the same test case runs against both `sha256_cng` and `sha256_openssl` implementations.
