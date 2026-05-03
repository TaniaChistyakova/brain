cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(brain LANGUAGES CXX)


file(GLOB brain_src
    lib/*.h
    lib/*.cpp
)

set(NATVIS)
if(MSVC)
	set(NATVIS "brain.natvis")
endif()

add_library(brain_lib STATIC ${brain_src})

add_executable(brain_exe brain.cpp ${NATVIS})
target_link_libraries(brain_exe brain_lib)
target_include_directories(brain_exe PRIVATE lib)

find_package(GTest)
if(NOT ${GTest_FOUND})
	#Download Gtest
	include(FetchContent)
	FetchContent_Declare(
	  googletest
	  GIT_REPOSITORY https://github.com/google/googletest.git
	  GIT_TAG        v1.17.0
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
	set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endif()

file(GLOB GTest_src
    test/*.h
    test/*.cpp
)

add_executable(brain_test ${GTest_src} ${NATVIS})
target_link_libraries(brain_test PRIVATE GTest::gtest GTest::gtest_main brain_lib)
target_include_directories(brain_test PRIVATE lib)

enable_testing()

include(GoogleTest)
gtest_discover_tests(brain_test)


find_package(LATEX COMPONENTS PDFLATEX BIBTEX )
if(${LATEX_FOUND})
	add_subdirectory(paper)
endif()
