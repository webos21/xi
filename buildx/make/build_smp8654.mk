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
# File    : build.smp8654.properties                 #
# Version : 0.1.0                                    #
# Desc    : properties file for SMP8654 build.       #
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
build_cfg_target  = smp8654
build_cfg_smp8654 = 1
build_cfg_posix   = 1


########################
# Directories
########################
build_tool_dir = /opt/toolchains/smp8654/bin


########################
# Program Definition
########################
build_tool_cc     = ${build_tool_dir}/mips-linux-gnu-gcc
build_tool_cxx    = ${build_tool_dir}/mips-linux-gnu-g++
build_tool_linker = ${build_tool_dir}/mips-linux-gnu-g++
build_tool_ar     = ${build_tool_dir}/mips-linux-gnu-ar
build_tool_ranlib = ${build_tool_dir}/mips-linux-gnu-ranlib


########################
# Compile Flags
########################
build_run_a       =
build_run_so      =

build_opt_a_pre   = lib
build_opt_a_ext   = a
build_opt_so_pre  = lib
build_opt_so_ext  = so
build_opt_exe_ext =

build_opt_c       = -EL -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Wa,-mips32r2 -march=24kf -mtune=24kf -DEM86XX_CHIP=EM86XX_CHIPID_TANGO3 -DEM86XX_REVISION=3 -DXBOOT2_SMP865X=1 -DEM86XX_MODE=EM86XX_MODEID_STANDALONE -DWITH_XLOADED_UCODE=1
build_opt_cxx     = -EL -g -Wall -Wextra -O3 -DXI_BUILD_${build_cfg_target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build_opt_ld      = -EL -Wl,--no-undefined
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
build_xi_src_in  += $(wildcard ${basedir}/xi/src/posix/*.c)
build_xi_src_ex   = 
build_xi_inc_dir  = -I${basedir}/include -I${basedir}/lib/${build_cfg_target}/include
build_xi_lib_dir  = -L${basedir}/lib/${build_cfg_target}
build_xi_lib_mod  = -lpthread -lrt -ldl

