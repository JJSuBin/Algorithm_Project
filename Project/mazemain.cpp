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

void Init(); // ���� ������ �ʱ�ȭ�ϴ� �Լ��̴�.
void readFile(char* filename); // ������ �о� �̷��� ũ�⸦ �ľ��Ѵ�. 1�� ��, 0�� ��� ǥ���ϰ� �ⱸ�� ����Ѵ�.
int findPathA(int row, int col); // A* �˰����� ������ �Լ��̴�.
void printMap(); // ���̶�� �ฦ ���̸� " "�� ����Ͽ� ���� �̷�ó�� ǥ���Ѵ�.
void printMouse(int x, int y); // �Է¹��� ��ġ�� �㸦 ����Ѵ�.
void printPath(int x, int y, int TYPE); // �Է¹��� ��ġ�� �Ḧ ����ϸ� TYPE�� ���� �ٸ� ���� ����Ѵ�.
void MapInformation(); // ���� ������ ���� �ʱ� �������� ����Ѵ�.
void gotoxy(int x, int y); //Ŀ���� ��ġ�� �����̴� �Լ��̴�.
void txtcolor(unsigned short color); //winAPI�� ����Ͽ� ���� �����ϴ� �Լ��̴�.
void writeFile(char* filename2);

//�̷ο� ���õ� ����
#define PATH 0 // ��
#define WALL 1 // ��
#define GMOVE 4 // �㰡 ������ ��
#define GBACK 5 // �㰡 �ǵ��ư� ��
#define TELL 6 // ������
#define EXIT 60 // �ⱸ
#define DELAY 10 // �˰����� �� �κ��� �ùķ��̼� ������

int **map = NULL; // �� �����Ҵ�
int rows, cols;
int exit_row = 0;
int exit_col = 0;
int tel; // �����¿� �� PATH�� ��. 2 �̻��̸� ������� �����Ѵ�.
int cnt; // �����¿� �� PATH�� �ƴ� ���� ��
int c = 0;

typedef struct Mouse // �� ����ü ����
{
	int energy, x = 0, y = 1;
	double mana;
} Mouse;

Mouse mouse;

typedef struct Point
{      //����Ʈ ����ü
	int row, col;
	int F, G, H;
} Point;

stackClass FinalPath; //�ִ� ��� �� �˰����� ���� �� ���
stackClass telStack;

