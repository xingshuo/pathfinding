#ifndef _PATHFIND_H_
#define _PATHFIND_H_

#include "config.h"
#include "map.h"
#include "heapq.h"
#include "hash.h"

#define STATUS_NO 0
#define STATUS_OPEN 1
#define STATUS_CLOSE 2

#define BASE_COST 10

typedef struct PathNode {
  int row;
  int col;
  struct PathNode* next;
} PathNode;

typedef struct Node {
  int row;
  int col;
  float f;
  int g;
  float h;
  int status;
  struct Node* parent;
  int pos;
} Node;

typedef struct {
    int start_row;
    int start_col;
    int end_row;
    int end_col;
    Map* map;
    Node* node_map;
    HeapQueue* open_list;
    PathNode* path;
    PathNode* noway;
    bool open_cache;
    HASH_MAP* cache_map;
} PathFinder;

PathFinder* create_pathfinder(Map* m, bool open_cache);
void delete_pathfinder(PathFinder* pf);
bool find_path(PathFinder* pf, int sr, int sc, int er, int ec);
void delete_path(PathFinder* pf);
void clear_cache(PathFinder* pf);

#endif