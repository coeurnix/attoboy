cmake_minimum_required(VERSION 3.15)
project(attoboy-workspace VERSION 0.0.3 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Enable testing at the root level for CTest
# ------------------------------------------------------------------------------
enable_testing()

# ------------------------------------------------------------------------------
# Build the libattoboy subproject
# ------------------------------------------------------------------------------
add_subdirectory(libattoboy)
