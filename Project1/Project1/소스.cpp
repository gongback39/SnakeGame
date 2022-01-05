#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>

enum {
	LEFT=-2, DOWN, NON_DIR, UP, RIGHT //절댓값을 활용해 좌.우 / 상,하 이동으로 인한 게임오버 방지
};

enum {
	EMPTY, BODY, HEAD, FOOD 
};

enum {
	SPRITE, DIRECTION //SPRITE에 EMPTY BODY HEAD FOOD지정, DIRECTION에 LEFT DOWN UP RIGHT지정
};


struct Snake {
	int length;
	std::pair<int, int> head;
	std::pair<int, int> tail;

	Snake(int n) : length(n) {}
};

class SnakeGame {
	int W, H;
	std::vector<std::vector<std::vector <int>>> board;
	Snake snake;
	int initLen;
	int delay;
public:
	SnakeGame(int w, int h, int length, int delay) : W(w), H(h), initLen(length), snake(length), delay(delay) {
		board.resize(H);
		for (int y = 0; y < H; ++y) {
			board[y].resize(W);
			for (int x = 0; x < W; ++x)
				board[y][x].resize(2); 
		}

		snake.head = std::make_pair(H/2, snake.length-1);
		snake.tail = std::make_pair(H/2, 0);

		for (int i = 0; i < snake.length-1; i++) {
			board[H/2][i][DIRECTION] = RIGHT; 
		}

		board[H/2][snake.length-1][SPRITE] = HEAD;
		board[H/2][snake.length-1][DIRECTION] = RIGHT;

		int x, y;
		do {
			y = rand() % H;
			x = rand() % W;
		} while (board[y][x][SPRITE] != EMPTY); 
		board[y][x][SPRITE] = FOOD; //임의로 지정된 좌표가 EMPTY이면 FOOD 대입
	}
	void DrawScene() {
		system("cls");
		//for (int y = 0; y < H+3; y++)
		//	std::cout << "\x1b[A"; // https://tforgione.fr/posts/ansi-escape-codes/

		for (int x = 0; x < W+2; x++) {
			std::cout << '=';
		}
		std::cout << '\n';
		for (int y = 0; y < H; y++) {
			std::cout << '|';
			for (int x = 0; x < W; x++) {
				if (board[y][x][SPRITE] == BODY) std::cout << '+';
				else if (board[y][x][SPRITE] == HEAD) std::cout << '@';
				else if (board[y][x][SPRITE] == FOOD) std::cout << '*';
				else std::cout << ' ';
			}
			std::cout << '|' << '\n';
		}
		for (int x = 0; x < W+2; x++) 
			std::cout << '=';
		std::cout << '\n';
	} //보드 생성 및 업뎃
	static int GetDirection() {
		int rtn = NON_DIR, ch;
		ch = _getch();
		switch (ch) {
			case 72: rtn = UP; 		break;
			case 75: rtn = LEFT; 	break;
			case 77: rtn = RIGHT; 	break;
			case 80: rtn = DOWN; 	break;
		}

		return rtn;
	} //방향키 입력
	void GameLoop() {
		DrawScene();
		int start, current = RIGHT, ret = RIGHT, x, y;
		bool bEat = false;
		while (true) {
			start = clock();
			while (1000*(clock() - start)/CLOCKS_PER_SEC <= delay) {
				if (_kbhit()) {
					current = GetDirection();
				}
			}
			// Head
			y = snake.head.first;
			x = snake.head.second;
			board[y][x][SPRITE] = BODY;
			
			if(current == NON_DIR)
				current = ret;
			if (current != -ret) {
				board[y][x][DIRECTION] = current;
				ret = current;
			}
			else if (current == -ret) {
				board[y][x][DIRECTION] = ret;
			}// 상하/좌우 이동으로 인한 게임종료 방지
			
			switch (board[y][x][DIRECTION]) {
				case UP: snake.head.first--; 		break;
				case RIGHT: snake.head.second++; 	break;
				case DOWN: snake.head.first++; 		break;
				case LEFT: snake.head.second--; 	break;
			} //방향 전환 

			// New head
			y = snake.head.first;
			x = snake.head.second;
			if (y < 0 || x < 0 || y >= H || x >= W || board[y][x][SPRITE] == BODY) 
				break;
			if (board[y][x][SPRITE] == FOOD) bEat = true;
			board[y][x][0] = HEAD; //이동하는 head 구현

			//Tail
			if (bEat == false) { //No food was eaten 
				y = snake.tail.first;
				x = snake.tail.second;
				board[y][x][SPRITE] = EMPTY;
				switch (board[y][x][DIRECTION]) {
					case UP: snake.tail.first--; 		break;
					case RIGHT: snake.tail.second++; 	break;
					case DOWN: snake.tail.first++; 		break;
					case LEFT: snake.tail.second--; 	break;
				} //꼬리의 방향 변화
			}
			else { // eat food 
				snake.length++;
				do {
					y = rand() % H;
					x = rand() % W;
				} while (board[y][x][SPRITE] != EMPTY);
				board[y][x][SPRITE] = FOOD;
				bEat = false; //새로운 Food 생성

				delay-=10;
				if(delay < 10) delay = 10; // 화면 갱신 지연시간
			}
			DrawScene();
			std::cout << "Score: " << snake.length - initLen << std::endl;// FOOD를 먹어 길어진 많큼 점수 부여
		}
	}
};

int main() {
	srand(unsigned(time(nullptr)));
	SnakeGame game(40, 24, 4, 150); //보드 X크기, Y크기 , SNAKE 최소길이, 게임 업뎃 딜레이
	game.GameLoop();
	return 0;
}