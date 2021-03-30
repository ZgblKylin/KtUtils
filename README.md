# KtUtils
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-MPL-green.svg)](https://www.mozilla.org/en-US/MPL/)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/a70beb37999b4a6298de4e4d3b3510d8)](https://www.codacy.com/gh/ZgblKylin/KtUtils/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ZgblKylin/KtUtils&amp;utm_campaign=Badge_Grade)

Qt Utilities by ZgblKylin.

## Build

This project is designed to be integrate as a sub_directory.

### Configure

Two CMake built-in options are used:

- `BUILD_SHARED_LIBS`: `OFF` by default, enable to build shared libraries.
- `BUILD_TESTING`: `OFF` by default, enable to build tests.

### Compile

Just add these codes into your `CMakeLists.txt`

```cmake
add_subdirectory(KtUtils)
target_link_library(${PROJECT_NAME} PUBLIC KtUtils)
```

Then it'll export precompile header `<KtUtils/global>` to your project. Feel free to use everything without manually include header files!
