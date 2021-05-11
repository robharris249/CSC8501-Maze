#include "Maze.h"
#include <iostream>
#include <Windows.h>

void move(Maze* m);

int main() {
	Maze* m = new Maze();
	bool exitReached = false;

	m->loadMaze();
	m->findNearestCoin();

	bool keyPressFlag = true;
	while (m->coins.size() > 0) {
		
		if (GetKeyState(VK_SPACE) & 0x8000) {//if Spacebar has been pressed
			if (keyPressFlag) {//this is to ensure code runs only once per sapce bar press
				move(m);
				std::cout << "Player Health = " << m->playerHealth << std::endl;
				keyPressFlag = false;
			}
		} else {
			keyPressFlag = true;
		}

	}

	m->pathFinding();

	while (!exitReached) {

 		if (GetKeyState(VK_SPACE) & 0x8000) {//if Spacebar has been pressed
			if (keyPressFlag) {//this is to ensure code runs only once per sapce bar press
				move(m);
				std::cout << "Player Health = " << m->playerHealth << std::endl;
				keyPressFlag = false;
			}
		} else {
			keyPressFlag = true;
		}
		if (m->player == m->entrance) {
			exitReached = true;
		}
	}
	std::cout << "Player Health = " << m->playerHealth << std::endl;
	return 0;
}

static void move(Maze* m) {

	m->player.first = m->path[1]->x;
	m->player.second = m->path[1]->y;
	m->maze[m->path[1]->y][m->path[1]->x] = 'P';
	m->maze[m->path[0]->y][m->path[0]->x] = ' ';
	m->path.erase(m->path.begin());
	m->printMaze();
	m->playerHealth--;
	if (m->coins.size() != 0) {
		if (m->path.size() < 2) {
			m->playerHealth += 11;
			for (unsigned int i = 0; i < m->coins.size(); i++) {
				if (m->coins[i].first == m->player.first && m->coins[i].second == m->player.second) {//remove taken coin from list
					m->coins.erase(m->coins.begin() + i);
				}
			}
			m->path.erase(m->path.begin());
			if (m->coins.size() != 0) {
				m->findNearestCoin();
			}
		}
	}
}