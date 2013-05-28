@echo off
REM Copyright 2013 Cheolmin Jo (webos21@gmail.com)
REM 
REM Licensed under the Apache License, Version 2.0 (the "License");
REM you may not use this file except in compliance with the License.
REM You may obtain a copy of the License at
REM 
REM     http://www.apache.org/licenses/LICENSE-2.0
REM 
REM Unless required by applicable law or agreed to in writing, software
REM distributed under the License is distributed on an "AS IS" BASIS,
REM WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
REM See the License for the specific language governing permissions and
REM limitations under the License.
REM 

REM 
REM check the arguments (remove the REMs)
REM 
REM echo "arg01 = %1"
REM echo "arg02 = %2"
REM echo "arg03 = %3"
REM echo "arg04 = %4"
REM echo "arg05 = %5"
REM echo "arg06 = %6"
REM echo "arg07 = %7"
REM echo "arg08 = %8"
REM echo "arg09 = %9"

REM 
REM set default directory variables
REM 
SET "WBTMP_sdkdir=C:\Program Files\Microsoft SDKs\Windows\v7.1"
SET "WBTMP_vsdir=C:\Program Files (x86)\Microsoft Visual Studio 10.0"
SET "WBTMP_vside=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE"
SET "WBTMP_vcdir=%WBTMP_vsdir%\VC"
SET "WBTMP_path=%WBTMP_vside%"
SET "WBTMP_inc=%WBTMP_vcdir%\include;%WBTMP_sdkdir%\Include"

REM 
REM set the lib directory for each target machine
REM 
if "%1" == "mingw32" (
	SET "WBTMP_lib=%WBTMP_vcdir%\lib;%WBTMP_sdkdir%\Lib"
) else (
	SET "WBTMP_lib=%WBTMP_vcdir%\lib\amd64;%WBTMP_sdkdir%\Lib\x64"
)

REM 
REM set the PATH/INCLUDE/LIB
REM 
SET PATH=%WBTMP_path%;%PATH%
SET INCLUDE=%WBTMP_inc%;%INCLUDE%
SET LIB=%WBTMP_lib%;%LIB%

REM 
REM execute the Make
REM 
%2 %3 %4 %5 %6 %7 %8 %9
