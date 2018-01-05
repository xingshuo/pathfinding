/**
 * author: xingshuo
 * date: 2017-12-25
 */
#include "config.h"
#include "heapq.h"
#include "pathfind.h"

#include <math.h>

#define DETA  -1

static inline uint64_t
gen_hashkey(PathFinder* pf, int sr, int sc, int er, int ec) {
    uint64_t product = (uint64_t)pf->map->max_col*pf->map->max_row;
    uint64_t a = (uint64_t)sr*pf->map->max_col+sc;
    uint64_t b = (uint64_t)er*pf->map->max_col+ec;
    return (a <= b) ? (a*product + b) : (b*product + a);
}

static void
save_path(PathFinder* pf, Node* enode) {
    Node* cur = enode;
    while (cur) {
        PathNode* node = malloc(sizeof(PathNode));
        node->row = cur->row;
        node->col = cur->col;
        node->next = pf->path;

        pf->path = node;
        cur = cur->parent;
    }
    if (pf->open_cache) {
        uint64_t key = gen_hashkey(pf,pf->start_row,pf->start_col,pf->end_row,pf->end_col);
        if (key != INVALID_KEY) {
            if (enode) {
                hash_add(pf->cache_map, key, pf->path);
            }else {
                hash_add(pf->cache_map, key, pf->noway);
            }
        }
    }
}

static inline void
get_nodeline_vector(PathFinder *pf, Node* n1, Node* n2, float *vx, float *vz) {
    float x1,z1,x2,z2;
    if (is_halfgrid_row(pf->map, n1->row)) {
        x1 = pf->map->grid_size*(n1->col + 1);
    }else {
        x1 = pf->map->grid_size*(n1->col + 0.5);
    }
    z1 = (n1->row + 0.5) * pf->map->grid_size;

    if (is_halfgrid_row(pf->map, n2->row)) {
        x2 = pf->map->grid_size*(n2->col + 1);
    }else {
        x2 = pf->map->grid_size*(n2->col + 0.5);
    }
    z2 = (n2->row + 0.5) * pf->map->grid_size;
    *vx = x2 - x1;
    *vz = z2 - z1;
}

static float
compute_h(PathFinder* pf, Node* node, float path_vx, float path_vz) {
    Node* snode = (Node*)&pf->node_map[pf->start_row * pf->map->max_col + pf->start_col];
    float vx1 = path_vx;
    float vz1 = path_vz;
    float vx2, vz2;
    get_nodeline_vector(pf, snode, node, &vx2, &vz2);
    float val = vx1*vx2 + vz1*vz2;
    float cosval;
    if (val == 0) {
        cosval = 0;
    }else {
        cosval = val/(sqrt(vx1*vx1 + vz1*vz1) * sqrt(vx2*vx2 + vz2*vz2));
    }
    float wx1 = pf->map->wt[ node->row*pf->map->max_col + node->col ].x;
    float wy1 = pf->map->wt[ node->row*pf->map->max_col + node->col ].y;
    float wz1 = pf->map->wt[ node->row*pf->map->max_col + node->col ].z;

    float wx2 = pf->map->wt[ node->row*pf->map->max_col + node->col ].x;
    float wy2 = pf->map->wt[ node->row*pf->map->max_col + node->col ].y;
    float wz2 = pf->map->wt[ node->row*pf->map->max_col + node->col ].z;
    float dist = max(max(abs(wx1 - wx2),abs(wy1 - wy2)),abs(wz1 - wz2));
    return BASE_COST * (dist + DETA*cosval);
}

static inline int
compute_g(Node* node, Node* parent) {
    if (parent) {
        return parent->g + BASE_COST;
    }
    return 0;
}

static void
add_open(PathFinder* pf, int row, int col, Node* node, Node* parent, float path_vx, float path_vz) {
    node->status = STATUS_OPEN;
    node->parent = parent;
    node->row = row;
    node->col = col;
    node->g = compute_g(node, parent);
    node->h = compute_h(pf, node, path_vx, path_vz);
    node->f = node->g + node->h;
    queue_push(pf->open_list, node);
}

