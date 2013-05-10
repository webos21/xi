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
# File    : build.osx64.properties                   #
# Version : 0.1.0                                    #
# Desc    : properties file for MacOSX 64bit build.  #
#----------------------------------------------------#
# History)                                           #
#   - 2011/06/15 : Created by cmjo                   #
######################################################


########################
# Build Configuration
########################
build_cfg_target  = osx64
build_cfg_mac     = 1
build_cfg_posix   = 1


########################
# Directories
########################
build_tool_dir = 


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
build_run_a       =
build_run_so      =

build_opt_a_pre   = lib
build_opt_a_ext   = a
build_opt_so_pre  = lib
build_opt_so_ext  = dylib
build_opt_exe_ext =

build_opt_c       = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE
build_opt_cxx     = -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE
build_opt_ld      = -g -Wl,--no-undefined
build_opt_ld_so   = -dynamiclib -Wl,-install_name,
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  = 
build_opt_libgxx  = 



########################
# Compile Target : XI
########################

build_xibase_src_bin    =
build_xibase_src_in     = _all/*.c, win32/*.c
build_xibase_src_ex     = 
build_xibase_inc_dir    = -I${basedir}/include
build_xibase_lib_dir    = 
build_xibase_lib_mod    = -lws2_32 -lmswsock -luserenv
build_xibase_dlldef     = ${basedir}/src/xibase/xibase.def

buildtc_xibase_src_bin  = tc_main.c
buildtc_xibase_src_in   = *.c
buildtc_xibase_src_ex   = tc_main.c
buildtc_xibase_inc_dir  = -I${basedir}/include
buildtc_xibase_lib_dir  = 
buildtc_xibase_lib_mod  = -lxibase
buildtc_xibase_dlldef   = ${basedir}/src/xibase/xibasetest.def


########################
# Compile Target : Ext
########################

build_ext_zlib_dlldef    =

build_ext_ffi_dlldef     =
build_ext_ffi_srcdep     = osx64

build_ext_iconv_dlldef   =
build_ext_iconv_run      =

build_ext_jpeg_dlldef    =

build_ext_png_dlldef     =

build_ext_ft_dlldef      =
build_ext_ft_flags       = -DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON

build_ext_icu4c_flags    = -DU_DARWIN
build_ext_icuuc_dlldef   =
build_ext_icui18n_dlldef =

