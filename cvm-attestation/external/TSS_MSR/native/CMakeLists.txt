cmake_minimum_required(VERSION 3.15...3.27)
project(tbs_native LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "tbs_native wraps the Windows TBS API and can only be built on Windows.")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Locate the active Python interpreter and its development components.
find_package(Python 3.8 COMPONENTS Interpreter Development.Module REQUIRED)

# Locate the nanobind CMake helpers (installed as a Python package).
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(tbs_native STABLE_ABI tbs_native.cpp)

# Link against the Windows TPM Base Services import library (ships with the
# Windows SDK).
target_link_libraries(tbs_native PRIVATE Tbs)

# Install the compiled module at the wheel root so that it is importable as
# a top-level "tbs_native" module.
install(TARGETS tbs_native LIBRARY DESTINATION .)