bool
find_path(PathFinder* pf, int sr, int sc, int er, int ec) {
    if (pf->open_cache) {
        uint64_t key = gen_hashkey(pf, sr, sc, er, ec);
        if (key != INVALID_KEY) {
            PathNode* node = hash_query(pf->cache_map, key);
            if (node) {
                if (is_access_path(pf, node)) {
                    pf->path = node;
                    return true;
                }else {
                    return false;
                }
            }
        }
    }
    pf->start_row = sr;
    pf->start_col = sc;
    pf->end_row = er;
    pf->end_col = ec;
    Map* map = pf->map;
    memset(pf->node_map, 0, sizeof(Node) * map->max_row * map->max_col);
    reset_queue(pf->open_list);
    Node* snode = (Node*)&pf->node_map[sr * map->max_col + sc];
    snode->row = sr;
    snode->col = sc;
    Node* enode = (Node*)&pf->node_map[er * map->max_col + ec];
    enode->row = er;
    enode->col = ec;
    float path_vx,path_vz;
    get_nodeline_vector(pf, snode, enode, &path_vx, &path_vz);
    add_open(pf, sr, sc, snode, NULL, path_vx, path_vz);
    while (queue_size(pf->open_list) > 0) {
        Node* current = queue_pop(pf->open_list);
        if (current->row == er && current->col == ec) {
            save_path(pf, current);
            return true;
        }
        current->status = STATUS_CLOSE;
        int (*dir_list)[2] = is_halfgrid_row(map, current->row) ? DIR_HALF_LIST : DIR_WHOLE_LIST;
        int i,nr,nc;
        for (i=0; i<6; i++) {
            nr = current->row + *(*(dir_list+i)+0);
            nc = current->col + *(*(dir_list+i)+1);
            if (!is_valid_grid(map, nr, nc)) {
                continue;
            }
            if (is_block_grid(map, nr, nc)) {
                continue;
            }
            Node* neighbour = (Node*)&pf->node_map[nr * map->max_col + nc];
            if (neighbour->status == STATUS_CLOSE) {
                continue;
            }
            if (neighbour->status == STATUS_OPEN) {
                int  g1 = compute_g(neighbour, current);
                if (g1 < neighbour->g) {
                    neighbour->parent = current;
                    neighbour->g = g1;
                    neighbour->f = g1 + neighbour->h;
                }
            }else {
                add_open(pf, nr, nc, neighbour, current, path_vx, path_vz);
            }
        }
    }
    save_path(pf, NULL);
    return false;
}

void
delete_path(PathFinder* pf) {
    if (!pf->open_cache) {
        while (pf->path) {
            PathNode* node = pf->path;
            pf->path = pf->path->next;
            free(node);
        }
    }
    pf->path = NULL;
}

PathFinder*
create_pathfinder(Map* m, bool open_cache) {
    PathFinder* pf = malloc(sizeof(*pf));
    pf->map = m;
    pf->node_map = malloc(m->max_row * m->max_col * sizeof(Node));
    pf->open_list = create_queue(m->max_row * m->max_col);
    pf->path = NULL;
    pf->noway = malloc(sizeof(PathNode));
    pf->open_cache = open_cache;
    pf->cache_map = new_hashmap(16);
    return pf;
}

void
clear_cache(PathFinder* pf) {
    HASH_MAP* hm = pf->cache_map;
    int i;
    for (i=0; i<hm->size; i++) {
        slot * s = &hm->slot_list[i];
        if (s->key!=INVALID_KEY) {
            if (is_access_path(pf, s->value)) {
                PathNode* node = s->value;
                while (node) {
                    PathNode* tmp = node;
                    node = node->next;
                    free(tmp);
                }
            }
            s->key = INVALID_KEY;
            s->value = NULL;
            s->next = -1;
        }
    }
}

void
delete_pathfinder(PathFinder* pf) {
    pf->map = NULL;
    free(pf->node_map);
    delete_queue(pf->open_list);
    delete_path(pf);
    clear_cache(pf);
    delete_hashmap(pf->cache_map);
    free(pf->noway);
    free(pf);
}