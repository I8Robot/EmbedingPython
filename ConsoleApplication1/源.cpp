#include <python.h> //包含头文件，在c++中嵌入python，这是必须的
#include <iostream>
#include <tchar.h>
#include "Windows.h"
#include <string>
#include "atlstr.h"

// 使用王弼szUTF8需要delete[]
int GBK2UTF8(char *szGBK, char *szUTF8)
{
	// GBK转换为unicode
	int n = MultiByteToWideChar(CP_ACP, 0, szGBK, -1, NULL, 0);
	WCHAR* wszTmp1 = new WCHAR[n*sizeof(WCHAR)];
	MultiByteToWideChar(CP_ACP, 0, szGBK, -1, wszTmp1, n);

	// unicode转换为UTF-8
	int m = WideCharToMultiByte(CP_UTF8, 0, wszTmp1, -1, NULL, 0, NULL, NULL);
	szUTF8 = new char[m*sizeof(char)];
	WideCharToMultiByte(CP_UTF8, 0, wszTmp1, -1, szUTF8, m, NULL, NULL);

	delete[] wszTmp1;
	wszTmp1 = nullptr;
	return 0;
}

int GBK2UTF8(std::string &strGBK, std::string &strUTF8)
{
	// GBK转换为unicode
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	WCHAR* wszTmp1 = new WCHAR[n*sizeof(WCHAR)];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, wszTmp1, n);

	// unicode转换为UTF-8
	int m = WideCharToMultiByte(CP_UTF8, 0, wszTmp1, -1, NULL, 0, NULL, NULL);
	char *szUTF8 = new char[m*sizeof(char)];
	WideCharToMultiByte(CP_UTF8, 0, wszTmp1, -1, szUTF8, m, NULL, NULL);
	strUTF8 = szUTF8;	// strUTF8 = std::string(szUTF8);

	delete[] wszTmp1;
	delete szUTF8;
	wszTmp1 = nullptr;
	return 0;
}

int UTF82GBK(std::string &strUTF8, std::string &strGBK)
{
	// UTF-8转换为unicode
	int n = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	WCHAR* wszTmp1 = new WCHAR[n*sizeof(WCHAR)];
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszTmp1, n);

	// unicode转换为gbk
	int m = WideCharToMultiByte(CP_ACP, 0, wszTmp1, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[m*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, wszTmp1, -1, szGBK, m, NULL, NULL);
	strGBK = szGBK;	// strUTF8 = std::string(szUTF8);

	delete[] wszTmp1;
	delete szGBK;
	wszTmp1 = nullptr;
	return 0;
}

int UTF82Unicode(std::string &strUTF8, std::wstring &strUnicode)
{
	// UTF-8转换为unicode
	int n = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	WCHAR* wszTmp1 = new WCHAR[n*sizeof(WCHAR)];
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszTmp1, n);
	strUnicode = wszTmp1;
	delete[] wszTmp1;
	wszTmp1 = nullptr;
	return 0;
}

