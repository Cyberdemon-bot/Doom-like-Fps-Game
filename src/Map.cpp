#include "Map.h"

bool Map::Init (const std::vector<std::vector<int>>& miniMap)
{
    rows = miniMap.size(), cols =  miniMap[0].size();
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            if(miniMap[i][j])
            {
                std::pair<int, int> pos = {i, j};
                worldMap[pos] = miniMap[i][j];
            }
    return true;
}

std::map<std::pair<int, int>, int>* Map::GetMap() {return &worldMap;}

bool Map::FindPos(std::pair<int, int> pos)  const
{
    if(worldMap.find(pos) != worldMap.end()) return true;
    return false;
}

int Map::GetVal(std::pair<int, int> pos)
{
    return worldMap[pos];
}

int Map::GetRow()  const{return rows;}

int Map::GetCol()  const{return cols;}


