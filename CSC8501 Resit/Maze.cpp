#include "Maze.h"

#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;



inline bool operator < (const Maze::Maze::Node& lhs, const Maze::Maze::Node& rhs) {
    return lhs.fCost < rhs.fCost;
}

inline bool operator == (const Maze::Maze::Node& lhs, const Maze::Maze::Node& rhs) {
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}

bool isDestination(int x, int y, Maze::Maze::Node dest) {
    if (x == dest.x && y == dest.y) {
        return true;
    }
    else {
        return false;
    }
}

void Maze::loadMaze() {

    ifstream infile("maze.txt");
    string line;

    while (getline(infile, line)) {
        vector<char> mazeRow;

        for (char& c : line) {
            switch (c) {
            case 'x':
                mazeRow.push_back('X');
                break;
            case 'p':
                mazeRow.push_back('P');
                break;
            case 's':
                mazeRow.push_back('S');
                break;
            default:
                mazeRow.push_back(c);
                break;
            }
        }
        maze.push_back(mazeRow);
    }

    for (unsigned int y = 0; y < maze.size(); y++) {
        vector<Node*> pathMapRow;
        for (unsigned int x = 0; x < maze[y].size(); x++) {
            if (maze[y][x] == 'X') {
                pathMapRow.push_back(new Node{x, y, nullptr, false});
            }
            else if (maze[y][x] == 'S') {
                coins.push_back(make_pair(x, y));
                pathMapRow.push_back(new Node{x, y, nullptr, true});
            }
            else if (maze[y][x] == 'P') {
                player.first = x;
                player.second = y;
                entrance.first = x;
                entrance.second = y;
                pathMapRow.push_back(new Node{ x, y, nullptr, true });
            }
            else {
                pathMapRow.push_back(new Node{ x, y, nullptr, true });
            }
        }
        pathMap.push_back(pathMapRow);
    }

    printMaze();

    cout << "Player position: " << player.first << "," << player.second << endl;
}

void Maze::printMaze() {
    for (unsigned int y = 0; y < maze.size(); y++) {
        for (unsigned int x = 0; x < maze[y].size(); x++) {
            cout << '|' << maze[y][x];
        }
        cout << '|' << endl;
    }
    cout << endl;
}

void Maze::findNearestCoin() {
    unsigned int closestCoin = 0;
    vector<pair<unsigned int, unsigned int>> coinOptions; //first = key, second = pathSize

    for (unsigned int i = 0; i < coins.size(); i++) {    //loop through all coins
        destination = coins[i];                          //make destination to currentCoin
        pathFinding();                                   //find path to it
        coinOptions.push_back(make_pair(i, path.size()));//add coin's ID(i) and coin's path length to coinOptions
        path.clear();                                    //clear the path ready for next iteration
    }

    for (unsigned int i = 1; i < coinOptions.size(); i++) {            //loop through coinOptions
        if (coinOptions[i].second < coinOptions[closestCoin].second) { //to find coinOption with shortest path
            closestCoin = i;
        }
    }

    destination = coins[closestCoin];//set destination to the coin with shortest path
    pathFinding();                   //reaquire path to nearest coin
}

void Maze::recalculateNodeCosts() {
    for (unsigned int y = 0; y < maze.size(); y++) {//calculate Hcosts & GCosts based on new destination and new player position
        for (unsigned int x = 0; x < maze[y].size(); x++) {
            pathMap[y][x]->hCost = abs((int)x - (int)destination.first) + abs((int)y - (int)destination.second);
            pathMap[y][x]->gCost = abs((int)x - (int)player.first) + abs((int)y - (int)player.second);
            pathMap[y][x]->fCost = pathMap[y][x]->gCost + pathMap[y][x]->hCost;
        }
    }
}

