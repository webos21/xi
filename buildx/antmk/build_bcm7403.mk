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
# File    : build_bcm7403.mk                         #
# Version : 0.1.0                                    #
# Desc    : properties file for BCM7403 build.       #
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
build_cfg_target   = bcm7403
build_cfg_bcm7403  = 1
build_cfg_posix    = 1


########################
# Directories
########################
build_tool_dir     = /opt/toolchains/bcm7406/bin


########################
# Program Definition
########################
build_tool_cc      = ${build_tool_dir}/mipsel-linux-gcc
build_tool_cxx     = ${build_tool_dir}/mipsel-linux-g++
build_tool_linker  = ${build_tool_dir}/mipsel-linux-g++
build_tool_ar      = ${build_tool_dir}/mipsel-linux-ar
build_tool_ranlib  = ${build_tool_dir}/mipsel-linux-ranlib


########################
# Compile Flags
########################
#build_run_a        = 1
build_run_so       = 1
build_run_test     = 1

build_opt_a_pre    = lib
build_opt_a_ext    = a
build_opt_so_pre   = lib
build_opt_so_ext   = so
build_opt_exe_ext  =

build_opt_c        = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx      = -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_fPIC     = -fPIC
build_opt_ld       = -g -Wl,--no-undefined
build_opt_ld_so    = -shared -Wl,-soname,
build_opt_ld_rpath = -Wl,-rpath-link,
build_opt_ld_noud  = -Wl,--no-undefined
build_opt_ld_mgwcc =
build_opt_ld_mgwcx =


########################
# Compile Target : XI
########################

build_xibase_src_bin     =
build_xibase_src_mk      = $(wildcard $(basedir)/src/base/src/_all/*.c)
build_xibase_src_mk     += $(wildcard $(basedir)/src/base/src/posix/*.c)
build_xibase_src_in      = _all/*.c, posix/*.c
build_xibase_src_ex      = 
build_xibase_cflags      = -I${basedir}/include
build_xibase_ldflags     = -lpthread -lrt -ldl

buildtc_xibase_src_bin   = tc_main.c
buildtc_xibase_src_mk    = $(wildcard $(basedir)/src/base/test/*.c)
buildtc_xibase_src_in    = *.c
buildtc_xibase_src_ex    = tc_main.c
buildtc_xibase_cflags    = -I${basedir}/include
buildtc_xibase_ldflags   = -lxibase


########################
# Compile Target : Ext
########################

build_ext_zlib_run       = 1
build_ext_zlib_cflags    =
build_ext_zlib_ldflags   =

build_ext_ffi_run        = 1
build_ext_ffi_cflags     =
build_ext_ffi_ldflags    =
build_ext_ffi_srcdep     = mips

#build_ext_iconv_run      = 1
build_ext_iconv_cflags   =
build_ext_iconv_ldflags  =

build_ext_jpeg_run       = 1
build_ext_jpeg_cflags    =
build_ext_jpeg_ldflags   =

build_ext_png_run        = 1
build_ext_png_cflags     =
build_ext_png_ldflags    =

build_ext_ft_run         = 1
build_ext_ft_cflags      = -DFT2_BUILD_LIBRARY
build_ext_ft_ldflags     =

build_ext_icu4c_run      = 1
build_ext_icu4c_cflags   = -DU_STATIC_IMPLEMENTATION
build_ext_icu4c_ldf_uc   =
build_ext_icu4c_ldf_i18n =

build_ext_sqlite_run     = 1
build_ext_sqlite_cflags  =
build_ext_sqlite_ldflags = -ldl


########################
# Compile Target : Java
########################

build_java_jvm_cflags    =
build_java_jvm_ldflags   =

build_java_jcl_cflags    =
build_java_jcl_ldflags   =

