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
 *  timer.c -- Timer routines
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 */ 
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

void timer_clear(xtimer_t *timer)
{
    timerclear(&timer->elapsed_time);
    timerclear(&timer->timestamp);
}

void timer_start(xtimer_t *timer)
{
    if (gettimeofday(&timer->timestamp, NULL) < 0) {
        perror("gettimeofday failed");
        exit(1);
    }
}

void timer_stop(xtimer_t *timer)
{
    struct timeval t_stop;
    struct timeval t_interval;
    if (gettimeofday(&t_stop, NULL) < 0) {
        perror("gettimeofday failed");
        exit(1);
    }

    timersub(&t_stop, &timer->timestamp, &t_interval);
    timeradd(&timer->elapsed_time, &t_interval, &timer->elapsed_time);
}

double timer_elapsed_time(xtimer_t *timer)
{
    return (timer->elapsed_time.tv_sec +
            timer->elapsed_time.tv_usec / (double) USEC_PER_SEC);
}

