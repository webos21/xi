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
# Desc    : [xi.src.base] Makefile.                  #
#----------------------------------------------------#
# History)                                           #
#   - 2008/06/12 : Created by cmjo                   #
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
current_dir_abs        = $(CURDIR)
current_dir_rel        = $(notdir $(current_dir_abs))
# Base
module_dir_target      = $(basedir)/amk/$(build_cfg_target)/xi/$(current_dir_rel)
module_dir_object      = $(module_dir_target)/object
module_dir_test        = $(module_dir_target)/test
# Output
module_dir_output_base = $(basedir)/amk/$(build_cfg_target)/emul
module_dir_output_bin  = $(module_dir_output_base)/bin
module_dir_output_inc  = $(module_dir_output_base)/include
module_dir_output_lib  = $(module_dir_output_base)/lib
module_dir_output_res  = $(module_dir_output_base)/res
module_dir_output_test = $(module_dir_output_base)/test

# PREPARE : Build Options
module_build_src_bin   = $(build_xibase_src_bin)
module_build_src_mk    = $(build_xibase_src_mk)
module_build_src_ex    = $(build_xibase_src_ex)
module_build_incdirs   = $(build_xibase_inc_dir)
module_build_libdirs   = $(build_xibase_lib_dir)
module_build_target_a  = $(build_opt_a_pre)xibase.$(build_opt_a_ext)
module_build_target_so = $(build_opt_so_pre)xibase.$(build_opt_so_ext)

module_test_src_bin    = $(buildtc_xibase_src_bin)
module_test_src_mk     = $(buildtc_xibase_src_mk)
module_test_src_ex     = $(buildtc_xibase_src_ex)
module_test_incdirs    = $(buildtc_xibase_inc_dir)
module_test_libdirs    = $(buildtc_xibase_lib_dir)
module_test_target_a   = $(buildtc_opt_a_pre)xibasetest.$(build_opt_a_ext)
module_test_target_so  = $(buildtc_opt_so_pre)xibasetest.$(build_opt_so_ext)
module_test_target_bin = xibase$(build_opt_exe_ext)

# PREPARE : Build Targets
ifeq ($(build_run_a),1)
module_link_a_tmp0     = $(patsubst $(basedir)/src/base/src/$(module_build_src_ex),,$(module_build_src_mk))
module_objs_static     = $(patsubst %.c,%.o,$(module_link_a_tmp0))
module_link_a_tmp1     = $(notdir $(module_objs_static))
module_link_static     = $(addprefix $(module_dir_object)/,$(module_link_a_tmp1))
module_target_static   = $(module_build_target_a)
endif
ifeq ($(build_run_so),1)
module_objs_so_tmp0    = $(patsubst $(basedir)/src/base/src/$(module_build_src_ex),,$(module_build_src_mk))
module_objs_shared     = $(patsubst %.c,%.lo,$(module_objs_so_tmp0))
module_link_so_tmp1    = $(notdir $(module_objs_shared))
module_link_shared     = $(addprefix $(module_dir_object)/,$(module_link_so_tmp1))
module_target_shared   = $(module_build_target_so)
endif
ifeq ($(build_run_test),1)
module_objs_test_tmp0  = $(patsubst $(basedir)/src/base/test/$(module_test_src_ex),,$(module_test_src_mk))
module_objs_tstatic    = $(patsubst %.c,%.to,$(module_objs_test_tmp0))
module_link_test_tmp1  = $(notdir $(module_objs_tstatic))
module_link_tstatic    = $(addprefix $(module_dir_test)/,$(module_link_test_tmp1))
module_objs_tshared    = $(patsubst %.c,%.tlo,$(module_objs_test_tmp0))
module_link_test_tmp2  = $(notdir $(module_objs_tshared))
module_link_tshared    = $(addprefix $(module_dir_test)/,$(module_link_test_tmp2))
module_target_test     = $(module_test_target_bin)
endif


###################
# build-targets
###################

all: prepare $(module_target_static) $(module_target_shared) $(module_target_test) post

prepare_mkdir_base:
	@echo $(need_warning)
	@$(MKDIR) -p "$(module_dir_target)"
	@$(MKDIR) -p "$(module_dir_object)"
	@$(MKDIR) -p "$(module_dir_test)"

prepare_mkdir_output:
	@$(MKDIR) -p "$(module_dir_output_base)"
	@$(MKDIR) -p "$(module_dir_output_bin)"
	@$(MKDIR) -p "$(module_dir_output_inc)"
	@$(MKDIR) -p "$(module_dir_output_lib)"
	@$(MKDIR) -p "$(module_dir_output_res)"
	@$(MKDIR) -p "$(module_dir_output_test)"

