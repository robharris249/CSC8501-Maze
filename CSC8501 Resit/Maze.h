#pragma once
#include <vector>

class Maze {

public:
    struct Node {
        unsigned int x;
        unsigned int y;
        Node* parent;
        bool walkable;
        unsigned int gCost;
        unsigned int hCost;
        unsigned int fCost;
    };

    void printMaze();
    void loadMaze();
    void findNearestCoin();
    void recalculateNodeCosts();
    void pathFinding();
    std::vector<Maze::Node*> getNeighbours(Maze::Maze::Node* n, std::vector<Maze::Node*> closedList);
    bool checkValidNeighbour(Node* n, std::vector<Maze::Node*> closedList);
    bool isOutOfBounds(unsigned int x, unsigned int y);
    void printHcosts();
    void printGcosts();
    void printFcosts();

    std::vector<std::vector<char>> maze;
    std::vector<std::vector<Node*>> pathMap;
    std::vector<Node*> path;
    std::vector<std::pair<unsigned int, unsigned  int>> coins;
    std::pair<unsigned int, unsigned  int> destination;
    std::pair<unsigned int, unsigned int> player;
    std::pair<unsigned int, unsigned  int> entrance;
    int playerHealth = 150;
};

