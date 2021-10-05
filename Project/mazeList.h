#pragma once

#include<malloc.h>
#include<stdlib.h>

template<typename type>
class List
{
private:
	int valueNo;
public:
	type *list;

	List();
	~List();
	void push(type value);
	void push(type value, int index);
	void erase(int index);
	type get(int index);
	type* getc(int index);
	void replace(type value, int index);
	int size();
	void clear();
};

template<typename type>
List<type>::List()
{
	list = (type*)malloc(sizeof(type));
	valueNo = 0;
}

template<typename type>
List<type>::~List()
{
	free(list);
}

template<typename type>
void List<type>::push(type value)
{
	valueNo++;
	list = (type*)realloc(list, sizeof(type)*valueNo);
	list[valueNo - 1] = value;
}

template<typename type>
void List<type>::push(type value, int index)
{
	valueNo++;
	list = (type*)realloc(list, sizeof(type)*valueNo);
	int valueNo_ = valueNo - 1;
	for (int i = 0; i < valueNo - index; i++, valueNo_--)
		list[valueNo_] = list[valueNo_ - 1];
	list[index] = value;
}

template<typename type>
type List<type>::get(int index)
{
	if (index >= valueNo)
	{
		printf("Wrong Number");
		return list[index];
	}

	//	for(int i=0;i<index;i++)
	//		*list++;
	return list[index];
}

template<typename type>
type* List<type>::getc(int index)
{
	if (index >= valueNo)
	{
		printf("Wrong Number");
		return &list[index];
	}

	//	for(int i=0;i<index;i++)
	//		*list++;
	return &list[index];
}

template<typename type>
void List<type>::erase(int index)
{
	if (index >= valueNo)
	{
		printf("Wrong Number");
		return;
	}

	//	list[index] = NULL;
	int repeatV = valueNo - index - 1;
	for (int i = 0; i < repeatV; i++, index++)
		list[index] = list[index + 1];
	valueNo--;
	list = (type*)realloc(list, sizeof(type)*valueNo);
}

template<typename type>
void List<type>::replace(type value, int index)
{
	if (index > valueNo)
	{
		printf("Wrong Number");
		return;
	}
	list[index] = value;
}

template<typename type>
int List<type>::size()
{
	return valueNo;
}

template<typename type>
void List<type>::clear()
{
	for (int i = 0; i < valueNo; i++)
		free(list[i]);
	free(list);
	list = (type*)malloc(sizeof(type));
	valueNo = 0;
}