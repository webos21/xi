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
# Desc    : [xi.java.jcl] Makefile                   #
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
current_dir_abs        = $(CURDIR)
current_dir_rel        = $(notdir $(current_dir_abs))
# Base
module_dir_target      = $(basedir)/amk/$(build_cfg_target)/java/$(current_dir_rel)
module_dir_object      = $(module_dir_target)/object
module_dir_classes     = $(module_dir_target)/classes
# Output
module_dir_output_base = $(basedir)/amk/$(build_cfg_target)/emul
module_dir_output_bin  = $(module_dir_output_base)/bin
module_dir_output_inc  = $(module_dir_output_base)/include
module_dir_output_lib  = $(module_dir_output_base)/lib
module_dir_output_res  = $(module_dir_output_base)/res
module_dir_output_test = $(module_dir_output_base)/test

# PREPARE : Build Options
module_build_src_bin    =
module_build_src_mk     = $(wildcard $(current_dir_abs)/src/native/harmony/*.c)
module_build_src_mk    += $(wildcard $(current_dir_abs)/src/native/harmony/*.cpp)
module_build_src_ex     = 
module_build_incdirs    = -I$(module_dir_output_inc) -I$(current_dir_abs)/src/native/include
module_build_libdirs    = -L$(module_dir_output_lib) -lxibase -lz -licuuc -licui18n
module_build_target_a   = $(build_opt_a_pre)jcl.$(build_opt_a_ext)
module_build_target_so  = $(build_opt_so_pre)jcl.$(build_opt_so_ext)
module_build_target_jar = jcl.jar
ifneq ($(ComSpec),)
module_build_java_srcpath = $(current_dir_abs)/src/java;$(current_dir_abs)/vmclasses;$(current_dir_abs)/vmrefs
else
module_build_java_srcpath = $(current_dir_abs)/src/java:$(current_dir_abs)/vmclasses:$(current_dir_abs)/vmrefs
endif

# PREPARE : Set VPATH!!
vpath
vpath %.c $(current_dir_abs)/src/native/harmony
vpath %.cpp $(current_dir_abs)/src/native/harmony

# PREPARE : Build Targets
ifeq ($(build_run_a),1)
module_objs_a_tmp0     = $(patsubst %.cpp,%.o,$(module_build_src_mk))
module_objs_static     = $(patsubst %.c,%.o,$(module_objs_a_tmp0))
module_link_a_tmp1     = $(notdir $(module_objs_static))
module_link_static     = $(addprefix $(module_dir_object)/,$(module_link_a_tmp1))
module_target_static   = $(module_build_target_a)
endif
ifeq ($(build_run_so),1)
module_objs_so_tmp0    = $(patsubst %.cpp,%.lo,$(module_build_src_mk))
module_objs_shared     = $(patsubst %.c,%.lo,$(module_objs_so_tmp0))
module_link_so_tmp1    = $(notdir $(module_objs_shared))
module_link_shared     = $(addprefix $(module_dir_object)/,$(module_link_so_tmp1))
module_target_shared   = $(module_build_target_so)
endif
ifeq ($(build_run_test),1)
module_target_test     = jcl_test
endif


###################
# build-targets
###################

all: prepare $(module_target_static) $(module_target_shared) $(module_build_target_jar) post $(module_target_test)

prepare_mkdir_base:
	@$(MKDIR) -p "$(module_dir_target)"
	@$(MKDIR) -p "$(module_dir_object)"
	@$(MKDIR) -p "$(module_dir_classes)"

prepare_mkdir_output:
	@$(MKDIR) -p "$(module_dir_output_base)"
	@$(MKDIR) -p "$(module_dir_output_bin)"
	@$(MKDIR) -p "$(module_dir_output_inc)"
	@$(MKDIR) -p "$(module_dir_output_lib)"
	@$(MKDIR) -p "$(module_dir_output_res)"
	@$(MKDIR) -p "$(module_dir_output_test)"

prepare_result:
	@echo "$(need_warning)"
	@echo "================================================================"
	@echo "TARGET                  : $(TARGET)"
	@echo "----------------------------------------------------------------"
	@echo "current_dir_abs         : $(current_dir_abs)"
	@echo "current_dir_rel         : $(current_dir_rel)"
	@echo "----------------------------------------------------------------"
	@echo "module_dir_target       : $(module_dir_target)"	
	@echo "module_dir_object       : $(module_dir_object)"	
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
	@echo "module_build_target_jar : $(module_build_target_jar)"	
	@echo "================================================================"

prepare: prepare_mkdir_base prepare_mkdir_output prepare_result


$(module_build_target_a): $(module_link_static)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_a)"
	@echo "----------------------------------------------------------------"
	$(build_tool_ar) rcu $(module_dir_target)/$(module_build_target_a) $(module_link_static)
	$(build_tool_ranlib) $(module_dir_target)/$(module_build_target_a)
	@echo "================================================================"


$(module_build_target_so): $(module_link_shared)
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_so)"
	@echo "----------------------------------------------------------------"
	$(build_tool_linker) \
		$(build_opt_ld) \
		$(build_opt_ld_so)$(module_build_target_so) \
		-o $(module_dir_target)/$(module_build_target_so) \
		$(module_link_shared) \
		$(module_build_libdirs) \
		$(build_ext_jcl_dlldef) \
		$(build_opt_mnocyg) \
		$(build_opt_libgcc) \
		$(build_opt_libgxx)
	@echo "================================================================"


build_jclasses:
	@echo "================================================================"
	@echo "BUILD : java classes"
	@echo "----------------------------------------------------------------"
	find $(current_dir_abs)/src/java -name "*.java" > $(module_dir_target)/jsrc.txt
	find $(current_dir_abs)/vmclasses -name "*.java" >> $(module_dir_target)/jsrc.txt
	find $(current_dir_abs)/vmrefs -name "*.java" >> $(module_dir_target)/jsrc.txt
	$(build_tool_javac) \
		$(java_opt_debug) \
		-d $(module_dir_classes) \
		-sourcepath $(module_build_java_srcpath) \
		@$(module_dir_target)/jsrc.txt
	$(CP) $(current_dir_abs)/src/java/java/security/security.properties $(module_dir_classes)/java/security
	$(CP) $(current_dir_abs)/src/java/java/util/logging/logging.properties $(module_dir_classes)/java/util/logging


$(module_build_target_jar): build_jclasses
	@echo "================================================================"
	@echo "BUILD : $(module_build_target_jar)"
	@echo "----------------------------------------------------------------"
	$(build_tool_jar) \
		cf $(module_dir_target)/$(module_build_target_jar) \
		-C $(module_dir_classes) .


post:
	@echo "================================================================"
	@echo "OUTPUT : $(current_dir_abs)"
	@echo "----------------------------------------------------------------"
	$(CP) $(current_dir_abs)/src/native/include/jni.h $(module_dir_output_inc)
	$(CP) $(current_dir_abs)/src/native/include/jni_md.h $(module_dir_output_inc)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_a) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_a) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_so) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_so) $(module_dir_output_lib) \
	$(TEST_END)
	$(TEST_FILE) $(module_dir_target)/$(module_build_target_jar) $(TEST_THEN) \
		$(CP) $(module_dir_target)/$(module_build_target_jar) $(module_dir_output_lib) \
	$(TEST_END)
	@echo "================================================================"


jcl_test:
	@echo "================================================================"
	@echo "BUILD : jcl test classes"
	@echo "----------------------------------------------------------------"
	find $(current_dir_abs)/test -name "*.java" > $(module_dir_target)/jtest.txt
	$(build_tool_javac) \
		$(java_opt_debug) \
		-bootclasspath $(module_dir_output_lib)/$(module_build_target_jar) \
		-d ${module_dir_output_test} \
		@$(module_dir_target)/jtest.txt


clean: prepare
	$(RM) -rf "$(module_dir_target)"


###################
# build-rules
###################

$(module_dir_object)/%.o: %.c
	$(build_tool_cc) $(build_opt_c) -c $(module_build_incdirs) -o $@ $<

$(module_dir_object)/%.o: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) -c $(module_build_incdirs) -o $@ $<

$(module_dir_object)/%.lo: %.c
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) -c $(module_build_incdirs) -o $@ $<

$(module_dir_object)/%.lo: %.cpp
	$(build_tool_cxx) $(build_opt_cxx) $(build_opt_fPIC) -c $(module_build_incdirs) -o $@ $<


