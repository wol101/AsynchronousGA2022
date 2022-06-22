cmake_minimum_required(VERSION 3.5)

project(AsynchronousGA2022 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AsynchronousGA2022
    ../src/ArgParse.cpp
    ../src/DataFile.cpp
    ../src/GAASIO.cpp
    ../src/Genome.cpp
    ../src/MD5.cpp
    ../src/Mating.cpp
    ../src/Population.cpp
    ../src/Preferences.cpp
    ../src/Random.cpp
    ../src/ServerASIO.cpp
    ../src/Statistics.cpp
    ../src/XMLConverter.cpp
    ../pystring/pystring.cpp
    ../src/ArgParse.h
    ../src/DataFile.h
    ../src/GAASIO.h
    ../src/Genome.h
    ../src/MD5.h
    ../src/Mating.h
    ../src/Population.h
    ../src/Preferences.h
    ../src/Random.h
    ../src/ServerASIO.h
    ../src/Statistics.h
    ../src/XMLConverter.h
    ../asio-1.18.2/include/asio.hpp
    ../exprtk/exprtk.hpp
    ../pystring/pystring.h
    ../rapidxml-1.13/rapidxml.hpp
    ../rapidxml-1.13/rapidxml_iterators.hpp
    ../rapidxml-1.13/rapidxml_print.hpp
    ../rapidxml-1.13/rapidxml_utils.hpp
)

target_include_directories(AsynchronousGA2022 PRIVATE
    ../src
    ../asio-1.18.2/include
    ../exprtk
    ../pystring
    ../rapidxml-1.13
)

if (MSVC)
    target_compile_options(AsynchronousGA2022 PRIVATE /bigobj /MT)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS _WIN32_WINNT=_WIN32_WINNT_WIN7)
endif ()

