
FILE_PATH = ...

math.randomseed(os.clock()*1000000)

ROL_NUM = 20
COL_NUM = 20
BLOCK_SIZE = 36 --must be 2*n

function dumpmap(map_data, block_size, path)
    local sformat = string.format 
    local f = io.open(path, "w")
    local row_num = #map_data
    local col_num = #map_data[1]
    f:write(sformat("%s\n",row_num))
    f:write(sformat("%s\n",col_num))
    f:write(sformat("%s\n",block_size))
    local s = ""
    for i=1,row_num do
        local m = {}
        for j=1,col_num do
            local v = map_data[i][j]
            m[j] = string.char(v)
        end
        s = s .. table.concat(m,"")
    end
    f:write(s)
    f:close()
    print(sformat("---dump map data to %s ok---\n",path))
end

MAP = {}

for i=1,ROL_NUM do
    for j=1,COL_NUM do
        if not MAP[i] then
            MAP[i] = {}
        end
        if math.random(1,10) <= 3 then
            MAP[i][j] = 1
        else
            MAP[i][j] = 0
        end
    end
end

dumpmap(MAP, BLOCK_SIZE, FILE_PATH)