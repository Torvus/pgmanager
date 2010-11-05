project(tests)
cmake_minimum_required(VERSION 2.8)
find_package(GTK2 REQUIRED gtk)
set(PQ_DIR ./)
find_package(PQ REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK_SOURCE_VIEW2 REQUIRED gtksourceview-2.0)
pkg_check_modules(GCONF2 REQUIRED gconf-2.0)
pkg_check_modules(ENCA REQUIRED enca)
include_directories(${GTK2_INCLUDE_DIRS}
                    ${PQ_INCLUDE_DIRS}
                    ${GTK_SOURCE_VIEW2_INCLUDE_DIRS}
                    ${GCONF2_INCLUDE_DIRS}
                    ${ENCA_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR on )
set(tests_SRCS main.c
               pgmserver.c
               tests.c)
set(tests_HDRS tests.h)
set(tests_NEED_SRCS)
foreach(PG_MANAGER_SRC ${pgmanager_SRCS})
    if(NOT (${PG_MANAGER_SRC} STREQUAL "src/main.c"))
        set(tests_NEED_SRCS ${tests_NEED_SRCS} "../${PG_MANAGER_SRC}")
    endif()
endforeach(PG_MANAGER_SRC)
configure_file(../config.h.in config.h)
add_executable(tests ${tests_SRCS} ${tests_HDRS} ${tests_NEED_SRCS})
target_link_libraries(tests ${GTK2_LIBRARIES}
                            ${PQ_LIBRARIES}
                            ${GTK_SOURCE_VIEW2_LIBRARIES}
                            ${GCONF2_LIBRARIES}
                            ${ENCA_LIBRARIES}
                            cunit)

