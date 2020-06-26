from conans import ConanFile, CMake, tools


class ApacheGeodeNative(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "gtest/1.10.0", "benchmark/1.5.0"
    generators = "cmake_find_package_multi", "cmake_paths"
