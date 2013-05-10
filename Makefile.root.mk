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
#                         XI                         #
#----------------------------------------------------#
# File    : Makefile                                 #
# Version : 0.0.1                                    #
# Desc    : [xi.root] Makefile                       #
#----------------------------------------------------#
# History)                                           #
#   - 2008/06/12 : Created by cmjo                   #
######################################################

# PREPARE : set base
basedir = .

# PREPARE : Check Environment
ifeq ($(TARGET),)
need_warning = "Warning : you are here without proper command!!!!"
include $(basedir)/buildx/antmk/project.mk
include $(basedir)/buildx/antmk/java.mk
include $(basedir)/buildx/antmk/build_$(project_def_target).mk
TARGET = $(project_def_target)
else
need_warning = ""
include $(basedir)/buildx/antmk/project.mk
include $(basedir)/buildx/antmk/java.mk
include $(basedir)/buildx/antmk/build_$(TARGET).mk
endif

# PREPARE : get current directory
xi_root_abs   = $(CURDIR)
xi_root_rel   = $(notdir $(xi_root_abs))

# PREPARE : set target directory
xi_root_target = $(basedir)/amk/$(build_cfg_target)


###################
# build-targets
###################

all: prepare do_build

prepare:
	@echo $(need_warning)
	@$(MKDIR) -p "$(xi_root_target)"
	@echo "======================= $(project_name) - Build Plan ========================"
	@echo "os.name                 : $(OS)"
	@echo "os.arch                 : $(shell uname)"
	@echo "----------------------------------------------------------------"
	@echo "project_version         : $(project_version)"
	@echo "----------------------------------------------------------------"
	@echo "build_cfg_target        : $(build_cfg_target)"
	@echo "build_dir_target        : $(xi_root_target)/$(build_cfg_target)"
	@echo "----------------------------------------------------------------"
	@echo "build_tool_cc           : $(build_tool_cc)"	
	@echo "build_tool_cxx          : $(build_tool_cxx)"	
	@echo "build_tool_linker       : $(build_tool_linker)"	
	@echo "build_tool_ar           : $(build_tool_ar)"	
	@echo "build_tool_ranlib       : $(build_tool_ranlib)"	
	@echo "----------------------------------------------------------------"
	@echo "build_opt_a_pre         : $(build_opt_a_pre)"	
	@echo "build_opt_a_ext         : $(build_opt_a_ext)"	
	@echo "build_opt_so_pre        : $(build_opt_so_pre)"	
	@echo "build_opt_so_ext        : $(build_opt_so_ext)"	
	@echo "build_opt_exe_ext       : $(build_opt_exe_ext)"	
	@echo "----------------------------------------------------------------"
	@echo "build_opt_c             : $(build_opt_c)"	
	@echo "build_opt_cxx           : $(build_opt_cxx)"	
	@echo "build_opt_ld            : $(build_opt_ld)"	
	@echo "build_opt_ld_so         : $(build_opt_ld_so)"	
	@echo "build_opt_ld_noud       : $(build_opt_ld_noud)"	
	@echo "----------------------------------------------------------------"
	@echo "build_opt_fPIC          : $(build_opt_fPIC)"	
	@echo "build_opt_so_ext        : $(build_opt_so_ext)"	
	@echo "build_opt_mnocyg        : $(build_opt_mnocyg)"	
	@echo "build_opt_libgcc        : $(build_opt_libgcc)"	
	@echo "build_opt_libgxx        : $(build_opt_libgxx)"	
	@echo "================================================================"


do_build:
	@$(MAKE) -C external TARGET=$(TARGET)
	@$(MAKE) -C src TARGET=$(TARGET)
	@$(MAKE) -C java TARGET=$(TARGET)


clean: prepare
	$(RM) "$(xi_root_target)"

