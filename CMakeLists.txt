cmake_minimum_required(VERSION 3.14)
project(Minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
  GIT_TAG        main
)
set(FTXUI_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(FTXUI_BUILD_DOCS OFF CACHE INTERNAL "")
set(FTXUI_BUILD_TESTS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(ftxui)

add_executable(minesweeper
  src/main.cpp
  src/board.cpp
)

if(NOT MSVC)
  target_compile_options(minesweeper PRIVATE -ffunction-sections -fdata-sections)
  target_link_options(minesweeper PRIVATE -Wl,--gc-sections)
endif()

target_link_libraries(minesweeper PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
)

target_include_directories(minesweeper PRIVATE include)
