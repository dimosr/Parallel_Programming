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
 *  alloc.c -- 2D array allocation
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 */ 

#include <string.h>
#include <stdlib.h>
#include "alloc.h"

void **calloc_2d(size_t n, size_t m, size_t size)
{
    char **ret = (char **) malloc(n*sizeof(char *));
    if (ret) {
        char *area = (char *) calloc(n*m, size);
        if (area) {
            for (size_t i = 0; i < n; ++i)
                ret[i] = (char *) &area[i*m*size];
        } else {
            free(ret);
            ret = NULL;
        }
    }

    return (void **) ret;
}

void **copy_2d(void **dst, const void **src, size_t n, size_t m, size_t size)
{
    memcpy(dst[0], src[0], n*m*size);
    return dst;
}

void free_2d(void **array)
{
    free(array[0]);
    free(array);
}
