/**
 * author: xingshuo
 * date: 2017-12-25
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <time.h>

#define max(a,b) (((a)>(b)) ? (a) : (b))

#define USE_4HEAP 0 //the switch of 2-heap or 4-heap

enum
{
    None,
    Block = 1,
}; 

typedef uint8_t Tile;

#define is_block_grid(m, r, c)  ((m)->tiles[r*(m)->max_col + c] == Block)
#define is_halfgrid_row(m, row)  (row%2 == (m)->half_grid_flag)
#define is_valid_grid(m, r, c) (r>=0 && r<(m)->max_row && c>=0 && c<(m)->max_col)
#define is_access_path(pf, node) (node && node != (pf)->noway)

#define cal_grids_dist(m, sr, sc, er, ec, dist) \
    do {    \
        int x1 = (m)->wt[sr*(m)->max_col + sc].x;   \
        int y1 = (m)->wt[sr*(m)->max_col + sc].y;   \
        int z1 = (m)->wt[sr*(m)->max_col + sc].z;   \
                                                    \
        int x2 = (m)->wt[er*(m)->max_col + ec].x;   \
        int y2 = (m)->wt[er*(m)->max_col + ec].y;   \
        int z2 = (m)->wt[er*(m)->max_col + ec].z;   \
        dist = max(max(abs(x1 - x2),abs(y1 - y2)),abs(z1 - z2));  \
    }while (0)

extern int DIR_HALF_LIST[][2];
extern int DIR_WHOLE_LIST[][2];
extern int WEIGHT_CONFIG[][3];

#endif