int main()
{
	char* chrTmp = "中文abc";		// GBK: 0xd6d0  0xcec4    ASNI:0x61 0x62 0x63
									//         中     文             a    b    c
	char* chrGBK = NULL;
	GBK2UTF8(chrTmp, chrGBK);
	TCHAR *wchrTmp = _T("中文abc");	// Unicode: 0x4e2d 0x6587 0x0061 0x0062 0x0063
									//             中    文     a       b      c

	CString cstr_wchrTmp = wchrTmp;
	printf("printf--char: %s\n", chrTmp);
 	printf("printf--TCHAR(Unicode): %s\n", wchrTmp);
	std::cout << "cout--char: %s\n" << chrTmp << std::endl;
 	std::cout << "cout--TCHAR(Unicode)" << cstr_wchrTmp << std::endl;
	MessageBox(NULL, wchrTmp, _T("wchrTmp"), MB_OK);
	Py_Initialize();
	// 检查初始化是否成功  
	if (!Py_IsInitialized()) {
		return -1;
	}
	// 将Python工作路径切换到待调用模块所在目录，一定要保证路径名的正确性
// 	std::string path = "F:/project/C++/embedPython/x64/Release";
// 	std::string chdir_cmd = std::string("sys.path.append(\"") + path + "\")";
// 	const char* cstr_cmd = chdir_cmd.c_str();
// 	PyRun_SimpleString("import sys");
// 	PyRun_SimpleString(cstr_cmd);

// 	PyRun_SimpleString("print(sys.path)");
 	PyObject * pModule = NULL;
 	PyObject * pFunc = NULL;
	PyObject *pArg=NULL, *pValue=NULL;

 	pModule = PyImport_ImportModule("trans");
 	
	if (pModule != NULL)
	{
		// 加载函数
		pFunc = PyObject_GetAttrString(pModule, "translate_Youdao");
		if (pFunc && PyCallable_Check(pFunc))	// 验证是否加载成功
		{
			PyObject *list;
			//list = Py_BuildValue("[s]", "绿豆");
			list = Py_BuildValue("[s]", "three");
			 			std::string strgbk = "三";
			 			std::string strUtf8;
			 			GBK2UTF8(strgbk, strUtf8);
			pArg = Py_BuildValue("(s)", strUtf8.c_str());
			//pArg = Py_BuildValue("(s)", "three");
			PyObject *tuple;

		//	pArg = Py_BuildValue("(iis)", 1, 2, "three");

		//	int r = PyTuple_SetItem(pArg, 0, Py_BuildValue("(s)", "绿豆"));
			
			if (pArg == NULL)
			{
				std::cout << "ERROR." << std::endl;
			}
			std::cout << "[INFO] Get function (translate_Youdao) succeed." << std::endl;
			PyObject* pRet = PyObject_CallObject(pFunc, pArg);
			const char * pC = NULL;
			if (!PyObject_Str(pRet))
				std::cout << "[ERROR] PyArg_ParseTuple ERROR" << std::endl;
			pC = PyUnicode_AsUTF8(pRet);
			TCHAR* wStr = (TCHAR*)pC;
			std::string str(pC);
			std::string strRetGBK;
			std::wstring strUnicodeRet;
			UTF82GBK(str, strRetGBK);
			UTF82Unicode(str, strUnicodeRet);
			std::cout << pC << std::endl;
		//	std::cout << strRetGBK.compare(strTmp) << std::endl;
			CString cstrUTF8(pC);
			CString cstrGBK(strRetGBK.c_str());
			CString cstrUnicode(strUnicodeRet.c_str());
			MessageBox(NULL, cstrUTF8, _T("cstrUTF8"), MB_OK);
			MessageBox(NULL, cstrGBK, _T("cstrGBK"), MB_OK);
			MessageBox(NULL, cstrUnicode, _T("cstrUnicode"), MB_OK);
			printf("GBK: %s\n", strRetGBK);
			std::cout << strRetGBK.c_str() << std::endl;
			//std::cout << strUnicodeRet.compare(wchrTmp) << std::endl;
			printf("Unicode: %s\n", strUnicodeRet);
			printf("Unicode: %s\n", strUnicodeRet.c_str());
			//MessageBoxW(NULL,strUnicodeRet.c_str());
			
			//int nnn = str.compare(str1);
// 			int iLen = WideCharToMultiByte(CP_UTF8, 0, wStr, -1, NULL, 0, NULL, NULL);
// 			char* pchr = new char(iLen*sizeof(char));
// 			WideCharToMultiByte(CP_UTF8, 0, wStr, -1, pchr, iLen, NULL, NULL);
			//PyObject_Print(PyObject_Str(pRet), stdout, 0);
			//Py_DECREF(pRet);
			//printf("%s", wStr);
			Py_DECREF(pArg);
		}
		else
		{
			std::cout << "[ERROR] Can't find function (translate_Youdao)" << std::endl;
		}		
	}
	

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);

	Py_Finalize();

	return 0;
}
