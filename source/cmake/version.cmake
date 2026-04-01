#  version.cmake - cmake script to generate git version information 
# 
# Copyright (c) 2024 Frank Vanbever
# With modifications by Igor Mikolic-Torreira (2026)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_SHA1_SHORT
  ERROR_QUIET
)

if( "${GIT_SHA1_SHORT}" STREQUAL "" )
  set( GIT_SHA1_SHORT "N/A" )
  set( GIT_SHA1_FULL "N/A" )
  set( GIT_DIRTY "N/A" )
  set( GIT_DIRTY_VAL "0" )
  set( GIT_TAG "N/A" )
  set( GIT_BRANCH "N/A" )
else()
  execute_process(
    COMMAND bash -c "git rev-parse HEAD"
    OUTPUT_VARIABLE GIT_SHA1_FULL
  )
  execute_process(
    COMMAND bash -c "git diff --quiet --exit-code || echo dirty"
    OUTPUT_VARIABLE GIT_DIRTY
  )
  execute_process(
    COMMAND bash -c "git diff --quiet --exit-code && echo 0 || echo 1"
    OUTPUT_VARIABLE GIT_DIRTY_VAL
  )
  execute_process(
    COMMAND git describe --exact-match --tags 
    OUTPUT_VARIABLE GIT_TAG 
    ERROR_QUIET
  )
  execute_process(
   COMMAND git rev-parse --abbrev-ref HEAD 
   OUTPUT_VARIABLE GIT_BRANCH
  )

  set( WARNING_FOR_GEN_FILES "DO NOT EDIT -- this file is generated during the build process" )

  string( STRIP "${GIT_SHA1_SHORT}" GIT_SHA1_SHORT )
  string( STRIP "${GIT_SHA1_FULL}" GIT_SHA1_FULL )
  string( STRIP "${GIT_DIRTY}" GIT_DIRTY )
  string( STRIP "${GIT_DIRTY_VAL}" GIT_DIRTY_VAL )
  string( STRIP "${GIT_TAG}" GIT_TAG )
  string (STRIP "${GIT_BRANCH}" GIT_BRANCH )
endif()

message( "Build Git HEAD is ${GIT_SHA1_SHORT}" )
if( GIT_DIRTY_VAL )
  message( "WARNING: Build is DIRTY" )
endif()

configure_file(
  "${SRC_DIR}/GitHeadInfo.h.in"
  "${BIN_DIR}/GitHeadInfo.h"
)
