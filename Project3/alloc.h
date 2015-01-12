/**************************************************
# Copyright (C) 2014 Raptis Dimos <raptis.dimos@yahoo.gr>
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# **************************************************/

/*
 *  alloc.h -- 2D array allocation
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 */ 

#ifndef ALLOC_H__
#define ALLOC_H__

#include "common.h"

BEGIN_C_DECLS__

void **calloc_2d(size_t n, size_t m, size_t size);
void **copy_2d(void **dst, const void **src, size_t n, size_t m, size_t size);
void free_2d(void **array);

END_C_DECLS__

#endif  /* ALLOC_H__ */
