cmake_minimum_required(VERSION 3.20)
project(hyprwallpaper)

set(CMAKE_CXX_STANDARD 20)

set(ENV{PKG_CONFIG_PATH} "/usr/lib64/pkgconfig:/usr/share/pkgconfig")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
add_compile_definitions(ROOT="${CMAKE_SOURCE_DIR}")

add_executable(hyprwallpaper main.cpp)

target_include_directories(hyprwallpaper PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(hyprwallpaper PRIVATE ${GTKMM_LIBRARIES})
