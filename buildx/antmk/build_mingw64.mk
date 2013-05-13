# Copyright 2013 Cheolmin Jo (webos21@gmail.com)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

######################################################
#                        XI                          #
#----------------------------------------------------#
# File    : build.mingw.properties                   #
# Version : 0.1.0                                    #
# Desc    : properties file for MinGW build.         #
#----------------------------------------------------#
# History)                                           #
#   - 2011/06/15 : Created by cmjo                   #
######################################################


########################
# Programs
########################
include ${basedir}/buildx/antmk/shprog.mk


########################
# Build Configuration
########################
build_cfg_target  = mingw64
build_cfg_mingw   = 1
build_cfg_posix   = 1


########################
# Directories
########################
build_tool_dir     = 
#${env.windir}/Microsoft.NET/Framework/v3.5


########################
# Program Definition
########################
build_tool_cc      = ${build_tool_dir}gcc
build_tool_cxx     = ${build_tool_dir}g++
build_tool_linker  = ${build_tool_dir}g++
build_tool_ar      = ${build_tool_dir}ar
build_tool_ranlib  = ${build_tool_dir}ranlib
build_tool_msbuild = ${build_tool_dir_ms}/msbuild.exe


########################
# Compile Flags
########################
#build_run_a        = 1
build_run_so       = 1
build_run_test     = 1

build_opt_a_pre    = alib
build_opt_a_ext    = a
build_opt_so_pre   = 
build_opt_so_ext   = dll
build_opt_exe_ext  = .exe

build_opt_c        = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx      = -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_ld       = -g -Wl,--no-undefined
build_opt_ld_so    = -shared -Wl,-soname,
build_opt_ld_rpath = -Wl,-rpath-link,
build_opt_ld_noud  = -Wl,--no-undefined

build_opt_fPIC     =
build_opt_mnocyg   =
build_opt_libgcc   = -static-libgcc
build_opt_libgxx   = -static-libstdc++
build_opt_libexe   =


########################
# Compile Target : XI
########################

build_xibase_src_bin    =
build_xibase_src_mk     = $(wildcard $(basedir)/src/base/src/_all/*.c)
build_xibase_src_mk    += $(wildcard $(basedir)/src/base/src/win32/*.c)
build_xibase_src_in     = _all/*.c, win32/*.c
build_xibase_src_ex     = 
build_xibase_inc_dir    = -I${basedir}/include
build_xibase_lib_dir    = 
build_xibase_lib_mod    = -lws2_32 -lmswsock -luserenv
build_xibase_dlldef     = ${basedir}/src/base/xibase.def

buildtc_xibase_src_bin  = tc_main.c
buildtc_xibase_src_mk     = $(wildcard $(basedir)/src/base/test/*.c)
buildtc_xibase_src_in   = *.c
buildtc_xibase_src_ex   = tc_main.c
buildtc_xibase_inc_dir  = -I${basedir}/include
buildtc_xibase_lib_dir  = 
buildtc_xibase_lib_mod  = -lxibase
buildtc_xibase_dlldef   = ${basedir}/src/base/xibasetest.def


########################
# Compile Target : Ext
########################

build_ext_zlib_dlldef    = ${basedir}/external/zlib/zlib.def

build_ext_ffi_dlldef     = ${basedir}/external/libffi/ffi64.def
build_ext_ffi_srcdep     = win64

#build_ext_iconv_dlldef   = ${basedir}/external/libiconv/iconv.def
#build_ext_iconv_run      = 1

build_ext_jpeg_dlldef    = ${basedir}/external/libjpeg/jpeg.def

build_ext_png_dlldef     = ${basedir}/external/libpng/png.def

build_ext_ft_dlldef      = ${basedir}/external/freetype/freetype.def
build_ext_ft_flags       = -DFT2_BUILD_LIBRARY

build_ext_icu4c_flags    = -DU_WINDOWS
build_ext_icuuc_dlldef   = ${basedir}/external/icu4c/icuuc.def
build_ext_icui18n_dlldef = ${basedir}/external/icu4c/icui18n.def

build_java_jcl_dlldef    = ${basedir}/java/jcl/jcl.def

