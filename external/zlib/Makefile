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
# Desc    : XI makefile.                             #
#----------------------------------------------------#
# History)                                           #
#   - 2008/06/12 : Created by cmjo                   #
######################################################

# Directories
basedir = ..

# include TARGET.mk
ifeq ($(TARGET),)
include $(basedir)/buildx/make/xpi_linux64.mk
else
include $(basedir)/buildx/make/xpi_$(TARGET).mk
endif

# target-name
mod_static_lib = $(build_opt_a_pre)xg.$(build_opt_a_ext)
mod_shared_lib = $(build_opt_so_pre)xg.$(build_opt_so_ext)

# calc vars
current_dir_abs = $(CURDIR)
current_dir_rel = $(notdir $(current_dir_abs))

xpi_output_dir  = ${basedir}/bin/output

module_target_dir = $(basedir)/bin/$(build_cfg_target)/$(current_dir_rel)

module_incdirs = -I$(build_opt_inc_xi) $(build_xi_inc_dir)
module_libdirs = $(build_xi_lib_dir)

# source-extension
extension=.c

# source-files
mod_sources  = $(build_xi_src_in)

# object-files
mod_static_objs = $(patsubst %$(extension),%.o,$(mod_sources))
mod_shared_objs = $(patsubst %$(extension),%.lo,$(mod_sources))

mod_static_link = $(wildcard $(basedir)/bin/$(build_cfg_target)/$(current_dir_rel)/objects/*.o)
mod_shared_link = $(wildcard $(basedir)/bin/$(build_cfg_target)/$(current_dir_rel)/objects/*.lo)

# make targets
#all: prepare mod_static mod_shared post
all: prepare mod_shared post

prepare:
	@echo "TARGET - $(TARGET)"
	@echo "current_dir_abs - $(current_dir_abs)"
	@echo "current_dir_rel - $(current_dir_rel)"
	@$(MKDIR) -p $(module_target_dir)/objects
	@$(MKDIR) -p $(xpi_output_dir)/bin
	@$(MKDIR) -p $(xpi_output_dir)/lib/$(build_cfg_target)

post:
#	@$(CP) $(module_target_dir)/$(mod_static_lib) $(xpi_output_dir)/$(build_cfg_target)
	@$(CP) $(module_target_dir)/$(mod_shared_lib) $(xpi_output_dir)/lib/$(build_cfg_target)

mod_static: $(mod_static_lib)

mod_shared: $(mod_shared_lib)

clean:
	@echo "Nothing to clean on XI"

# build-rules

%.o: %$(extension)
	$(build_tool_cc) $(build_opt_c) -c $(module_incdirs) -o $(module_target_dir)/objects/$(notdir $@) $<

%.lo: %$(extension)
	$(build_tool_cc) $(build_opt_c) $(build_opt_fPIC) -c $(module_incdirs) -o $(module_target_dir)/objects/$(notdir $@) $<

$(mod_static_lib): $(mod_static_objs)
	$(build_tool_ar) rcu $(module_target_dir)/$@ $(mod_static_link)
	$(build_tool_ranlib) $(module_target_dir)/$@

$(mod_shared_lib): $(mod_shared_objs)
	@echo "mod_shared_link = $(mod_shared_link)"
ifeq (1, $(build_cfg_mac))
	$(build_tool_linker) ${build_opt_ld} -dynamiclib -Wl,-install_name,$(mod_shared_lib) -o ${module_target_dir}/${mod_shared_lib} $(mod_shared_link) $(module_libdirs) $(build_xi_lib_mod)
else
	$(build_tool_linker) ${build_opt_ld} -shared -Wl,-soname,$(mod_shared_lib) -o ${module_target_dir}/${mod_shared_lib} $(mod_shared_link) $(module_libdirs) $(build_xi_lib_mod)
endif
