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
# File    : build.win32.properties                   #
# Version : 0.1.0                                    #
# Desc    : properties file for Windows build.       #
#----------------------------------------------------#
# History)                                           #
#   - 2011/06/15 : Created by cmjo                   #
######################################################


########################
# Programs
########################
include shprog.mk


########################
# Build Configuration
########################
build_cfg_target  = win32
build_cfg_win32   = 1
#build_cfg_posix   = 1


########################
# Directories
########################
build_tool_dir = ${windir}/Microsoft.NET/Framework/v3.5
#build_tool_dir = "${ProgramFiles}/Microsoft Visual Studio 9.0/VC/bin"


########################
# Program Definition
########################
build_tool_cc      = ${build_tool_dir}/cl.exe
build_tool_cxx     = ${build_tool_dir}/cl.exe
build_tool_linker  = ${build_tool_dir}/link.exe
build_tool_ar      =
build_tool_ranlib  =
build_tool_msbuild = ${build_tool_dir}/msbuild.exe


########################
# Compile Flags
########################
build_run_a       = 
build_run_so      =

build_opt_a_pre   = 
build_opt_a_ext   = lib
build_opt_so_pre  = 
build_opt_so_ext  = dll
build_opt_exe_ext = .exe

build_opt_c       = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_cxx     = -g -Wall -Wextra -O3 -D_REENTRANT -DXI_BUILD_${build_cfg_target} -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_ld      = -g -Wl,--no-undefined
build_opt_ld_so   = -shared -Wl,-soname,
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  = 
build_opt_libgxx  = 


########################
# Compile Target
########################
build_xi_src_in   = $(wildcard ${basedir}/xi/src/_all/*.c)
build_xi_src_in  += $(wildcard ${basedir}/xi/src/win32/*.c)
build_xi_src_ex   = 
build_xi_inc_dir  = ${basedir}/include
build_xi_lib_dir  = 
build_xi_lib_mod  = -lpthread -lrt -ldl

build_pi_src_in   = $(wildcard ${basedir}/pi/src/_all/*.c)
build_pi_src_in  += $(wildcard ${basedir}/pi/src/_simul/*.c)
build_pi_src_in  += $(wildcard ${basedir}/pi/src/win32/*.c)
build_pi_src_ex   = 
build_pi_inc_dir  = -I${basedir}/pi/src/_simul
build_pi_lib_dir  = 
build_pi_lib_mod  = -lX11 -lXext

build_test_alib   =
