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
# Version : 0.1.1                                    #
# Desc    : [xi.external.icu4c] Makefile             #
#----------------------------------------------------#
# History)                                           #
#   - 2013/05/12 : Created by cmjo                   #
######################################################

# PREPARE : set base
basedir = ../..

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

# PREPARE : Directories
# Base
current_dir_abs             = $(CURDIR)
current_dir_rel             = $(notdir $(current_dir_abs))
# Base
module_dir_target           = $(basedir)/amk/$(build_cfg_target)/external/$(current_dir_rel)
module_dir_object           = $(module_dir_target)/object
module_dir_object_icuuc     = $(module_dir_object)/icuuc
module_dir_object_icui18n   = $(module_dir_object)/icui18n
# Output
module_dir_output_base      = $(basedir)/amk/$(build_cfg_target)/emul
module_dir_output_bin       = $(module_dir_output_base)/bin
module_dir_output_inc       = $(module_dir_output_base)/include
module_dir_output_inc_icu4c = $(module_dir_output_inc)/unicode
module_dir_output_lib       = $(module_dir_output_base)/lib
module_dir_output_res       = $(module_dir_output_base)/res
module_dir_output_test      = $(module_dir_output_base)/test

# PREPARE : Build Options
module_build_src_bin           = 
module_build_src_mk_icuuc      = $(wildcard $(current_dir_abs)/common/*.c)
module_build_src_mk_icuuc     += $(wildcard $(current_dir_abs)/common/*.cpp)
module_build_src_mk_icui18n    = $(wildcard $(current_dir_abs)/i18n/*.c)
module_build_src_mk_icui18n   += $(wildcard $(current_dir_abs)/i18n/*.cpp)
module_build_src_ex            = %props_data.c
module_build_cflags_icuuc      = -I$(current_dir_abs)/common -I$(current_dir_abs)/i18n -DICU_DATA_DIR_PREFIX_ENV_VAR="ICU_HOME" -DICU_DATA_DIR="../res/icu" -DU_COMMON_IMPLEMENTATION -DHAVE_ANDROID_OS $(build_ext_icu4c_cflags)
module_build_cflags_icui18n    = -I$(current_dir_abs)/common -I$(current_dir_abs)/i18n -DU_I18N_IMPLEMENTATION -DHAVE_ANDROID_OS $(build_ext_icu4c_cflags) 
module_build_ldflags_icuuc     = $(build_ext_icu4c_ldf_uc)
module_build_ldflags_icui18n   = -L$(module_dir_target) -licuuc $(build_ext_icu4c_ldf_i18n)
module_build_target_a_icuuc    = $(build_opt_a_pre)icuuc.$(build_opt_a_ext)
module_build_target_so_icuuc   = $(build_opt_so_pre)icuuc.$(build_opt_so_ext)
module_build_target_a_icui18n  = $(build_opt_a_pre)icui18n.$(build_opt_a_ext)
module_build_target_so_icui18n = $(build_opt_so_pre)icui18n.$(build_opt_so_ext)

# PREPARE : Set VPATH!!
vpath
vpath %.c $(current_dir_abs)/common:$(current_dir_abs)/i18n
vpath %.cpp $(current_dir_abs)/common:$(current_dir_abs)/i18n

# PREPARE : Build Targets
ifeq ($(build_run_a),1)
module_objs_a_tmp0           = $(patsubst %props_data.c,,$(module_build_src_mk_icuuc))
module_objs_a_tmp1           = $(patsubst %.cpp,%.o,$(module_objs_a_tmp0))
module_objs_static_icuuc     = $(patsubst %.c,%.o,$(module_objs_a_tmp1))
module_objs_a_tmp2           = $(patsubst %props_data.c,,$(module_build_src_mk_icui18n))
module_objs_a_tmp3           = $(patsubst %.cpp,%.o,$(module_objs_a_tmp2))
module_objs_static_icui18n   = $(patsubst %.c,%.o,$(module_objs_a_tmp3))
module_link_a_tmp4           = $(notdir $(module_objs_static_icuuc))
module_link_static_icuuc     = $(addprefix $(module_dir_object_icuuc)/,$(module_link_a_tmp4))
module_link_a_tmp5           = $(notdir $(module_objs_static_icui18n))
module_link_static_icui18n   = $(addprefix $(module_dir_object_icui18n)/,$(module_link_a_tmp5))
module_target_static         = $(module_build_target_a_icuuc) $(module_build_target_a_icui18n)
endif
ifeq ($(build_run_so),1)
module_objs_so_tmp0          = $(patsubst %props_data.c,,$(module_build_src_mk_icuuc))
module_objs_so_tmp1          = $(patsubst %.cpp,%.lo,$(module_objs_so_tmp0))
module_objs_shared_icuuc     = $(patsubst %.c,%.lo,$(module_objs_so_tmp1))
module_objs_so_tmp2          = $(patsubst %props_data.c,,$(module_build_src_mk_icui18n))
module_objs_so_tmp3          = $(patsubst %.cpp,%.lo,$(module_objs_so_tmp2))
module_objs_shared_icui18n   = $(patsubst %.c,%.lo,$(module_objs_so_tmp3))
module_link_so_tmp4          = $(notdir $(module_objs_shared_icuuc))
module_link_shared_icuuc     = $(addprefix $(module_dir_object_icuuc)/,$(module_link_so_tmp4))
module_link_so_tmp5          = $(notdir $(module_objs_shared_icui18n))
module_link_shared_icui18n   = $(addprefix $(module_dir_object_icui18n)/,$(module_link_so_tmp5))
module_target_shared         = $(module_build_target_so_icuuc) $(module_build_target_so_icui18n)
endif


###################
# build-targets
###################

all: prepare $(module_target_static) $(module_target_shared) post

prepare_mkdir_base:
	@$(MKDIR) -p "$(module_dir_target)"
	@$(MKDIR) -p "$(module_dir_object)"
	@$(MKDIR) -p "$(module_dir_object_icuuc)"
	@$(MKDIR) -p "$(module_dir_object_icui18n)"

prepare_mkdir_output:
	@$(MKDIR) -p "$(module_dir_output_base)"
	@$(MKDIR) -p "$(module_dir_output_bin)"
	@$(MKDIR) -p "$(module_dir_output_inc)"
	@$(MKDIR) -p "$(module_dir_output_inc_icu4c)"
	@$(MKDIR) -p "$(module_dir_output_lib)"
	@$(MKDIR) -p "$(module_dir_output_res)"
	@$(MKDIR) -p "$(module_dir_output_test)"

prepare_result:
	@echo $(need_warning)
	@echo "================================================================"
	@echo "TARGET                         : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "current_dir_abs                : $(current_dir_abs)"
	@echo "current_dir_rel                : $(current_dir_rel)"
	@echo "----------------------------------------------------------------"
	@echo "module_dir_target              : $(module_dir_target)"	
	@echo "module_dir_object              : $(module_dir_object)"	
	@echo "module_dir_object_icuuc        : $(module_dir_object_icuuc)"	
	@echo "module_dir_object_icui18n      : $(module_dir_object_icui18n)"	
	@echo "----------------------------------------------------------------"
	@echo "module_dir_output_base         : $(module_dir_output_base)"	
	@echo "module_dir_output_bin          : $(module_dir_output_bin)"	
	@echo "module_dir_output_inc          : $(module_dir_output_inc)"	
	@echo "module_dir_output_lib          : $(module_dir_output_lib)"	
	@echo "module_dir_output_res          : $(module_dir_output_res)"	
	@echo "module_dir_output_test         : $(module_dir_output_test)"	
	@echo "----------------------------------------------------------------"
	@echo "module_build_src_bin           : $(module_build_src_bin)"	
	@echo "module_build_src_mk_icuuc      : $(module_build_src_mk_icuuc)"	
	@echo "module_build_src_mk_icui8n     : $(module_build_src_mk_icui8n)"	
	@echo "module_build_src_ex            : $(module_build_src_ex)"	
	@echo "module_build_cflags_icuuc      : $(module_build_cflags_icuuc)"	
	@echo "module_build_cflags_icui18n    : $(module_build_cflags_icui18n)"	
	@echo "module_build_ldflags_icuuc     : $(module_build_ldflags_icuuc)"	
	@echo "module_build_ldflags_icui18n   : $(module_build_ldflags_icui18n)"	
	@echo "module_build_target_a_icuuc    : $(module_build_target_a)"	
	@echo "module_build_target_a_icui18n  : $(module_build_target_a)"	
	@echo "module_build_target_so_icuuc   : $(module_build_target_so)"	
	@echo "module_build_target_so_icui18n : $(module_build_target_so)"	
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result


$(module_build_target_a_icuuc): $(module_link_static_icuuc)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_a_icuuc)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_target)/$(module_build_target_a_icuuc) $(module_link_static_icuuc)
	$(build_tool_ranlib) $(module_dir_target)/$(module_build_target_a_icuuc)
	@echo "================================================================"


$(module_build_target_a_icui18n): $(module_link_static_icui18n)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_a_icui18n)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_target)/$(module_build_target_a_icui18n) $(module_link_static_icui18n)
	$(build_tool_ranlib) $(module_dir_target)/$(module_build_target_a_icui18n)
	@echo "================================================================"


$(module_build_target_so_icuuc): $(module_link_shared_icuuc)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_so_icuuc)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_build_target_so_icuuc) \
		-o $(module_dir_target)/$(module_build_target_so_icuuc) \
		$(module_link_shared_icuuc) \
		$(module_build_ldflags_icuuc) \
		$(build_opt_ld_mgwcx)
	@echo "================================================================"


$(module_build_target_so_icui18n): $(module_link_shared_icui18n)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_so_icui18n)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_build_target_so_icui18n) \
		-o $(module_dir_target)/$(module_build_target_so_icui18n) \
		$(module_link_shared_icui18n) \
		$(module_build_ldflags_icui18n) \
		$(build_opt_ld_mgwcx)
	@echo "================================================================"


post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	$(CP) -R $(current_dir_abs)/common/unicode/*.h $(module_dir_output_inc_icu4c)
	$(CP) -R $(current_dir_abs)/i18n/unicode/*.h $(module_dir_output_inc_icu4c)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_a_icuuc) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_a_icuuc) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_a_icui18n) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_a_icui18n) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_so_icuuc) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_so_icuuc) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_so_icui18n) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_so_icui18n) $(module_dir_output_lib) \
	$(TEST_END)
	@echo "================================================================"


clean: prepare
	$(RM) -rf "$(module_dir_target)"


###################
# build-rules
###################

$(module_dir_object_icuuc)/%.o: %.c
	$(build_tool_cc) $(build_opt_c) $(module_build_cflags_icuuc) -c -o $@ $<

$(module_dir_object_icuuc)/%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) $(module_build_cflags_icuuc) -c -o $@ $<

$(module_dir_object_icuuc)/%.o: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) $(module_build_cflags_icuuc) -c -o $@ $<

$(module_dir_object_icuuc)/%.lo: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) $(build_opt_fPIC) $(module_build_cflags_icuuc) -c -o $@ $<

$(module_dir_object_icui18n)/%.o: %.c
	$(build_tool_cc) $(build_opt_c) $(module_build_cflags_icui18n) -c -o $@ $<

$(module_dir_object_icui18n)/%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) $(module_build_cflags_icui18n) -c -o $@ $<

$(module_dir_object_icui18n)/%.o: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) $(module_build_cflags_icui18n) -c -o $@ $<

$(module_dir_object_icui18n)/%.lo: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) $(build_opt_fPIC) $(module_build_cflags_icui18n) -c -o $@ $<

