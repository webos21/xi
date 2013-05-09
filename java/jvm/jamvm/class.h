/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007
 * Robert Lougher <rob@jamvm.org.uk>.
 *
 * This file is part of JamVM.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* Macros for reading data values from class files - values
   are in big endian format, and non-aligned.  See arch.h
   for READ_DBL - this is platform dependent */

#include "xi/xtype.h"

#define READ_U1(v,p,l)  do {v = *(p)++;} while(0)
#define READ_U2(v,p,l)  do {v = ((p)[0]<<8)|(p)[1]; (p)+=2;} while(0)
#define READ_U4(v,p,l)  do {v = ((p)[0]<<24)|((p)[1]<<16)|((p)[2]<<8)|(p)[3]; (p)+=4;} while(0)
#define READ_U8(v,p,l)  do {v = ((u8)(p)[0]<<56)|((u8)(p)[1]<<48)|((u8)(p)[2]<<40) \
                            |((u8)(p)[3]<<32)|((u8)(p)[4]<<24)|((u8)(p)[5]<<16) \
                            |((u8)(p)[6]<<8)|(u8)(p)[7]; (p)+=8;} while(0)

#define READ_INDEX(v,p,l)               READ_U2(v,p,l)
#define READ_TYPE_INDEX(v,cp,t,p,l)     READ_U2(v,p,l)

/* The default value of the boot classpath is based on the JamVM
   and Classpath install directories.  If zip support is enabled
   the classes will be contained in ZIP files, else they will be
   separate class files in a directory structure */

#define JAMVM_CLASSES ".."XI_SEP_FILE_S"lib"XI_SEP_FILE_S"jcl.jar"
#define CLASSPATH_CLASSES ""

#define DFLT_BCP ".."XI_SEP_FILE_S"lib"XI_SEP_FILE_S"jcl.jar"
