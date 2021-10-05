#pragma once
#include <iostream>
#include <windows.h>

typedef struct node
{
	int row;
	int col;
	node *Next;
}node;

typedef node* node_pointer;

class stackClass
{
public:
	int pointCol;
	int pointRow;
	int size;

	stackClass();
	stackClass(const stackClass & s);
	~stackClass();

	void Push(int m_row, int m_col);
	void Pop();
	boolean IsEmpty();
	int Size();

	node_pointer Top;

};