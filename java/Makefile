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
# Version : 0.1.1                                    #
# Desc    : [xi.java] Makefile                       #
#----------------------------------------------------#
# History)                                           #
#   - 2013/05/05 : Created by cmjo                   #
######################################################

# PREPARE : set base
basedir = ..

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
xi_java_abs   = $(CURDIR)
xi_java_rel   = $(notdir $(xi_java_abs))

# PREPARE : set target directory
xi_java_target = $(basedir)/amk/$(build_cfg_target)/$(xi_java_rel)


###################
# make targets
###################

all: prepare do_build

prepare:
	@echo $(need_warning)
	@$(MKDIR) -p "$(xi_java_target)"
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "xi_java_abs             : $(xi_java_abs)"
	@echo "xi_java_rel             : $(xi_java_rel)"
	@echo "----------------------------------------------------------------"
	@echo "xi_java_target          : $(xi_java_target)"	
	@echo "================================================================"


do_build:
	$(MAKE) -C jvm TARGET=$(TARGET)
	$(MAKE) -C jcl TARGET=$(TARGET)


clean: prepare
	$(RM) "$(xi_java_target)"

