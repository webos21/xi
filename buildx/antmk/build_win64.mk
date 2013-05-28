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
# File    : build_win32.mk                           #
# Version : 0.1.0                                    #
# Desc    : properties for Windows native build.     #
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
build_cfg_target  = win64
build_cfg_win     = 1
#build_cfg_posix   = 1


########################
# Directories
########################
build_winxx_sdkdir = C:\\Program\ Files\\Microsoft\ SDKs\\Windows\\v7.1
build_winxx_vsdir  = C:\\Program\ Files\ \(x86\)\\Microsoft\ Visual\ Studio\ 10.0
build_winxx_vside  = C:\\Program\ Files\ \(x86\)\\Microsoft\ Visual\ Studio\ 10.0\\Common7\\IDE
build_winxx_vcdir  = ${build_winxx_vsdir}\\VC
build_winxx_path   = ${build_winxx_vside}
build_winxx_inc    = ${build_winxx_vcdir}\\include;${build_winxx_sdkdir}\\Include
build_winxx_lib    = ${build_winxx_vcdir}\\lib\\amd64;${build_winxx_sdkdir}\\Lib\\x64

build_tool_dir     = ${build_winxx_vcdir}\\bin\\x86_amd64


########################
# Program Definition
########################
build_tool_as      = ${build_tool_dir}\\as.exe
build_tool_cc      = ${build_tool_dir}\\cl.exe
build_tool_cxx     = ${build_tool_dir}\\cl.exe
build_tool_linker  = ${build_tool_dir}\\link.exe
build_tool_ar      = ar
build_tool_ranlib  = ranlib


########################
# Compile Flags
########################
#build_run_a        = 1
build_run_so       = 1
build_run_test     = 1

build_opt_a_pre    =
build_opt_a_ext    = a
build_opt_so_pre   =
build_opt_so_ext   = dll
build_opt_exe_ext  = .exe

build_opt_c        = -Zi -nologo -W3 -WX- -Od -D "_WINDLL" -D "_MBCS" -D "XI_BUILD_${build_cfg_target}" -Gm -EHsc -RTC1 -MTd -GS -fp:precise -Zc:wchar_t -Zc:forScope -Gd -TC -analyze-
build_opt_cxx      = -Zi -nologo -W3 -WX- -Od -D "_WINDLL" -D "_MBCS" -D "XI_BUILD_${build_cfg_target}" -Gm -EHsc -RTC1 -MTd -GS -fp:precise -Zc:wchar_t -Zc:forScope -Gd -TP -analyze-
build_opt_ld       = -DEBUG -MACHINE:X64 -NOLOGO -TLBID:1 -DYNAMICBASE -NXCOMPAT
build_opt_cl_conly = -c
build_opt_cl_fPIC  =
build_opt_cl_out   =
build_opt_cl_pfx   = -Fo
build_opt_ld_so    = -DLL
build_opt_ld_out   = -OUT:
build_opt_ld_rpath =
build_opt_ld_noud  =
build_opt_ld_mgwcc =
build_opt_ld_mgwcx =
build_opt_ld_mspdb =


########################
# Compile Target : XI
########################

build_xibase_src_bin    =
build_xibase_src_mk     = $(wildcard $(basedir)/src/base/src/_all/*.c)
build_xibase_src_mk    += $(wildcard $(basedir)/src/base/src/win32/*.c)
build_xibase_src_in     = _all/*.c, win32/*.c
build_xibase_src_ex     = 
build_xibase_cflags     = -I${basedir}/include
build_xibase_ldflags    = ws2_32.lib mswsock.lib userenv.lib kernel32.lib advapi32.lib -DEF:${basedir}/src/base/xibase.def

buildtc_xibase_src_bin  = tc_main.c
buildtc_xibase_src_mk   = $(wildcard $(basedir)/src/base/test/*.c)
buildtc_xibase_src_in   = *.c
buildtc_xibase_src_ex   = tc_main.c
buildtc_xibase_cflags   = -I${basedir}/include
buildtc_xibase_ldflags  = "xibase.lib" -DEF:"${basedir}/src/base/xibasetest.def"


########################
# Compile Target : Ext
########################

build_ext_zlib_run       = 1
build_ext_zlib_cflags    =
build_ext_zlib_ldflags   = "kernel32.lib" -DEF:"${basedir}/external/zlib/zlib.def"

build_ext_ffi_run        = 1
build_ext_ffi_cflags     =
build_ext_ffi_ldflags    = -DEF:"${basedir}/external/libffi/ffi32.def"
build_ext_ffi_srcdep     = win32

#build_ext_iconv_run      = 1
build_ext_iconv_cflags   =
build_ext_iconv_ldflags  = -DEF:"${basedir}/external/libiconv/iconv.def"

build_ext_jpeg_run       = 1
build_ext_jpeg_cflags    =
build_ext_jpeg_ldflags   = -DEF:"${basedir}/external/libjpeg/jpeg.def"

build_ext_png_run        = 1
build_ext_png_cflags     =
build_ext_png_ldflags    = -DEF:"${basedir}/external/libpng/png.def"

build_ext_ft_run         = 1
build_ext_ft_cflags      = -D "FT2_BUILD_LIBRARY"
build_ext_ft_ldflags     = -DEF:"${basedir}/external/freetype/freetype.def"

build_ext_icu4c_run      = 1
build_ext_icu4c_cflags   = -D "U_WINDOWS" -D "U_STATIC_IMPLEMENTATION"
build_ext_icu4c_ldf_uc   = -DEF:"${basedir}/external/icu4c/icuuc.def"
build_ext_icu4c_ldf_i18n = -DEF:"${basedir}/external/icu4c/icui18n.def"

build_ext_sqlite_run     = 1
build_ext_sqlite_cflags  =
build_ext_sqlite_ldflags = -DEF:"${basedir}/external/sqlite/sqlite3.def"


########################
# Compile Target : Java
########################

build_java_jvm_cflags    =
build_java_jvm_ldflags   =

build_java_jcl_cflags    =
build_java_jcl_ldflags   = -DEF:"${basedir}/java/jcl/jcl.def"

