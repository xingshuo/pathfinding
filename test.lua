local cmap = require "cmap"
local canvas = require "canvas"

local map_file = ...
local m = cmap.create(map_file)
print("<font  color='blue'>",m,"row:",m:info("max_row"),"col:",m:info("max_col"),"gride_size:",m:info("grid_size"),"</font>","<br>")

local c = canvas.new()

local COLOR_WHITE = "#FFFFFF"
local COLOR_BLACK = "#000000"
local COLOR_BLUE  = "#00FFFF"
local COLOR_GREEN = "#00FF00"
local COLOR_RED   = "#FF0000"
local COLOR_DARKBLUE = "#0000FF"

local GRID_SIZE = m:info("grid_size")
local MAX_ROW = m:info("max_row")
local MAX_COL = m:info("max_col")

math.randomseed(os.clock()*100000)

local sr,sc,er,ec
for i=1,10 do
    sr,sc = math.random(1,MAX_ROW)-1,math.random(1,MAX_COL)-1
    if not m:is_block(sr, sc) then
        break
    end
end
for i=1,10 do
    er,ec = math.random(1,MAX_ROW)-1,math.random(1,MAX_COL)-1
    if not m:is_block(er, ec) then
        break
    end
end

local ok,path = m:find_path(sr,sc, er,ec)
local patch = {[sr*MAX_COL+sc] = 1, [er*MAX_COL+ec] = 1}
if ok then
    for _,pos in pairs(path) do
        patch[pos.row * MAX_COL + pos.col] = 1
    end
end

-------draw squares-------
for i=0,MAX_ROW-1 do
    for j=0,MAX_COL-1 do
        local x,y
        if i%2 == m:info("half_grid_flag") then --half grid row
            x = (0.5+j)*GRID_SIZE
            y = i*GRID_SIZE
        else
            x = j*GRID_SIZE
            y = i*GRID_SIZE
        end
        local color
        if m:is_block(i,j) then --is block
            color = COLOR_RED
            if i == sr and j == sc or i == er and j == ec then
                color = COLOR_DARKBLUE
            end
        else
            color = COLOR_WHITE
            if patch[i*MAX_COL + j] then
                if i == sr and j == sc or i == er and j == ec then
                    color = COLOR_BLUE
                else
                    color = COLOR_GREEN
                end
            end
        end
        c:rect(x, y, GRID_SIZE, GRID_SIZE, color)
    end
end

-------draw lines-------
for i=0,MAX_ROW do
    local y = i*GRID_SIZE
    local x1, x2 = 0, (MAX_COL+0.5)*GRID_SIZE
    c:line(x1,y, x2,y)
end

for i=0,MAX_ROW-1 do
    for j=0,MAX_COL-1 do
        local x
        if i%2 == m:info("half_grid_flag") then
            x = GRID_SIZE*(0.5+j)
        else
            x = GRID_SIZE*(1 + j)
        end
        local y1 = i*GRID_SIZE
        local y2 = (i+1)*GRID_SIZE
        c:line(x,y1, x,y2)
    end
end

--------test gc-------
m = nil
collectgarbage("collect")

print(c:html())