cmake_minimum_required(VERSION 3.25)
project(oop_cw CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (MSVC)
    add_compile_options("/Za")
endif ()


add_library(
        interface STATIC
        interface/src/destructors.cpp
)
target_include_directories(
        interface PUBLIC
        interface/include/
)


add_library(
        fun_expression_parser STATIC
        fun_expression_parser/src/expressions.hpp
        fun_expression_parser/src/implementation.cpp
)
target_link_libraries(
        fun_expression_parser PUBLIC interface
)
target_include_directories(
        fun_expression_parser PUBLIC
        fun_expression_parser/include/
)

add_library(
        default_application_logic STATIC
        default_application_logic/src/implementation.cpp
)
target_link_libraries(
        default_application_logic PUBLIC interface
)
target_include_directories(
        default_application_logic PUBLIC
        default_application_logic/include/
)

add_executable(
        rc rc.cpp
)

find_package(Python "3.10" EXACT REQUIRED COMPONENTS Development.Embed)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/widgets_py.cpp
        COMMAND rc ARGS "\"\\\"${CMAKE_SOURCE_DIR}/demo/src/widgets_py.hpp\"\\\"" "LdH::Studying::BGTU::OOP::Demo::widgets_py_source" demo/src/widgets.py "${CMAKE_BINARY_DIR}/widgets_py.cpp"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        MAIN_DEPENDENCY demo/src/widgets.py
        DEPENDS rc
        COMMENT "Embedding 'widgets.py'"
)

add_executable(
        demo
        # demo/src/py_utilities.hpp
        # demo/src/py_utilities.cpp
        demo/src/canvas.cpp
        demo/src/canvas.hpp
        demo/src/widgets_py.hpp
        "${CMAKE_BINARY_DIR}/widgets_py.cpp"
        demo/src/callback_wrappers.hpp
        demo/src/callback_wrappers.cpp
        demo/src/main.cpp
)
target_link_libraries(
        demo PRIVATE Python::Python default_application_logic fun_expression_parser interface
)

if (MSVC)
    target_compile_definitions(demo PRIVATE Py_PYTIME_H)
endif ()