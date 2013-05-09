# Copyright (c) 2006-2013 Cheolmin Jo (webos21@gmail.com)
# All rights reserved.
#
# This code is released under a BSD-style open source license,
# for more detail, see the copyright notice in LICENSE.
#

######################################################
#                        XPI                         #
#----------------------------------------------------#
# File    : Makefile                                 #
# Version : 0.0.1                                    #
# Desc    : Base makefile. It will fork sub-files    #
#----------------------------------------------------#
# History)                                           #
#   - 2008/06/12 : Created by cmjo                   #
######################################################

# Programs
ifeq ($(OS),Windows_NT) 
CP = "cp.exe"
RM = "rm.exe" -f
MKDIR = "mkdir.exe"
TAR = "tar.exe"
CHMOD = echo "chmod"
else
CP = cp
RM = rm -f
MKDIR = mkdir
TAR = tar
CHMOD = chmod
endif

# directories
basedir = .
destdir = ./bin

# make targets
all: usage

doc:
	doxygen buildx/doxygen.cfg

linux32:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

linux64:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

smt5010:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

android:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

mac32:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

mac64:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

lglsa:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

bcm7403:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

smp8654:
	@$(MKDIR) -p "$(destdir)/$@"
	$(MAKE) -C xi TARGET=$@
	$(MAKE) -C pi TARGET=$@
	$(MAKE) -C test TARGET=$@

entire: linux smt5010 lglsa bcm7403 smp8654

clean:
	@$(RM) -rf $(destdir)/*

mingw:
	@$(MKDIR) -p "${basedir}/ant/output/bin"
	@$(MKDIR) -p "${basedir}/ant/output/lib/${build_cfg_target}"
	@$(MKDIR) -p "${basedir}/bin/output/bin"
	@$(MKDIR) -p "${basedir}/bin/output/lib/${build_cfg_target}"
	$(build_tool_msbuild) $(basedir)/buildx/vs2008/xpi.sln
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/xg.lib ${basedir}/ant/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/xg.dll ${basedir}/ant/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/pg.lib ${basedir}/ant/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/pg.dll ${basedir}/ant/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/xg.lib ${basedir}/bin/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/xg.dll ${basedir}/bin/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/pg.lib ${basedir}/bin/output/lib/${build_cfg_target}
	$(CP) ${basedir}/bin/${build_cfg_target}/xpi/pg.dll ${basedir}/bin/output/lib/${build_cfg_target}
	$(MAKE) -C test TARGET=$@

usage:
	@echo "####################################################"
	@echo "#                  XPI Makefile                    #"
	@echo "#--------------------------------------------------#"
	@echo "# We only support below command.                   #"
	@echo "#                                                  #"
	@echo "#   - make doc     : make XPI Documents            #"
	@echo "#                                                  #"
	@echo "#   - make android : make the android XG/PG        #"
	@echo "#   - make linux32 : make the linux 32bit XG/PG    #"
	@echo "#   - make linux64 : make the linux 64bit XG/PG    #"
	@echo "#   - make macosx32: make the macosx 32bit XG/PG   #"
	@echo "#   - make macosx64: make the macosx 64bit XG/PG   #"
	@echo "#   - make smt5010 : make the smt5010 XG/PG        #"
	@echo "#   - make lglsa   : make the lglsa XG/PG          #"
	@echo "#   - make bcm7403 : make the bcm7403 XG/PG        #"
	@echo "#   - make smp8654 : make the smp8654 XG/PG        #"
	@echo "#   - make clean   : clean the source tree         #"
	@echo "#                                                  #"
	@echo "#   - make entire  : make XG/PG for ENTIRE target  #"
	@echo "#                                                  #"
	@echo "#   * for win32, use the visual studio (xpi.sln)   #"
	@echo "#   - make mingw   : make the mingw32 XG/PG        #"
	@echo "####################################################"


