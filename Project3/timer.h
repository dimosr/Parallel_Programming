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
 *  timer.h -- Timer interface
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 */ 
#ifndef TIMER_H__
#define TIMER_H__

#include <sys/time.h>
#include "common.h"

struct xtimer {
    struct timeval elapsed_time;
    struct timeval timestamp;
};

typedef struct xtimer   xtimer_t;

BEGIN_C_DECLS__

void timer_clear(xtimer_t *timer);
void timer_start(xtimer_t *timer);
void timer_stop(xtimer_t *timer);
double timer_elapsed_time(xtimer_t *timer);

END_C_DECLS__

#endif  /* TIMER_H__ */
