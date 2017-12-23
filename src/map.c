#include "config.h"
#include "map.h"
#include "pathfind.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

static Map*
create_map(int row_num, int col_num, int grid_size, Tile* tiles, int halfg_flag) {
    Map* m = malloc(sizeof(*m));
    m->max_row = row_num;
    m->max_col = col_num;
    m->grid_size = grid_size;
    m->tiles = tiles;
    m->half_grid_flag = halfg_flag;
    m->wt = malloc(row_num * col_num * sizeof(Weight));
    int sr = 0;
    int sc = 0;
    Weight* sw = &m->wt[sr*col_num + sc];
    sw->x = 0;
    sw->y = 0;
    sw->z = 0;
    typedef struct {
        int row;
        int col;
    }Pos;
    uint8_t marked[row_num*col_num];
    memset(marked, 0, sizeof(uint8_t)*row_num*col_num);
    Pos queue[row_num*col_num];
    int head = 0;
    int tail = 0;
    Pos ps = {sr,sc};
    queue[tail++] = ps;
    while (head < tail) {
        Pos pcur = queue[head++];
        int cr = pcur.row;
        int cc = pcur.col;
        int (*dir_list)[2]  = is_halfgrid_row(m, cr) ? DIR_HALF_LIST : DIR_WHOLE_LIST;
        int i,nr,nc;
        int wx = m->wt[cr*col_num + cc].x;
        int wy = m->wt[cr*col_num + cc].y;
        int wz = m->wt[cr*col_num + cc].z;
        for (i=0; i<6; i++) {
            nr = cr + *(*(dir_list+i)+0);
            nc = cc + *(*(dir_list+i)+1);
            if (!is_valid_grid(m, nr, nc)) {
                continue;
            }
            if (marked[nr*col_num + nc]) {
                continue;
            }
            m->wt[nr*col_num + nc].x = wx + WEIGHT_CONFIG[i][0];
            m->wt[nr*col_num + nc].y = wy + WEIGHT_CONFIG[i][1];
            m->wt[nr*col_num + nc].z = wz + WEIGHT_CONFIG[i][2];
            marked[nr*col_num + nc] = 1;
            Pos pn = {nr,nc};
            queue[tail++] = pn;
        }
    }
    return m;
}

Map*
load_map(const char* path, bool open_cache) {
    FILE* fp = fopen(path, "r");
    if(fp == NULL) {
        printf("load map failed!:<%s>\n", path);
        return NULL;
    }
    int row_num,col_num,grid_size;
    int n = fscanf(fp, "%d\n%d\n%d\n", &row_num, &col_num, &grid_size);
    if (n != 3) {
        fclose(fp);
        printf("read map data head error!:<%s><%d>\n", path, n);
        return NULL;
    }
    uint32_t tile_num = row_num*col_num;
    Tile *tiles = malloc(tile_num * sizeof(Tile));
    assert(tiles);
    uint32_t nread = fread(tiles, sizeof(Tile), tile_num, fp);
    fclose(fp);
    if (nread != tile_num) {
        printf("read map data body error!:<%s>%d %d %d\n", path, row_num, col_num, grid_size);
        return NULL;
    }
    Map* m = create_map(row_num, col_num, grid_size, tiles, 1);
    PathFinder* pf = create_pathfinder(m, open_cache);
    m->pf = pf;
    return m;
}

void
delete_map(Map* m) {
    free(m->wt);
    free(m->tiles);
    delete_pathfinder(m->pf);
    free(m);
}