int main(void)
{
	char filename[100];
	cout << "�̷� ���� �̸��� �Է��ϼ���: ";
	cin >> filename;
	readFile(filename);
	Init();
	printMap();
	MapInformation();
	_getch();
	if (findPathA(mouse.x, mouse.y) == EXIT) // �ⱸ�� ã����
	{
		if (FinalPath.Size() > 0) // �ִܰ�� ������ �� �� ����
		{
			while (true)
			{
				txtcolor(6);
				gotoxy(FinalPath.Top->col * 2, FinalPath.Top->row); // ���ÿ� ����� ��ġ�� Ŀ���� �ű��
				cout << "��"; // ����� �׸� ����Ѵ�.
				FinalPath.Pop();
				if (FinalPath.IsEmpty()) break; // �ִܰ�� ������ ��� �ݺ����� �����Ѵ�.
			}
		}
	}
	else // �ⱸ�� ã�� ���ϸ�
	{
		gotoxy(cols * 2 + 2, 5);
		cout << "�ⱸ�� ã�� ���߽��ϴ�." << endl;
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
	mouse.mana = 0; // �ʱ� ���� = 0
	mouse.energy = cols * rows * 2; // �ʱ� ������ = �� * �� * 2
}

void readFile(char* filename)
{
	ifstream file(filename);
	rows = 0;
	cols = 0;

	while (!file.eof())
	{
		char c = (char)file.get();
		if (c == '1' || c == '0' || c == '\n') // �� ���� ���ڴ� �̷��� ������ ��Ÿ���� ������ �ƴϴ�.
		{
			if (c == '\n') // �� ���� ����
				rows++;
			if (rows == 0) // �� ���� ����
				cols++;
		}
	}
	++rows;

	file.clear();
	file.seekg(0); // ���������� �� ������ �̵�

	int row = 0;
	int col = 0;
	map = new int*[rows]; // �����Ҵ�
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

			if (c == '1') map[row][col] = WALL; // ��
			else if (c == '0') map[row][col] = PATH; // ��
			col++;
		}
	}

	//�ⱸ�� ã�� �ڵ��̴�.
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			if ((map[0][j] == 0) && j != 1) { //�� ���ٿ� 0(PATH)�� �ְ�, �� ���� �Ա�(0,1)�� �ƴ϶��
				exit_row = 0; exit_col = j;
			}
			else if (map[i][0] == 0) { // ���� ���� PATH�� �ִٸ�
				exit_row = i; exit_col = 0;
			}
			else if (map[rows - 1][j] == 0) { // ���� �����ʿ� PATH�� �ִٸ�
				exit_row = rows - 1; exit_col = 0;
			}
			else if (map[i][cols - 1] == 0) { //���� �Ʒ� �ٿ� PATH�� �ִٸ�
				exit_row = i; exit_col = cols - 1;
			}

		}

	map[exit_row][exit_col] = EXIT; // �� ��ġ�� EXIT�� �����Ѵ�.
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
	List<Point> sels; // ������ġ���� �� �� �ִ� ���� �����ϴ� ����Ʈ
	tel = 0;
	cnt = 0;

	if (map[row][col] == EXIT)
		return EXIT;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if ((abs(i + j) % 2 == 1) && row + i >= 0 && row + i < rows && col + j >= 0 && col + j < cols && !(i == 0 && j == 0)) //�밢�� ����
			{
				if ((map[row + i][col + j] == PATH) || (map[row + i][col + j] == EXIT))  // PATH�� EXIT�̶��
				{
					tel++; // PATH���� ������Ų��.
					Point *point = new Point; // ����ġ ���� �� ����� ���� ������
					point->row = row + i;
					point->col = col + j;
					point->G = FinalPath.Size() + 1;
					point->H = abs(exit_row - point->row) + abs(exit_col - point->col);
					point->F = point->G + point->H;
					sels.push(*point); // ����Ʈ�� �����Ѵ�.

					if (tel >= 2) // �������̶��
					{
						telStack.Push(row, col); // �����潺�ÿ� ���� ��ġ�� �����Ѵ�.
					}
				}
				else // PATH�� EXIT�� �ƴ϶��
					cnt++;
			}

		}
	}


	for (int i = 0; i < sels.size() - 1; i++) // F�� �������� �񳻸��������� �����Ѵ�.(��������)
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
	} //�̵��� �� �ִ� ��ġ�� �켱���� �ο��Ѵ�.

	int a[2000];
	int b[2000];

	if (cnt == 4) // �����¿� ��� PATH�� EXIT�� �ƴ϶��
	{
		c = 0; // count 0���� �ʱ�ȭ (���ƿ;��ϴ� ���� ��)
		mouse.x = telStack.Top->row; // ���� ��ġ ����
		mouse.y = telStack.Top->col;

		while (mouse.x != FinalPath.Top->row || mouse.y != FinalPath.Top->col) // ���� �ֱٿ� ���� ��������� �ǵ��ư����Ѵ�.
		{
			// �ǵ��ư����ϴ� ��θ� �����ϴ� �迭
			a[c] = FinalPath.Top->row;
			b[c] = FinalPath.Top->col;
			c++; // ���ư����ϴ� ���� �� ���� 
			FinalPath.Pop();
		}

		if (c > 4 && mouse.mana >= 5) // �ڷ���Ʈ �� ������ �����ϸ�
		{
			int i = 0;
			for (i; i < c; i++)
			{
				map[a[i]][b[i]] = TELL; //�̵��ߴ����� TELL��ǥ��
				printPath(b[i], a[i], TELL); //DELAY ���� �ѹ��� ǥ���Ѵ�.
			}
			printMouse(mouse.y, mouse.x); // �̵��� ��ġ�� �㸦 ���
			map[mouse.x][mouse.y] = TELL;
			Sleep(DELAY);
			printPath(mouse.y, mouse.x, TELL);
			mouse.mana -= 5.0;
			telStack.Pop();
		}
		else // �ڷ���Ʈ �� ������ ���� ������
		{
			int i = 0;
			for (i; i < c; i++)
			{ // �̵��ϴ� ���� �ϳ��� ����Ѵ�.
				printMouse(b[i], a[i]);
				map[a[i]][b[i]] = GBACK;
				Sleep(DELAY);
				printPath(b[i], a[i], GBACK);
				mouse.mana += 0.1; // ��ĭ���� ���� ������ �����Ѵ�.
				mouse.energy -= 1;
			}
			telStack.Pop(); // ������ ������ �����Ѵ�.
		}
	}


	if (sels.size() > 0) //  �� �� �ִ� ���� ������
	{
		map[mouse.x][mouse.y] = GMOVE; // ���� ��ġ ������ �����ϰ�
		mouse.x = sels.get(0).row; // ����ġ�� ���� ���� ���� ��ġ�� �����Ѵ�.
		mouse.y = sels.get(0).col;
		sels.erase(0); // ����

		FinalPath.Push(mouse.x, mouse.y); // ����� ���� ��ġ�� �ִܰ�� ���ÿ� �����ϰ�
		printMouse(mouse.y, mouse.x); // �㸦 ����Ѵ�.
		Sleep(DELAY);
		printPath(mouse.y, mouse.x, GMOVE);
		mouse.mana += 0.1; // ���� ������ �����Ѵ�.
		mouse.energy -= 1;
	}
	return findPathA(mouse.x, mouse.y);
}


