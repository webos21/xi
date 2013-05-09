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
# File    : build.mac32.properties                   #
# Version : 0.1.0                                    #
# Desc    : properties file for MacOSX 32bit build.  #
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
build_cfg_target  = osx32
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

build_opt_c       = -m32 -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE
build_opt_cxx     = -m32 -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE
build_opt_ld      = -m32 -g -Wl,--no-undefined
build_opt_ld_so   = -dynamiclib -Wl,-install_name,
build_opt_ld_noud = -Wl,--no-undefined

build_opt_fPIC    = -fPIC
build_opt_mnocyg  = 
build_opt_libgcc  = 
build_opt_libgxx  = 


########################
# Compile Target
########################
build_xi_src_in   = $(wildcard ${basedir}/xi/src/_all/*.c)
build_xi_src_in  += $(wildcard ${basedir}/xi/src/posix/*.c)
build_xi_src_ex   = 
build_xi_inc_dir  = ${basedir}/include
build_xi_lib_dir  = 
build_xi_lib_mod  = -lpthread -ldl

