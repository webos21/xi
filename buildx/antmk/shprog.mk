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
# File    : shprog.mk                                #
# Version : 0.1.0                                    #
# Desc    : properties for finding shell cmd.        #
#----------------------------------------------------#
# History)                                           #
#   - 2013/04/16 : Created by cmjo                   #
######################################################


########################
# Programs
########################
ifneq ($(ComSpec),) 
CP = "cp.exe"
RM = "rm.exe" -f
MKDIR = "mkdir.exe"
TAR = "tar.exe"
CHMOD = echo "chmod"
TEST_FILE = if EXIST
TEST_DIR = if EXIST
TEST_VAR = if
TEST_EQ = "=="
TEST_THEN = 
TEST_END =
else
CP = cp
RM = rm -f
MKDIR = mkdir
TAR = tar
CHMOD = chmod
TEST_FILE = if test -f
TEST_DIR = if test -d
TEST_VAR = if test
TEST_EQ = "="
TEST_THEN = ; then
TEST_END = ; fi
endif