void printMap()
{
	system("cls");
	txtcolor(8); // ȸ��
	for (int i = 0; i < rows; i++) // ��
	{
		for (int j = 0; j < cols; j++) // ��
		{
			if (map[i][j] == WALL)   // WALL�̸� (1)
			{
				cout << "��";
			}
			else // PATH�� (0)
			{
				cout << "  ";
			}
		}
		cout << endl;// �� �ٲ�
	}

	gotoxy(2, 0);
	txtcolor(14);
	cout << "��";
}

void printMouse(int x, int y)
{
	txtcolor(15);
	gotoxy(cols * 2 + 2, 13);
	cout << "������: " << mouse.energy << "������";
	gotoxy(cols * 2 + 2, 15);
	cout << "����: " << mouse.mana << "       ";

	txtcolor(14);
	gotoxy(x * 2, y);
	cout << "��";// �� ���

	if (mouse.energy <= 0) // �������� ��� �Ҹ��ϸ�
	{
		gotoxy(cols * 2 + 2, 35);
		txtcolor(12);
		cout << "�ⱸ ã�� ����! �������� �����ϴ�." << endl;
		_getch();
		exit(0);
	}

}

void printPath(int x, int y, int TYPE) // �̵��� ��� ��θ� ����Ѵ�.
{
	gotoxy(x * 2, y);
	cout << " ";

	if (TYPE == GMOVE) // ������ �� ��� (�Ͼ��)
	{
		txtcolor(15);
		gotoxy(x * 2, y);
		cout << "��";
	}
	if (TYPE == GBACK) // �ǵ��ư� �� ��� (���)
	{
		txtcolor(2);
		gotoxy(x * 2, y);
		cout << "��";
	}
	if (TYPE == TELL) { // �ڷ���Ʈ�� �� (���� �������� ����, ���޶��� ��)
		txtcolor(3);
		gotoxy(x * 2, y);
		cout << "��";
	}
}

void MapInformation()         //��ü���� ��Ȳ ���
{
	gotoxy(cols * 2 + 2, 5);
	txtcolor(15);
	cout << "<�˰���>" << endl;
	gotoxy(cols * 2 + 2, 7);
	cout << "�̷�ũ�� : " << rows << " x " << cols;
	gotoxy(cols * 2 + 2, 9);
	cout << "�� ������: " << rows * cols * 2;
	gotoxy(cols * 2 + 2, 11);
	cout << "�ⱸ: (" << exit_row << ", " << exit_col << ")" << endl;
}

void gotoxy(int x, int y) // (x, y)��ǥ�� �̵��ϴ� �Լ�. WinAPI ���
{
	COORD pos = { x, y };   // ��ǥ���� �����ϴ� ����ü COORD�� �̿��Ͽ� �Է¹��� (x,y)��ǥ�� �̵�
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);   // Ŀ���� �̵���Ű�� SetConsoleCursorPosition
}

void txtcolor(unsigned short color) // �ؽ�Ʈ �� ���� �Լ�. WinAPI ���
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color);
}