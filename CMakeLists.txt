cmake_minimum_required(VERSION 3.23.2)
project(password_manager VERSION 0.01 LANGUAGES C)

include(FetchContent)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Declare our target. We want the lastest stable version, not the master.
# Also specify GIT_SHALLOW to avoid cloning branch we don't care about
FetchContent_Declare(
  cmocka
  GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
  GIT_TAG        cmocka-1.1.5
  GIT_SHALLOW    1
)

# We want to link to cmocka-static, so we need to set this option before calling the FetchContent_MakeAvailable
# We also don't care about example and tests
set(BUILD_SHARED_LIBS OFF CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)

# Download cmocka, and execute its cmakelists.txt
FetchContent_MakeAvailable(cmocka)
enable_testing()

add_subdirectory(df_psm)
add_subdirectory(app)
