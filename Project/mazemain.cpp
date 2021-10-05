#include<iostream>
#include<vector>
#include<fstream>
#include<Windows.h>
#include<conio.h>
#include<math.h>
#include<stdlib.h>
#include"mazeStack.h"
#include"mazeList.h"
#include <string.h>
#include <iomanip>

using namespace std;

void Init(); // 쥐의 정보를 초기화하는 함수이다.
void readFile(char* filename); // 파일을 읽어 미로의 크기를 파악한다. 1을 벽, 0을 길로 표시하고 출구를 계산한다.
int findPathA(int row, int col); // A* 알고리즘을 구현한 함수이다.
void printMap(); // 벽이라면 □를 길이면 " "를 출력하여 실제 미로처럼 표현한다.
void printMouse(int x, int y); // 입력받은 위치에 쥐를 출력한다.
void printPath(int x, int y, int TYPE); // 입력받은 위치에 ■를 출력하며 TYPE에 따라 다른 색을 출력한다.
void MapInformation(); // 맵의 정보와 쥐의 초기 에너지를 출력한다.
void gotoxy(int x, int y); //커서의 위치를 움직이는 함수이다.
void txtcolor(unsigned short color); //winAPI를 사용하여 색을 변경하는 함수이다.
void writeFile(char* filename2);

//미로와 관련된 정의
#define PATH 0 // 길
#define WALL 1 // 벽
#define GMOVE 4 // 쥐가 지나간 길
#define GBACK 5 // 쥐가 되돌아간 길
#define TELL 6 // 갈림길
#define EXIT 60 // 출구
#define DELAY 10 // 알고리즘이 들어간 부분의 시뮬레이션 딜레이

int **map = NULL; // 맵 동적할당
int rows, cols;
int exit_row = 0;
int exit_col = 0;
int tel; // 상하좌우 중 PATH의 수. 2 이상이면 갈림길로 간주한다.
int cnt; // 상하좌우 중 PATH가 아닌 길의 수
int c = 0;

typedef struct Mouse // 쥐 구조체 선언
{
	int energy, x = 0, y = 1;
	double mana;
} Mouse;

Mouse mouse;

typedef struct Point
{      //포인트 구조체
	int row, col;
	int F, G, H;
} Point;

stackClass FinalPath; //최단 경로 중 알고리즘을 따라 간 경로
stackClass telStack;

int main(void)
{
	char filename[100];
	cout << "미로 파일 이름을 입력하세요: ";
	cin >> filename;
	readFile(filename);
	Init();
	printMap();
	MapInformation();
	_getch();
	if (findPathA(mouse.x, mouse.y) == EXIT) // 출구를 찾으면
	{
		if (FinalPath.Size() > 0) // 최단경로 스택이 빌 때 까지
		{
			while (true)
			{
				txtcolor(6);
				gotoxy(FinalPath.Top->col * 2, FinalPath.Top->row); // 스택에 저장된 위치로 커서를 옮기고
				cout << "■"; // 노란색 네모를 출력한다.
				FinalPath.Pop();
				if (FinalPath.IsEmpty()) break; // 최단경로 스택이 비면 반복문을 종료한다.
			}
		}
	}
	else // 출구를 찾지 못하면
	{
		gotoxy(cols * 2 + 2, 5);
		cout << "출구를 찾지 못했습니다." << endl;
	}
	fflush(stdin);
	getchar();
	getchar();
	getchar();

	char filename2[100] = "result_";
	strcat_s(filename2, filename);
	writeFile(filename2);
	return 0;
}

void Init()
{
	mouse.mana = 0; // 초기 마나 = 0
	mouse.energy = cols * rows * 2; // 초기 에너지 = 열 * 행 * 2
}

