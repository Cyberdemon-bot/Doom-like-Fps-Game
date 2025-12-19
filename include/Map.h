#pragma once
#include <map>
#include <vector>
class Map
{
private:
    int rows, cols;
    std::map<std::pair<int, int>, int> worldMap;
public:
    bool Init(const std::vector<std::vector<int>>& miniMap);
    std::map<std::pair<int, int>, int>* GetMap();
    bool FindPos(std::pair<int, int> pos) const;
    int GetVal(std::pair<int, int> pos);
    int GetRow() const;
    int GetCol() const;
};
