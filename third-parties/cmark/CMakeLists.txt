# -*- CMake -*-

####################################################################################################

set(CMARK_SOURCE_PATH "cmark.git/src")

####################################################################################################

# cmake_minimum_required(VERSION 2.8.9)

# # prevent ugly developer warnings because version is set directly, not through project()
# # it should be redone properly by using VERSION in project() if on CMake 3.x
# if(CMAKE_MAJOR_VERSION GREATER 2)
#     cmake_policy(SET CMP0048 OLD)
# endif()

# project(cmark)

# include("FindAsan.cmake")

# if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
#     message(FATAL_ERROR "Do not build in-source.\nPlease remove CMakeCache.txt and the CMakeFiles/ directory.\nThen: mkdir build ; cd build ; cmake .. ; make")
# endif()

# set(PROJECT_NAME "cmark")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 27)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )

# option(CMARK_TESTS "Build cmark tests and enable testing" ON)
# option(CMARK_STATIC "Build static libcmark library" ON)
# option(CMARK_SHARED "Build shared libcmark library" ON)

# add_subdirectory(src)
# add_subdirectory(extensions)
# if(CMARK_TESTS AND CMARK_SHARED)
#   add_subdirectory(api_test)
# endif()
# add_subdirectory(man)
# if(CMARK_TESTS)
#   enable_testing()
#   add_subdirectory(test testdir)
# endif()

# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE "Release" CACHE STRING
#   "Choose the type of build, options are: Debug Profile Release Asan Ubsan." FORCE)
# endif(NOT CMAKE_BUILD_TYPE)

####################################################################################################

# if(${CMAKE_VERSION} VERSION_GREATER "3.3")
#     cmake_policy(SET CMP0063 NEW)
# endif()

# set(LIBRARY "libcmark")
# set(STATICLIBRARY "libcmark_static")
set(HEADERS
  cmark.h
  cmark_extension_api.h
  parser.h
  buffer.h
  node.h
  iterator.h
  chunk.h
  references.h
  utf8.h
  scanners.h
  inlines.h
  houdini.h
  cmark_ctype.h
  render.h
  registry.h
  syntax_extension.h
  plugin.h
  )
set(LIBRARY_SOURCES
  cmark.c
  node.c
  iterator.c
  blocks.c
  inlines.c
  scanners.c
  scanners.re
  utf8.c
  buffer.c
  references.c
  render.c
  man.c
  xml.c
  html.c
  commonmark.c
  latex.c
  houdini_href_e.c
  houdini_html_e.c
  houdini_html_u.c
  cmark_ctype.c
  arena.c
  linked_list.c
  syntax_extension.c
  registry.c
  plugin.c
  ${HEADERS}
  )

# set(PROGRAM "cmark")
# set(PROGRAM_SOURCES
#   main.c
#   )

# include_directories(. ${CMAKE_CURRENT_BINARY_DIR})
# include_directories(
#   ${PROJECT_BINARY_DIR}/extensions
# )

include_directories(${CMARK_SOURCE_PATH})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMARK_SOURCE_PATH}/cmark_version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmark_version.h)

include(GenerateExportHeader)

# add_executable(${PROGRAM} ${PROGRAM_SOURCES})
# add_compiler_export_flags()

# target_link_libraries(${PROGRAM} libcmark_static)

# add_dependencies(${PROGRAM} libcmarkextensions_static)
# target_link_libraries(${PROGRAM} libcmarkextensions_static)

# # Disable the PUBLIC declarations when compiling the executable:
# set_target_properties(${PROGRAM} PROPERTIES
#   COMPILE_FLAGS "-DCMARK_STATIC_DEFINE -DCMARKEXTENSIONS_STATIC_DEFINE")

# # Check integrity of node structure when compiled as debug:
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCMARK_DEBUG_NODES -DDEBUG")
# set(CMAKE_LINKER_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}")

# set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE} -pg")
# set(CMAKE_LINKER_PROFILE "${CMAKE_LINKER_FLAGS_RELEASE} -pg")

# add_definitions(-DLIBDIR=\"${CMAKE_BINARY_DIR}\")

