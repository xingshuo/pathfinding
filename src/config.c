/**
 * author: xingshuo
 * date: 2017-12-25
 */
#include "config.h"
#include "map.h"

/*------array index and direction mapping-----
 *(0)left top   \      /   (3)right top
 *(1)left       -center-   (4)right
 *(2)left bottom/      \   (5)right bottom
 */
int DIR_HALF_LIST[6][2] = {
    {-1,0},
    {0,-1},
    {1,0},
    {-1,1},
    {0,1},
    {1,1}
};

int DIR_WHOLE_LIST[6][2] = {
    {-1,-1},
    {0,-1},
    {1,-1},
    {-1,0},
    {0,1},
    {1,0}
};

int WEIGHT_CONFIG[6][3] = {
    {0,1,-1},
    {-1,1,0},
    {-1,0,1},
    {1,0,-1},
    {1,-1,0},
    {0,-1,1}
};