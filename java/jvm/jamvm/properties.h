/*
 * Copyright (C) 2009 Robert Lougher <rob@jamvm.org.uk>.
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

/* If we have endian.h include it.  Otherwise, include sys/param.h
   if we have it. If the BYTE_ORDER macro is still undefined, we
   fall-back, and work out the endianness ourselves at runtime --
   this always works.
*/

#define IS_BIG_ENDIAN FALSE

#define IS_BE64 (sizeof(xuintptr) == 8 && IS_BIG_ENDIAN)