void Maze::pathFinding() {
    vector<Node*> openList;
    vector<Node*> closedList;
    if (coins.size() == 0) {    //if there aren't any coins to find
        destination = entrance;
    }
    recalculateNodeCosts(); //recalculate all the nodes' costs with new destination
    openList.push_back(pathMap[player.second][player.first]);
    bool targetReached = false;
    Node* currentNode = pathMap[player.second][player.first];

    while (!targetReached) {
        currentNode = openList[0];//set current node to the first node in the open list
        for (unsigned int i = 1; i < openList.size(); i++) {//loop through openList
            if (openList[i]->fCost < currentNode->fCost) {  //to find node with lower Fcost than currentNode
                currentNode = openList[i];                  //this becomes the currentNode
            }
        }
        for (unsigned int i = 0; i < openList.size(); i++) {//loop through openList
            if (openList[i] == currentNode) {               //to find current node
                openList.erase(openList.begin() + i);       //remove current node from openList
                closedList.push_back(currentNode);          //and add it to closedList
            }
        }
        if (currentNode->hCost == 0) {//if current node is the destination (only node with an hcost of 0)
            targetReached = true;
        } else {
            std::vector<Maze::Node*> neighbours = getNeighbours(currentNode, closedList);
            for (unsigned int i = 0; i < neighbours.size(); i++) {    //loop through neighbours of currentNode
                bool inOpenList = false;
                for (unsigned int j = 0; j < openList.size(); j++) {  //then loop through openList
                    if (neighbours[i] == openList[j]) {               //to see if a neighbour is already in OpenList
                        inOpenList = true;
                    }
                }
                if (!inOpenList) {                       //if neighbour isn't in openList
                    neighbours[i]->parent = currentNode; //set the parent of the neighbour to currentNode
                    openList.push_back(neighbours[i]);   //add neighbour to openList
                }
            }
            neighbours.clear();//clear neighbours ready for next iteration
        }
    }
    while (!(currentNode == pathMap[player.second][player.first])) { //loop through path map
        path.insert(path.begin(), currentNode);                      //add currentNode to start of path
        currentNode = currentNode->parent;                           //currentNode becomes its parent
    }                                                                //essentially this will reverse the order into the correct order for the path from player to destination
    path.insert(path.begin(), currentNode);                          //finally add the last currentNode (player's node) to the start of the path
}

std::vector<Maze::Node*> Maze::getNeighbours(Maze::Maze::Node* n, vector<Maze::Node*> closedList) {
    std::vector<Maze::Node*> neighbours;

    if(!isOutOfBounds(n->x, n->y - 1)) {
        if (checkValidNeighbour(pathMap[n->y - 1][n->x], closedList)) {
            neighbours.push_back(pathMap[n->y - 1][n->x]);
        }
    }
    if (!isOutOfBounds(n->x + 1, n->y)) {
        if (checkValidNeighbour(pathMap[n->y][n->x + 1], closedList)) {
            neighbours.push_back(pathMap[n->y][n->x + 1]);
        }
    }
    if (!isOutOfBounds(n->x, n->y + 1)) {
        if (checkValidNeighbour(pathMap[n->y + 1][n->x], closedList)) {
            neighbours.push_back(pathMap[n->y + 1][n->x]);
        }
    }
    if (!isOutOfBounds(n->x - 1, n->y)) {
        if (checkValidNeighbour(pathMap[n->y][n->x - 1], closedList)) {
            neighbours.push_back(pathMap[n->y][n->x - 1]);
        }
    }
    return neighbours;
}

bool Maze::checkValidNeighbour(Maze::Node* n, vector<Maze::Node*> closedList) {
    if (n->walkable) {
        bool isValid = true;
        for (unsigned int i = 0; i < closedList.size(); i++) {
            if (n == closedList[i]) {
                isValid = false;
            }
        }
        return isValid;
    }
    return false;
}

bool Maze::isOutOfBounds(unsigned int x, unsigned int y) {
    if (x > pathMap[y].size() -1 || x < 0) {
        return true;
    }
    else if (y > pathMap.size() -1 || y < 0) {
        return true;
    }
    else {
        return false;
    }
}

void Maze::printGcosts() {
    for (unsigned int y = 0; y < pathMap.size(); y++) {
        for (unsigned int x = 0; x < pathMap[y].size(); x++) {
            if (pathMap[y][x]->gCost < 10) {
                cout << "| " << pathMap[y][x]->gCost;
            } else {
                cout << "|" << pathMap[y][x]->gCost;
            }
        }
        cout << '|' << endl;
    }
    cout << endl;
}

void Maze::printHcosts() {
    for (unsigned int y = 0; y < pathMap.size(); y++) {
        for (unsigned int x = 0; x < pathMap[y].size(); x++) {
            if (pathMap[y][x]->hCost < 10) {
                cout << "| " << pathMap[y][x]->hCost;
            }
            else {
                cout << "|" << pathMap[y][x]->hCost;
            }
        }
        cout << '|' << endl;
    }
    cout << endl;
}

void Maze::printFcosts() {
    for (unsigned int y = 0; y < pathMap.size(); y++) {
        for (unsigned int x = 0; x < pathMap[y].size(); x++) {
            if (pathMap[y][x]->walkable) {
                if (pathMap[y][x]->fCost < 10) {
                    cout << "| " << pathMap[y][x]->fCost;
                }
                else {
                    cout << "|" << pathMap[y][x]->fCost;
                }
            } else {
                cout << "| x";
            }
            
        }
        cout << '|' << endl;
    }
    cout << endl;
}