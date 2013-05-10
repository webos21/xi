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
# File    : Makefile                                 #
# Version : 0.0.1                                    #
# Desc    : [xi] main Makefile                       #
#----------------------------------------------------#
# History)                                           #
#   - 2008/06/12 : Created by cmjo                   #
######################################################

# make targets
all: usage

doc:
	doxygen buildx/doxygen.cfg

linux32:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

linux64:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

mingw32:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

mingw64:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

osx32:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

osx64:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

android:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

bcm7403:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

smp8654:
	@$(MAKE) -f Makefile.root.mk TARGET=$@

entire: linux32 linux64 mingw32 mingw64 osx32 osx64 android bcm7403 smp8654

clean:
	@$(RM) -rf ./amk/*

usage:
	@echo "####################################################"
	@echo "#                  XPI Makefile                    #"
	@echo "#--------------------------------------------------#"
	@echo "# We only support below command.                   #"
	@echo "#                                                  #"
	@echo "#   - make doc     : make XPI Documents            #"
	@echo "#                                                  #"
	@echo "#   - make linux32 : make the linux 32bit XI       #"
	@echo "#   - make linux64 : make the linux 64bit XI       #"
	@echo "#   - make mingw32 : make the MinGW 32bit XI       #"
	@echo "#   - make mingw64 : make the MinGW 64bit XI       #"
	@echo "#   - make osx32   : make the Mac OSX 32bit XI     #"
	@echo "#   - make osx64   : make the Mac OSX 64bit XI     #"
	@echo "#   - make android : make the android XG/PG        #"
	@echo "#   - make bcm7403 : make the bcm7403 XG/PG        #"
	@echo "#   - make smp8654 : make the smp8654 XG/PG        #"
	@echo "#   - make clean   : clean the source tree         #"
	@echo "#                                                  #"
	@echo "#   - make entire  : make XI for ENTIRE target     #"
	@echo "#                                                  #"
	@echo "#   * for win32, use the visual studio (xpi.sln)   #"
	@echo "#   - make mingw   : make the mingw32 XG/PG        #"
	@echo "####################################################"