void readFile(char* filename)
{
	ifstream file(filename);
	rows = 0;
	cols = 0;

	while (!file.eof())
	{
		char c = (char)file.get();
		if (c == '1' || c == '0' || c == '\n') // 이 외의 문자는 미로의 정보를 나타내지 정보가 아니다.
		{
			if (c == '\n') // 열 길이 증가
				rows++;
			if (rows == 0) // 행 길이 증가
				cols++;
		}
	}
	++rows;

	file.clear();
	file.seekg(0); // 파일포인터 맨 앞으로 이동

	int row = 0;
	int col = 0;
	map = new int*[rows]; // 동적할당
	map[row] = new int[cols];

	while (!file.eof())
	{
		char c = (char)file.get();
		if (c == '0' || c == '1' || c == '\n')
		{
			if (c == '\n')
			{
				row++;
				if (row == rows) break;
				col = 0;
				map[row] = new int[cols];
				continue;
			}

			if (c == '1') map[row][col] = WALL; // 벽
			else if (c == '0') map[row][col] = PATH; // 길
			col++;
		}
	}

	//출구를 찾는 코드이다.
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			if ((map[0][j] == 0) && j != 1) { //맨 윗줄에 0(PATH)이 있고, 그 길이 입구(0,1)가 아니라면
				exit_row = 0; exit_col = j;
			}
			else if (map[i][0] == 0) { // 제일 왼쪽 PATH가 있다면
				exit_row = i; exit_col = 0;
			}
			else if (map[rows - 1][j] == 0) { // 제일 오른쪽에 PATH가 있다면
				exit_row = rows - 1; exit_col = 0;
			}
			else if (map[i][cols - 1] == 0) { //제일 아랫 줄에 PATH가 있다면
				exit_row = i; exit_col = cols - 1;
			}

		}

	map[exit_row][exit_col] = EXIT; // 그 위치를 EXIT로 저장한다.
	file.close();
}
void writeFile(char* filename2) {
	ofstream file(filename2);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == PATH)
				file << setw(2) << '0';
			else if (map[i][j] == GMOVE || map[i][j] == GBACK || map[i][j] == TELL)
				file << setw(2) << '*';
			else
				file << setw(2) << '1';

			//file.width(2);

		}
		file << endl;
	}
	file.close();
}
int findPathA(int row, int col)
{
	List<Point> sels; // 현재위치에서 갈 수 있는 곳을 저장하는 리스트
	tel = 0;
	cnt = 0;

	if (map[row][col] == EXIT)
		return EXIT;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((abs(i + j) % 2 == 1) && row + i >= 0 && row + i < rows && col + j >= 0 && col + j < cols && !(i == 0 && j == 0)) //대각선 배제
			{
				if ((map[row + i][col + j] == PATH) || (map[row + i][col + j] == EXIT))  // PATH나 EXIT이라면
				{
					tel++; // PATH임을 증가시킨다.
					Point *point = new Point; // 가중치 저장 및 계산을 위한 포인터
					point->row = row + i;
					point->col = col + j;
					point->G = FinalPath.Size() + 1;
					point->H = abs(exit_row - point->row) + abs(exit_col - point->col);
					point->F = point->G + point->H;
					sels.push(*point); // 리스트에 삽입한다.

					if (tel >= 2) // 갈림길이라면
					{
						telStack.Push(row, col); // 갈림길스택에 현재 위치를 삽입한다.
					}
				}
				else // PATH나 EXIT이 아니라면
					cnt++;
			}

		}
	}


	for (int i = 0; i < sels.size() - 1; i++) // F를 기준으로 비내림차순으로 정렬한다.(버블정렬)
	{
		for (int j = 0; j < sels.size() - 1; j++)
		{
			if (sels.get(j).F > sels.get(j + 1).F)
			{
				Point hold = sels.get(j);
				sels.replace(sels.get(j + 1), j);
				sels.replace(hold, j + 1);
			}
		}
	} //이동할 수 있는 위치에 우선순위 부여한다.

	int a[2000];
	int b[2000];

	if (cnt == 4) // 상하좌우 모두 PATH나 EXIT이 아니라면
	{
		c = 0; // count 0으로 초기화 (돌아와야하는 길의 수)
		mouse.x = telStack.Top->row; // 쥐의 위치 설정
		mouse.y = telStack.Top->col;

		while (mouse.x != FinalPath.Top->row || mouse.y != FinalPath.Top->col) // 가장 최근에 만난 갈림길까지 되돌아가야한다.
		{
			// 되돌아가야하는 경로를 저장하는 배열
			a[c] = FinalPath.Top->row;
			b[c] = FinalPath.Top->col;
			c++; // 돌아가야하는 길의 수 증가 
			FinalPath.Pop();
		}

		if (c > 4 && mouse.mana >= 5) // 텔레포트 할 조건을 만족하면
		{
			int i = 0;
			for (i; i < c; i++)
			{
				map[a[i]][b[i]] = TELL; //이동했던길은 TELL로표시
				printPath(b[i], a[i], TELL); //DELAY 없이 한번에 표시한다.
			}
			printMouse(mouse.y, mouse.x); // 이동한 위치에 쥐를 출력
			map[mouse.x][mouse.y] = TELL;
			Sleep(DELAY);
			printPath(mouse.y, mouse.x, TELL);
			mouse.mana -= 5.0;
			telStack.Pop();
		}
		else // 텔레포트 할 조건이 되지 않으면
		{
			int i = 0;
			for (i; i < c; i++)
			{ // 이동하는 길을 하나씩 출력한다.
				printMouse(b[i], a[i]);
				map[a[i]][b[i]] = GBACK;
				Sleep(DELAY);
				printPath(b[i], a[i], GBACK);
				mouse.mana += 0.1; // 한칸마다 쥐의 정보는 변경한다.
				mouse.energy -= 1;
			}
			telStack.Pop(); // 갈림길 스택을 삭제한다.
		}
	}


	if (sels.size() > 0) //  갈 수 있는 길이 있으면
	{
		map[mouse.x][mouse.y] = GMOVE; // 현재 위치 정보를 변경하고
		mouse.x = sels.get(0).row; // 가중치가 가장 작은 곳의 위치를 저장한다.
		mouse.y = sels.get(0).col;
		sels.erase(0); // 삭제

		FinalPath.Push(mouse.x, mouse.y); // 저장된 쥐의 위치를 최단경로 스택에 저장하고
		printMouse(mouse.y, mouse.x); // 쥐를 출력한다.
		Sleep(DELAY);
		printPath(mouse.y, mouse.x, GMOVE);
		mouse.mana += 0.1; // 쥐의 정보를 변경한다.
		mouse.energy -= 1;
	}
	return findPathA(mouse.x, mouse.y);
}