prepare_result:
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "current_dir_abs         : $(current_dir_abs)"
	@echo "current_dir_rel         : $(current_dir_rel)"
	@echo "----------------------------------------------------------------"
	@echo "module_dir_target       : $(module_dir_target)"	
	@echo "module_dir_object       : $(module_dir_object)"	
	@echo "module_dir_test         : $(module_dir_test)"	
	@echo "----------------------------------------------------------------"
	@echo "module_dir_output_base  : $(module_dir_output_base)"	
	@echo "module_dir_output_bin   : $(module_dir_output_bin)"	
	@echo "module_dir_output_inc   : $(module_dir_output_inc)"	
	@echo "module_dir_output_lib   : $(module_dir_output_lib)"	
	@echo "module_dir_output_res   : $(module_dir_output_res)"	
	@echo "module_dir_output_test  : $(module_dir_output_test)"	
	@echo "----------------------------------------------------------------"
	@echo "module_build_src_bin    : $(module_build_src_bin)"	
	@echo "module_build_src_mk     : $(module_build_src_mk)"	
	@echo "module_build_src_ex     : $(module_build_src_ex)"	
	@echo "module_build_incdirs    : $(module_build_incdirs)"	
	@echo "module_build_libdirs    : $(module_build_libdirs)"	
	@echo "module_build_target_a   : $(module_build_target_a)"	
	@echo "module_build_target_so  : $(module_build_target_so)"	
	@echo "----------------------------------------------------------------"
	@echo "module_test_src_bin     : $(module_test_src_bin)"	
	@echo "module_test_src_mk      : $(module_test_src_mk)"	
	@echo "module_test_src_ex      : $(module_test_src_ex)"	
	@echo "module_test_incdirs     : $(module_test_incdirs)"	
	@echo "module_test_libdirs     : $(module_test_libdirs)"	
	@echo "module_test_target_a    : $(module_test_target_a)"	
	@echo "module_test_target_so   : $(module_test_target_so)"
	@echo "module_test_target_bin  : $(module_test_target_bin)"
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result


$(module_build_target_a): $(module_objs_static) $(module_link_static)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_a)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_target)/$(module_build_target_a) $(module_link_static)
	$(build_tool_ranlib) $(module_dir_target)/$(module_build_target_a)
	@echo "================================================================"


$(module_build_target_so): $(module_objs_shared) $(module_link_shared)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_so)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_build_target_so) \
		-o $(module_dir_target)/$(module_build_target_so) \
		$(module_link_shared) \
		$(module_build_libdirs) \
		$(build_xibase_lib_mod) \
		$(build_xibase_dlldef) \
		$(build_opt_mnocyg) \
		$(build_opt_libgcc) \
		$(build_opt_libgxx)
	@echo "================================================================"


$(module_test_target_a): $(module_objs_tstatic) $(module_link_tstatic)
	@echo "================================================================"
	@echo "BUILD : $(module_test_target_a)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_test)/$(module_test_target_a) $(module_link_tstatic)
	$(build_tool_ranlib) $(module_dir_test)/$(module_test_target_a)
	@echo "================================================================"


$(module_test_target_so): $(module_objs_tshared) $(module_link_tshared)
	@echo "================================================================"
	@echo "BUILD : $(module_test_target_so)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_test_target_so) \
		-o $(module_dir_target)/$(module_test_target_so) \
		$(module_link_tshared) \
		$(module_test_libdirs) -L$(module_dir_target)\
		$(buildtc_xibase_lib_mod) \
		$(buildtc_xibase_dlldef) \
		$(build_opt_mnocyg) \
		$(build_opt_libgcc) \
		$(build_opt_libgxx)
	@echo "================================================================"


$(module_test_target_bin): $(module_test_target_so)
	@echo "================================================================"
	@echo "BUILD : $(module_test_target_bin)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		-o $(module_dir_target)/$(module_test_target_bin) \
		$(module_link_tshared) \
		$(module_test_incdirs) \
		$(module_test_libdirs) -L$(module_dir_target)\
		$(buildtc_xibase_lib_mod) -lxibasetest \
		$(build_opt_mnocyg) \
		$(build_opt_libgcc) \
		$(build_opt_libgxx) \
		$(current_dir_abs)/test/$(module_test_src_bin)
	@echo "================================================================"


post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	$(CP) -R $(basedir)/include/* $(module_dir_output_inc)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_a) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_a) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_so) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_so) $(module_dir_output_lib) \
	$(TEST_END)
	$(CP) -R $(basedir)/lib/$(build_cfg_target)/* $(module_dir_output_base)
	$(CP) -R $(basedir)/res/* $(module_dir_output_res)
	$(TEST_FILE) $(module_dir_target)/$(module_test_target_a) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_test_target_a) $(module_dir_output_test) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_test_target_so) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_test_target_so) $(module_dir_output_test) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_test_target_bin) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_test_target_bin) $(module_dir_output_test) \
	$(TEST_END)
	@echo "================================================================"


###################
# build-rules
###################

%.o: %.c
	$(build_tool_cc) $(build_opt_c) -c $(module_build_incdirs) -o $(module_dir_object)/$(notdir $@) $<

%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) -c $(module_build_incdirs) -o $(module_dir_object)/$(notdir $@) $<

%.to: %.c
	$(build_tool_cc) $(build_opt_c) -c $(module_test_incdirs) -o $(module_dir_test)/$(notdir $@) $<

%.tlo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) -c $(module_test_incdirs) -o $(module_dir_test)/$(notdir $@) $<

