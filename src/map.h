#ifndef _MAP_H_
#define _MAP_H_

#include "config.h"

typedef struct {
    int x;
    int y;
    int z;
}Weight;

typedef struct {
    int max_row;
    int max_col;
    int grid_size;
    Weight *wt;
    Tile *tiles;
    int half_grid_flag; //if the head grid of first row is half grid value 0, else value 1
    void *pf;
}Map;

Map* load_map(const char* path, bool open_cache);
void delete_map(Map* m);

#endif