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
# File    : build_android.mk                         #
# Version : 0.1.0                                    #
# Desc    : properties file for Android build.       #
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
build_cfg_target   = android
build_cfg_android  = 1
build_cfg_posix    = 1


########################
# Directories
########################
build_droid_ndk       = /home/appos/DevSoft/android-ndk-r8d
build_droid_api       = android-9
build_droid_cpu       = arm
build_droid_arch      = arch-${build_droid_cpu}
build_droid_gccver    = 4.4.3
build_droid_eabi      = ${build_droid_cpu}-linux-androideabi
build_droid_platform  = ${build_droid_ndk}/platforms/${build_droid_api}
build_droid_toolchain = ${build_droid_ndk}/toolchains/${build_droid_eabi}-${build_droid_gccver}

build_tool_dir        = ${build_droid_toolchain}/prebuilt/linux-x86/bin
build_droid_sysroot   = ${build_droid_ndk}/platforms/${build_droid_api}/${build_droid_arch}
build_droid_sysinc    = ${build_droid_sysroot}/usr/include
build_droid_syslib    = ${build_droid_sysroot}/usr/lib


########################
# Program Definition
########################
build_tool_as      = ${build_tool_dir}/${build_droid_eabi}-as
build_tool_cc      = ${build_tool_dir}/${build_droid_eabi}-gcc
build_tool_cxx     = ${build_tool_dir}/${build_droid_eabi}-g++
build_tool_linker  = ${build_tool_dir}/${build_droid_eabi}-g++
build_tool_ar      = ${build_tool_dir}/${build_droid_eabi}-ar
build_tool_ranlib  = ${build_tool_dir}/${build_droid_eabi}-ranlib


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

build_opt_c        = --sysroot=${build_droid_sysroot} -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx      = --sysroot=${build_droid_sysroot} -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_fPIC     = -fPIC
build_opt_ld       = --sysroot=${build_droid_sysroot} -g -Wl,--no-undefined
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
build_xibase_ldflags     = -lc -ldl 

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
build_ext_ffi_srcdep     = arm

#build_ext_iconv_run     = 1
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
build_ext_icu4c_ldf_uc   = -L${basedir}/lib/${build_cfg_target}/lib -lstlport_shared
build_ext_icu4c_ldf_i18n = -L${basedir}/lib/${build_cfg_target}/lib -lstlport_shared


########################
# Compile Target : Java
########################

build_java_jvm_cflags    =
build_java_jvm_ldflags   =

build_java_jcl_cflags    = -I${basedir}/lib/${build_cfg_target}/include -I${basedir}/lib/${build_cfg_target}/include/stlport/stlport
build_java_jcl_ldflags   = -L${basedir}/lib/${build_cfg_target}/lib -lstlport_shared

