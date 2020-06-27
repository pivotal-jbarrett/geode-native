from conans import ConanFile, CMake, tools


class ApacheGeodeNative(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "boost/1.73.0", "xerces-c/3.2.3"
    build_requires = "gtest/1.10.0", "benchmark/1.5.0", "sqlite3/3.32.3", "doxygen/1.8.17"
    generators = "cmake_find_package_multi", "cmake_paths"
    default_options = {"boost:without_test": True}
