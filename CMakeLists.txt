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
FetchContent_MakeAvailable(ftxui)

add_executable(minesweeper src/main.cpp)

target_link_libraries(minesweeper PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
)

target_include_directories(minesweeper PRIVATE
  src
  include
)
