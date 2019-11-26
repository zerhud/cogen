cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ix3 VERSION 0.0.0.0 LANGUAGES C CXX)

option(enable_tests "enable tests" ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(Boost 1.69 REQUIRED)

add_library(${PROJECT_NAME} SHARED parse.hpp parser.cpp parser.hpp)
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
target_include_directories(
	${PROJECT_NAME}
	PRIVATE ${Boost_INCLUDE_DIRS}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
	)

target_sources(${PROJECT_NAME} PRIVATE ast/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE ast/meta.cpp ast/meta.hpp)
target_sources(${PROJECT_NAME} PRIVATE ast/functions.hpp)
target_sources(${PROJECT_NAME} PRIVATE ast/entity.hpp)
target_sources(${PROJECT_NAME} PRIVATE ast/file.hpp)

target_sources(${PROJECT_NAME} PRIVATE fusion/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE fusion/meta.hpp)
target_sources(${PROJECT_NAME} PRIVATE fusion/functions.hpp)
target_sources(${PROJECT_NAME} PRIVATE fusion/entity.hpp)
target_sources(${PROJECT_NAME} PRIVATE fusion/file.hpp)

target_sources(${PROJECT_NAME} PRIVATE grammar/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar/meta.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar/functions.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar/entity.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar/file.hpp)

target_sources(${PROJECT_NAME} PRIVATE operators/common.cpp operators/common.hpp)

target_sources(${PROJECT_NAME} PRIVATE grammar_def/common.cpp    grammar_def/common.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar_def/meta.cpp      grammar_def/meta.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar_def/functions.cpp grammar_def/functions.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar_def/entity.cpp    grammar_def/entity.hpp)
target_sources(${PROJECT_NAME} PRIVATE grammar_def/file.cpp      grammar_def/file.hpp)

# utils
target_sources(${PROJECT_NAME} PRIVATE utils/traverser.cpp     utils/traverser.hpp)
target_sources(${PROJECT_NAME} PRIVATE utils/evaluator.cpp     utils/evaluator.hpp)
target_sources(${PROJECT_NAME} PRIVATE utils/checker.cpp       utils/checker.hpp)
target_sources(${PROJECT_NAME} PRIVATE utils/meta.cpp          utils/meta.hpp)
target_sources(${PROJECT_NAME} PRIVATE utils/selector.cpp      utils/selector.hpp)
target_sources(${PROJECT_NAME} PRIVATE utils/remove_empty.cpp  utils/remove_empty.hpp)


if(enable_tests)
	include(cmake/tests.inc.cmake)
endif()
