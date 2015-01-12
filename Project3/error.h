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
 *  error.h -- Error-handling routines
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 */ 

#ifndef ERROR_H__
#define ERROR_H__

#include "common.h"

BEGIN_C_DECLS__

extern char *program_name;
void set_program_name(char *argv0);

void warning(int errnum, const char *fmt, ...);
void error(int errnum, const char *fmt, ...);
void fatal(int errnum, const char *fmt, ...);

END_C_DECLS__

#endif  /* ERROR_H__ */