if (${CMAKE_VERSION} VERSION_GREATER "1.8")
  set(CMAKE_C_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
elseif(CMAKE_COMPILER_IS_GNUCC OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif ()

# if (CMARK_SHARED)
#   add_library(${LIBRARY} SHARED ${LIBRARY_SOURCES})
#   # Include minor version and patch level in soname for now.
#   set_target_properties(${LIBRARY} PROPERTIES
#   OUTPUT_NAME "cmark"
#   SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
#   VERSION ${PROJECT_VERSION})

#   set_property(TARGET ${LIBRARY}
#     APPEND PROPERTY MACOSX_RPATH true)

#   # Avoid name clash between PROGRAM and LIBRARY pdb files.
#   set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark_dll)

#   generate_export_header(${LIBRARY}
#     BASE_NAME ${PROJECT_NAME})

#   list(APPEND CMARK_INSTALL ${LIBRARY})
# endif()

# if (CMARK_STATIC)
#   add_library(${STATICLIBRARY} STATIC ${LIBRARY_SOURCES})
#   set_target_properties(${STATICLIBRARY} PROPERTIES
#   COMPILE_FLAGS -DCMARK_STATIC_DEFINE
#   POSITION_INDEPENDENT_CODE ON)

#   if (MSVC)
#   set_target_properties(${STATICLIBRARY} PROPERTIES
#     OUTPUT_NAME "cmark_static"
#     VERSION ${PROJECT_VERSION})
#   else()
#   set_target_properties(${STATICLIBRARY} PROPERTIES
#     OUTPUT_NAME "cmark"
#     VERSION ${PROJECT_VERSION})
# endif(MSVC)

# target_link_libraries(cmark ${CMAKE_DL_LIBS})

#   list(APPEND CMARK_INSTALL ${STATICLIBRARY})
# endif()
# set_property(TARGET ${LIBRARY}
#   APPEND PROPERTY MACOSX_RPATH true)

# # Avoid name clash between PROGRAM and LIBRARY pdb files.
# set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark_dll)

# generate_export_header(${LIBRARY}
#     BASE_NAME ${PROJECT_NAME})

# if (MSVC)
#   set_property(TARGET ${PROGRAM}
#     APPEND PROPERTY LINK_FLAGS /INCREMENTAL:NO)
# endif(MSVC)

# set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)

# include (InstallRequiredSystemLibraries)
# install(TARGETS ${PROGRAM} ${CMARK_INSTALL}
#   EXPORT cmark
#   RUNTIME DESTINATION bin
#   LIBRARY DESTINATION lib${LIB_SUFFIX}
#   ARCHIVE DESTINATION lib${LIB_SUFFIX}
#   )

# if(CMARK_SHARED OR CMARK_STATIC)
#   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcmark.pc.in
#     ${CMAKE_CURRENT_BINARY_DIR}/libcmark.pc @ONLY)
#   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcmark.pc
#     DESTINATION lib${LIB_SUFFIX}/pkgconfig)

#   install(FILES
#   cmark.h
#   cmark_extension_api.h
#   ${CMAKE_CURRENT_BINARY_DIR}/cmark_export.h
#   ${CMAKE_CURRENT_BINARY_DIR}/cmark_version.h
#   DESTINATION include
#   )

#   install(EXPORT cmark DESTINATION lib${LIB_SUFFIX}/cmake)
# endif()

# Feature tests
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckSymbolExists)
CHECK_INCLUDE_FILE(stdbool.h HAVE_STDBOOL_H)
CHECK_C_SOURCE_COMPILES(
  "int main() { __builtin_expect(0,0); return 0; }"
  HAVE___BUILTIN_EXPECT)
CHECK_C_SOURCE_COMPILES("
  int f(void) __attribute__ (());
  int main() { return 0; }
" HAVE___ATTRIBUTE__)

CONFIGURE_FILE(
  ${CMARK_SOURCE_PATH}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# # Always compile with warnings
# if(MSVC)
#   # Force to always compile with W4
#   if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
#     string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#   else()
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
#   endif()
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /wd4706 /wd4204 /wd4221 /wd4100 /D_CRT_SECURE_NO_WARNINGS")
# elseif(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -std=c99 -pedantic")
# endif()

# # Compile as C++ under MSVC older than 12.0
# if(MSVC AND MSVC_VERSION LESS 1800)
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
# endif()

# if(CMAKE_BUILD_TYPE STREQUAL "Ubsan")
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
# endif()

####################################################################################################

PREPEND(CMARK_SOURCES ${CMARK_SOURCE_PATH} ${LIBRARY_SOURCES})
PREPEND(CMARK_HEADERS ${CMARK_SOURCE_PATH} ${HEADERS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -std=c99 -pedantic")

add_library(cmark
  SHARED
  ${CMARK_SOURCES}
  ${CMARK_HEADERS}
  )

generate_export_header(cmark
  BASE_NAME cmark)

####################################################################################################
#
# End
#
####################################################################################################