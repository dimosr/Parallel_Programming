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
 *  cpu_kernels.c -- CPU kernels
 *  Copyright (C) 2014, Computing Systems Laboratory (CSLab)
 */ 

#include "graph.h"

graph_t *MAKE_KERNEL_NAME(_cpu, _omp_naive)(graph_t *graph)
{
    return graph_apsp_base_omp(graph, graph, graph);
}

graph_t *MAKE_KERNEL_NAME(_cpu, _omp_tiled)(graph_t *graph)
{
    return graph_apsp_tile(graph);
}

