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
# Build Configuration
########################
build.cfg.target  = win32
build.cfg.win32   = 1
#build.cfg.posix   = 1


########################
# Directories
########################
build.tool.dir = ${env.windir}/Microsoft.NET/Framework/v3.5
#build.tool.dir = "${env.ProgramFiles}/Microsoft Visual Studio 9.0/VC/bin"


########################
# Program Definition
########################
build.tool.cc      = ${build.tool.dir}/cl.exe
build.tool.cxx     = ${build.tool.dir}/cl.exe
build.tool.linker  = ${build.tool.dir}/link.exe
build.tool.ar      = 
build.tool.ranlib  = 
build.tool.msbuild = ${build.tool.dir}/msbuild.exe


########################
# Compile Flags
########################
#build.run.a
build.run.so

build.opt.a.pre   = 
build.opt.a.ext   = lib
build.opt.so.pre  = 
build.opt.so.ext  = dll
build.opt.exe.ext = .exe

build.opt.c       = -g -Wall -Wextra -Wdeclaration-after-statement -O3 -DXI_BUILD_${build.cfg.target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build.opt.cxx     = -g -Wall -Wextra -O3 -DXI_BUILD_${build.cfg.target} -D_REENTRANT -D_THREAD_SAFE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
build.opt.ld      = -Wl,--no-undefined
build_opt_ld_so   = -shared -Wl,-soname,
build_opt_ld_noud = -Wl,--no-undefined

build.opt.fPIC    = -fPIC
build.opt.mnocyg  = 
build.opt.libgcc  = 
build.opt.libgxx  = 

build.opt.inc.xi  = ${basedir}/include
build.opt.inc.pi  = ${basedir}/include/pi

########################
# Compile Target
########################
build.xi.src.in   = _all/*.c, win32/*.c
build.xi.src.ex   = 
build.xi.inc.dir  = 
build.xi.lib.dir  = 
build.xi.lib.mod  = -lpthread -lrt -ldl

build.pi.src.in   = _all/*.c, _simul/*.c, win32/*.c
build.pi.src.ex   = 
build.pi.inc.dir  =
build.pi.lib.dir  =
build.pi.lib.mod  = -lX11 -lXext

build.test.alib   = 
