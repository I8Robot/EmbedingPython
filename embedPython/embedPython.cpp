// embedPython.cpp : 定义控制台应用程序的入口点。
// 
// 
#include "stdafx.h"
#include <Python.h>
#include <iostream>
#include <stdio.h>
#include "windows.h"
#include "Psapi.h"
#include "VersionHelpers.h"
#include "Tlhelp32.h"

#define PyTuple_MAXSAVESIZE 100000

double showMemoryInfo()
{
	double dMemorySize_WorkingSetSize = 1;
	DWORD processID = GetCurrentProcessId();
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 4420);
	
	if (hProc == INVALID_HANDLE_VALUE)
		return 0;

	PSAPI_WORKING_SET_INFORMATION probe;
	BOOL bRet = QueryWorkingSet(hProc, &probe, sizeof(probe));

	PSAPI_WORKING_SET_INFORMATION *info = NULL;
	ULONG_PTR* data = new ULONG_PTR[probe.NumberOfEntries + 1];
	info = (PSAPI_WORKING_SET_INFORMATION*)data;
	bRet = QueryWorkingSet(hProc, info, (probe.NumberOfEntries + 1) * sizeof(ULONG_PTR));


	ULONG_PTR privatePages = 0;
	for (ULONG_PTR i = 0; i < probe.NumberOfEntries; i++)
	{
		if (!info->WorkingSetInfo[i].Shared)
			privatePages++;
	}

	printf("%d %dK\n", processID, privatePages * 4);
	dMemorySize_WorkingSetSize = privatePages * 4;
	return dMemorySize_WorkingSetSize;
}

// 验证C语言 malloc和free，C语言free之后不会改变指针的指向，也就是指针变量的值，
// 但是C++的delete不只将内存释放，还会改变指针的指向。
void testMalloc()
{
	int* pn1 = (int *)malloc(sizeof(int));
	if (pn1 == NULL)
	{
		printf("ERROR malloc failed!");
		return;
	}
	int* pn2 = pn1;
	*pn1 = 10;
	free(pn1);
	pn1 = NULL;
	// 测试一下刚刚释放的内存，是不是会被使用
	int* pn3 = (int *)malloc(sizeof(int));
	*pn3 = 15;
}

void without_def()
{
	double dMem = showMemoryInfo();
	printf("创建Tuple之前内存: %8lf\n", dMem);

	PyObject* pTu = PyTuple_New(PyTuple_MAXSAVESIZE);
	system("pause");
	dMem = showMemoryInfo();
	printf("刚创建Tuple0之后内存: %8lf\n", dMem);
	system("pause");
	for (int i = 0; i < PyTuple_MAXSAVESIZE; i++)
	{
		PyTuple_SetItem(pTu, i, PyLong_FromLong(i + 300));
	}
	system("pause");
	dMem = showMemoryInfo();
	printf("创建完Tuple0并赋值之后内存: %8lf\n", dMem);
	system("pause");
	//-----------------------------------------------//
	PyObject* pTu1 = PyTuple_New(PyTuple_MAXSAVESIZE);
	Sleep(100);
	dMem = showMemoryInfo();
	printf("刚创建Tuple1之后内存: %8lf\n", dMem);

	for (int i = 0; i < PyTuple_MAXSAVESIZE; i++)
	{
		PyTuple_SetItem(pTu1, i, PyLong_FromLong(i + 300));
	}
	Sleep(100);
	dMem = showMemoryInfo();
	printf("创建完Tuple1并赋值之后内存: %8lf\n", dMem);
	//--------------------------------------------//
	PyObject* pTu2 = PyTuple_New(PyTuple_MAXSAVESIZE);
	Sleep(100);
	system("pause");
	dMem = showMemoryInfo();
	printf("刚创建Tuple2之后内存: %8lf\n", dMem);
	system("pause");
	for (int i = 0; i < PyTuple_MAXSAVESIZE; i++)
	{
		PyTuple_SetItem(pTu2, i, PyLong_FromLong(i + 300));
	}
	Sleep(100);
	dMem = showMemoryInfo();
	printf("创建完Tuple2并赋值之后内存: %8lf\n", dMem);

	Py_CLEAR(pTu);
	Py_CLEAR(pTu1);
	Py_CLEAR(pTu2);
}

int main()
{
	Py_Initialize();
	for (int i = 0; i < 3; i++)
	{
		double dMem = -19;
		dMem = showMemoryInfo();
		printf("程序初始内存: %8f\n", dMem);
		
		without_def();
		
		printf("mmmmm\n");
	}
	Py_Finalize();
	double dMem = -19;
// 	dMem = showMemoryInfo();
// 	printf("程序最后内存: %8f\n", dMem);
	Sleep(1000);
	for (int i = 0; i < 10; i++)
	{
		dMem = showMemoryInfo();
		printf("程序最后内存: %8f\n", dMem);
		Sleep(1000);
	}
	
	char c;
	scanf_s("%c", &c);
	return 0;
}