void printMap()
{
	system("cls");
	txtcolor(8); // 회색
	for (int i = 0; i < rows; i++) // 열
	{
		for (int j = 0; j < cols; j++) // 행
		{
			if (map[i][j] == WALL)   // WALL이면 (1)
			{
				cout << "□";
			}
			else // PATH면 (0)
			{
				cout << "  ";
			}
		}
		cout << endl;// 줄 바꿈
	}

	gotoxy(2, 0);
	txtcolor(14);
	cout << "찍";
}

void printMouse(int x, int y)
{
	txtcolor(15);
	gotoxy(cols * 2 + 2, 13);
	cout << "에너지: " << mouse.energy << "　　　";
	gotoxy(cols * 2 + 2, 15);
	cout << "마나: " << mouse.mana << "       ";

	txtcolor(14);
	gotoxy(x * 2, y);
	cout << "찍";// 쥐 모양

	if (mouse.energy <= 0) // 에너지를 모두 소모하면
	{
		gotoxy(cols * 2 + 2, 35);
		txtcolor(12);
		cout << "출구 찾기 실패! 에너지가 없습니다." << endl;
		_getch();
		exit(0);
	}

}

void printPath(int x, int y, int TYPE) // 이동한 모든 경로를 출력한다.
{
	gotoxy(x * 2, y);
	cout << " ";

	if (TYPE == GMOVE) // 지나온 길 출력 (하얀색)
	{
		txtcolor(15);
		gotoxy(x * 2, y);
		cout << "■";
	}
	if (TYPE == GBACK) // 되돌아간 길 출력 (녹색)
	{
		txtcolor(2);
		gotoxy(x * 2, y);
		cout << "■";
	}
	if (TYPE == TELL) { // 텔레포트한 길 (직접 지나가지 않음, 에메랄드 색)
		txtcolor(3);
		gotoxy(x * 2, y);
		cout << "■";
	}
}

void MapInformation()         //전체적인 상황 출력
{
	gotoxy(cols * 2 + 2, 5);
	txtcolor(15);
	cout << "<알고리쥐>" << endl;
	gotoxy(cols * 2 + 2, 7);
	cout << "미로크기 : " << rows << " x " << cols;
	gotoxy(cols * 2 + 2, 9);
	cout << "총 에너지: " << rows * cols * 2;
	gotoxy(cols * 2 + 2, 11);
	cout << "출구: (" << exit_row << ", " << exit_col << ")" << endl;
}

void gotoxy(int x, int y) // (x, y)좌표로 이동하는 함수. WinAPI 사용
{
	COORD pos = { x, y };   // 좌표값을 저장하는 구조체 COORD를 이용하여 입력받은 (x,y)좌표로 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);   // 커서를 이동시키는 SetConsoleCursorPosition
}

void txtcolor(unsigned short color) // 텍스트 색 변경 함수. WinAPI 사용
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color);
}