set(CPACK_PACKAGE_CONTACT Torvus)
set(CPACK_BINARY_DEB on)
set(CPACK_SYSTEM_NAME amd64)
include(CPack)
project(pgmanager)
option(BUILD_TESTS "building tests")
cmake_minimum_required(VERSION 2.8)
enable_testing()
add_test(NAME GeneralTests
         COMMAND ${EXECUTABLE_OUTPUT_PATH}/tests/tests)
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
set(pgmanager_SRCS src/main.c
                   src/pgmcellrendererspin.c
                   src/pgmcellrenderertext.c
                   src/pgmcellrenderertoggle.c
                   src/pgmcheckbutton.c
                   src/pgmdatabase.c
                   src/pgmdatabaselistmodel.c
                   src/pgmdatastore.c
                   src/pgmdataview.c
                   src/pgmfunction.c
                   src/pgmfunctionlist.c
                   src/pgmfunctionview.c
                   src/pgmlanguage.c
                   src/pgmlanguagelist.c
                   src/pgmlanguagelistmodel.c
                   src/pgmmainwindow.c
                   src/pgmmarshal.c
                   src/pgmobject.c
                   src/pgmpreferencesdialog.c
                   src/pgmquery.c
                   src/pgmqueryeditor.c
                   src/pgmschema.c
                   src/pgmschemalist.c
                   src/pgmschemalistmodel.c
                   src/pgmserver.c
                   src/pgmserverlist.c
                   src/pgmsqleditor.c
                   src/pgmsqleditorpreference.c
                   src/pgmstock.c
                   src/pgmtable.c
                   src/pgmtablelist.c
                   src/pgmtableview.c
                   src/pgmtoolcombobox.c
                   src/pgmtype.c)
set(pgmanager_HDRS src/license.h
                   src/pgmcellrendererspin.h
                   src/pgmcellrenderertext.h
                   src/pgmcellrenderertoggle.h
                   src/pgmcheckbutton.h
                   src/pgmdatabase.h
                   src/pgmdatabaselistmodel.h
                   src/pgmdatastore.h
                   src/pgmdataview.h
                   src/pgmfunction.h
                   src/pgmfunctionlist.h
                   src/pgmfunctionview.h
                   src/pgmlanguage.h
                   src/pgmlanguagelist.h
                   src/pgmlanguagelistmodel.h
                   src/pgmmainwindow.h
                   src/pgmmarshal.h
                   src/pgmobject.h
                   src/pgmpreferencesdialog.h
                   src/pgmqueryeditor.h
                   src/pgmquery.h
                   src/pgmschema.h
                   src/pgmschemalist.h
                   src/pgmschemalistmodel.h
                   src/pgmserver.h
                   src/pgmserverlist.h
                   src/pgmsqleditor.h
                   src/pgmstock.h
                   src/pgmtable.h
                   src/pgmtablelist.h
                   src/pgmtableview.h
                   src/pgmtoolcombobox.h
                   src/pgmtranslate.h
                   src/pgmtype.h)
add_executable(pgmanager ${pgmanager_SRCS} ${pgmanager_HDRS})
configure_file(config.h.in config.h)
target_link_libraries(pgmanager ${GTK2_LIBRARIES}
                                ${PQ_LIBRARIES}
                                ${GTK_SOURCE_VIEW2_LIBRARIES}
                                ${GCONF2_LIBRARIES}
                                ${ENCA_LIBRARIES})
install(TARGETS pgmanager DESTINATION bin)
if(BUILD_TESTS)
    add_subdirectory(tests tests)
endif()
message( STATUS "CMAKE_PROJECT_NAME: " ${CMAKE_PROJECT_NAME} )
