#include "config.h"
#include "map.h"
#include "pathfind.h"

#include "lua.h"
#include "lauxlib.h"

#define MAP_METATABLE "map_meta"

#define check_map(L, idx)\
    *(Map**)luaL_checkudata(L, idx, MAP_METATABLE)

#define push_pos(L, row, col)\
    do {    \
        lua_newtable(L);    \
        lua_pushstring(L, "row");   \
        lua_pushinteger(L, row);  \
        lua_rawset(L,-3);   \
        lua_pushstring(L, "col");   \
        lua_pushinteger(L, col);  \
        lua_rawset(L,-3);   \
    }while (0);

static int
new_map(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    if (path == NULL) {
        return luaL_argerror(L, 1, "load map path err");
    }
    bool open_cache = lua_toboolean(L, 2);
    Map* m = load_map(path, open_cache);
    *(Map**)lua_newuserdata(L, sizeof(void*)) = m;
    luaL_getmetatable(L, MAP_METATABLE);
    lua_setmetatable(L, -2);
    return 1;
}

static int
_delete_map(lua_State *L) {
    Map* m = check_map(L, 1);
    delete_map(m);
    return 0;
}

static int
_tostring(lua_State *L) {
    Map* m = check_map(L, 1);
    lua_pushfstring(L, "[cmap: %p]", m);
    return 1;
}

static int
lset_block(lua_State *L) {
    Map* m = check_map(L, 1);
    int row = luaL_checknumber(L, 2);
    int col = luaL_checknumber(L, 3);
    Tile tile = luaL_checknumber(L, 4);
    if (is_valid_grid(m, row, col) && m->tiles[row*m->max_col + col] != tile) {
        m->tiles[row*m->max_col + col] = tile;
        PathFinder* pf = m->pf;
        if (pf->open_cache) {
            clear_cache(pf);
        }
    }
    return 0;
}

static int
lget_block(lua_State *L) {
    Map* m = check_map(L, 1);
    int row = luaL_checknumber(L, 2);
    int col = luaL_checknumber(L, 3);
    if (is_valid_grid(m, row, col)) {
        lua_pushinteger(L, m->tiles[row*m->max_col + col]);
        return 1;
    }else{
        return 0;
    }
}

static int
lfind_path(lua_State *L) {
    Map* m = check_map(L, 1);
    int srow = luaL_checknumber(L, 2);
    int scol = luaL_checknumber(L, 3);
    int erow = luaL_checknumber(L, 4);
    int ecol = luaL_checknumber(L, 5);
    if (!is_valid_grid(m, srow, scol) || is_block_grid(m, srow, scol)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    if (!is_valid_grid(m, erow, ecol) || is_block_grid(m, erow, ecol)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    bool find = find_path(m->pf, srow, scol, erow, ecol);
    if (find) {
        lua_pushboolean(L, 1);
        lua_newtable(L);
        int idx = 0;
        PathNode* node = ((PathFinder*)(m->pf))->path;
        while (node) {
            lua_pushinteger(L, ++idx);
            push_pos(L, node->row, node->col);
            lua_rawset(L,-3);
            node = node->next;
        }
        delete_path(m->pf);
        return 2;
    }else {
        lua_pushboolean(L, 0);
        return 1;
    }
}

static int
lget_noblocking_pos(lua_State *L) {
    Map* m = check_map(L, 1);
    int length = m->max_row*m->max_col;
    int rnd_pos = rand()%length;
    int i;
    for (i=rnd_pos; i<length; i++) {
        int r = i/m->max_col;
        int c = i%m->max_col;
        if (!is_block_grid(m, r, c)) {
            push_pos(L, r, c);
            return 1;
        }
    }
    for (i=rnd_pos-1; i>=0; i--) {
        int r = i/m->max_col;
        int c = i%m->max_col;
        if (!is_block_grid(m, r, c)) {
            push_pos(L, r, c);
            return 1;
        }
    }
    return 0;
}

static int
lis_block(lua_State *L) {
    Map* m = check_map(L, 1);
    int row = luaL_checknumber(L, 2);
    int col = luaL_checknumber(L, 3);
    if (!is_valid_grid(m, row, col))
        lua_pushboolean(L, 1);
    else
        lua_pushboolean(L, is_block_grid(m, row, col));
    return 1;
}

static int
lset_opencache(lua_State *L) {
    Map* m = check_map(L, 1);
    bool open = lua_toboolean(L, 2);
    PathFinder* pf = m->pf;
    pf->open_cache = open;
    clear_cache(pf);
    return 0;
}

static int
lis_opencache(lua_State *L) {
    Map* m = check_map(L, 1);
    PathFinder* pf = m->pf;
    lua_pushboolean(L, pf->open_cache);
    return 1;
}

static int
linfo(lua_State *L) {
    Map* m = check_map(L, 1);
    const char *which = luaL_optstring(L, 2, NULL);
    if (which == NULL) {
        lua_newtable(L);
        lua_pushstring(L, "max_row");
        lua_pushinteger(L, m->max_row);
        lua_rawset(L,-3);

        lua_pushstring(L, "max_col");
        lua_pushinteger(L, m->max_col);
        lua_rawset(L,-3);

        lua_pushstring(L, "grid_size");
        lua_pushinteger(L, m->grid_size);
        lua_rawset(L,-3);

        lua_pushstring(L, "half_grid_flag");
        lua_pushinteger(L, m->half_grid_flag);
        lua_rawset(L,-3);
        return 1;
    }else {
        if (!strcasecmp(which, "max_row")) {
            lua_pushinteger(L, m->max_row);
            return 1;
        }else if(!strcasecmp(which, "max_col")) {
            lua_pushinteger(L, m->max_col);
            return 1;
        }else if(!strcasecmp(which, "grid_size")) {
            lua_pushinteger(L, m->grid_size);
            return 1;
        }else if(!strcasecmp(which, "half_grid_flag")) {
            lua_pushinteger(L, m->half_grid_flag);
            return 1;
        }else {
            return 0;
        }
    }
}

int
luaopen_cmap(lua_State* L) {
    srand(time(NULL));
    luaL_checkversion(L);
    luaL_Reg l1[] = {
        {"create", new_map},
        {NULL, NULL},
    };
    luaL_Reg l2[] = {
        {"find_path", lfind_path},
        {"set_block", lset_block},
        {"get_block", lget_block},
        {"get_noblocking_pos", lget_noblocking_pos},
        {"is_block", lis_block},
        {"set_opencache", lset_opencache},
        {"is_opencache", lis_opencache},
        {"info", linfo},
        {NULL,NULL},
    };
    luaL_newmetatable(L, MAP_METATABLE);
    luaL_newlib(L, l2);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, _delete_map);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, _tostring);
    lua_setfield(L, -2, "__tostring");

    luaL_newlib(L, l1);
    return 1;
}