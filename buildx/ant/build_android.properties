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
# File    : build.android.properties                 #
# Version : 0.1.0                                    #
# Desc    : properties file for Android build.       #
#----------------------------------------------------#
# History)                                           #
#   - 2011/06/15 : Created by cmjo                   #
######################################################


########################
# Build Configuration
########################
build_cfg_target  = android
build_cfg_android = 1
build_cfg_posix   = 1


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
build_tool_cc     = gcc
build_tool_cxx    = g++
build_tool_linker = g++
build_tool_ar     = ar
build_tool_ranlib = ranlib


########################
# Compile Flags
########################
#build_run_a      =
build_run_so      =

build_opt_a_pre   = lib
build_opt_a_ext   = a
build_opt_so_pre  = lib
build_opt_so_ext  = so
build_opt_exe_ext =

build_opt_c       = --sysroot=${build_droid_sysroot} -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx     = --sysroot=${build_droid_sysroot} -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_ld      = -g --sysroot=${build_droid_sysroot} -Wl,--no-undefined
build_opt_ld_so   = -shared -Wl,-soname,
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  = 
build_opt_libgxx  = 


########################
# Compile Target
########################
build_xi_src_in   = xibase/src/_all/*.c, xibase/src/posix/*.c
build_xi_src_ex   = 
build_xi_inc_dir  = ${basedir}/include
build_xi_lib_dir  = 
build_xi_lib_mod  = -lc -ldl

