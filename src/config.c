/**
 * author: xingshuo
 * date: 2017-12-25
 */
#include "config.h"
#include "map.h"

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