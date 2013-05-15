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
# Desc    : [xi.external] Makefile                   #
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
xi_external_abs   = $(CURDIR)
xi_external_rel   = $(notdir $(xi_external_abs))

# PREPARE : set target directory
xi_external_target = $(basedir)/amk/$(build_cfg_target)/$(xi_external_rel)


###################
# make targets
###################

all: prepare do_build

prepare:
	@echo $(need_warning)
	@$(MKDIR) -p "$(xi_external_target)"
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "xi_external_abs         : $(xi_external_abs)"
	@echo "xi_external_rel         : $(xi_external_rel)"
	@echo "----------------------------------------------------------------"
	@echo "xi_external_target      : $(xi_external_target)"	
	@echo "================================================================"


do_build:
	$(TEST_VAR) "$(build_ext_zlib_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C zlib TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_ffi_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C libffi TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_iconv_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C libiconv TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_jpeg_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C libjpeg TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_png_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C libpng TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_ft_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C freetype TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_icu4c_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C icu4c TARGET=$(TARGET) \
	$(TEST_END)
	$(TEST_VAR) "$(build_ext_sqlite_run)" $(TEST_EQ) "1" $(TEST_THEN) \
		$(MAKE) -C sqlite TARGET=$(TARGET) \
	$(TEST_END)

clean: prepare
	$(RM) -rf "$(xi_external_target)"
