## Pathfinding
A kind of 7-Grids pathfinding algorithm based on A*,the method of calculating H referred to https://www.redblobgames.com/grids/hexagons/<br>
It's make sure the path is as short as possible and approximates the connection line between the start and end points.<br>
Like this:<br>
![path](https://github.com/xingshuo/pathfinding/blob/master/path.png)<br>
The lua-binding apis were written in lua-map.c with detailed description.<br>
It's also support pathfinding cache function that you can choose open or not.
## Build
```
make
```

## Test
```
1. sh run.sh or sh run.sh -D

2. Open c.html in browser
```