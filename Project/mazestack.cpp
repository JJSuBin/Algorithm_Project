#include "mazeStack.h"
#include<conio.h>


using namespace std;
stackClass::stackClass()
{
	Top = NULL;
	size = 0;
}

stackClass::stackClass(const stackClass & s) {}

stackClass::~stackClass() {}

void stackClass::Push(int m_row, int m_col)	// stack에 넣기
{
	node_pointer NewTop = new node;
	NewTop->row = m_row;
	NewTop->col = m_col;
	NewTop->Next = Top;

	Top = NewTop;
	size++;
}

void stackClass::Pop()	// stack에서 빼내기
{
	node_pointer Temp = Top;
	pointRow = Temp->row;
	pointCol = Temp->col;
	Top = Top->Next;
	delete Temp;
}

boolean stackClass::IsEmpty() // 비어있는지 확인
{
	if (Top == NULL)
		return true;
	else
		return false;
}

int stackClass::Size()
{
	return size;
}