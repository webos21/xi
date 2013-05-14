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
# File    : project.mk                               #
# Version : 0.1.0                                    #
# Desc    : properties for Project.                  #
#----------------------------------------------------#
# History)                                           #
#   - 2011/06/15 : Created by cmjo                   #
######################################################

project_name       = XI

project_date       = 2011-01
project_date_year  = 2011
project_date_month = 01

project_ver_target = dev
project_ver_major  = 1
project_ver_minor  = 0
project_ver_build  = 0
project_ver_number = ${project_ver_major}_${project_ver_minor}_${project_ver_build}
project_version    = ${project_ver_number}-${project_ver_target}

project_def_target = mingw64
