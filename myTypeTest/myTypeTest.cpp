#include "Python.h"
#include "frameobject.h"
#include "iostream"
#include "tchar.h"
#include "windows.h"
#include "Psapi.h"
#include "ctime"
#include "vector"

double showMemoryInfo()
{
	DWORD pid;
	pid = GetCurrentProcessId();

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (hProc == INVALID_HANDLE_VALUE)
	{
		printf("hProc inalid.");
		return 0;
	}
	PROCESS_MEMORY_COUNTERS pmc;
	pmc.cb = sizeof(pmc);
	GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc));
	printf("     提交大小：%f\n", pmc.PagefileUsage / 1024.0);
	return pmc.PagefileUsage / 1024.0;
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
	delete[] szUTF8;
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

// 测试PyLong_FromLong返回New Referecne，如果不decrease reference,内存会不会释放。
void testFromLong()
{
	long cL = 1;
	PyObject* pyL = PyLong_FromLong(cL);
	printf("---------------------- Call Py_DECREF --------------------\n");
	printf("调用10000次PyLong_FromLong之前内存：\n");
	showMemoryInfo();
	clock_t start = clock();
	for (int i = 0; i < 10000; i++)
	{
		pyL = PyLong_FromLong(i);
		Py_DECREF(pyL);		// 调用Py_DECREF释放内存，如果不DECREF，占用的内存不会释放
	}
	clock_t finish = clock();
	double t = (finish - start);
	printf("用时: %f\n", t);
	printf("调用10000次PyLong_FromLong，并且每次都Py_DECREF之后内存：\n");
	showMemoryInfo();
	printf("---------------------- No Py_DECREF --------------------\n");
	printf("调用10000次PyLong_FromLong之前内存：\n");
	showMemoryInfo();
	start = clock();
	for (int i = 0; i < 10000; i++)
	{
		pyL = PyLong_FromLong(i);
		//Py_DECREF(pyL);		// 如果不DECREF，占用的内存不会释放
	}
	finish = clock();
	t = (finish - start);
	printf("用时: %f\n", t);
	printf("调用10000次PyLong_FromLong,并且每次不Py_DECREF之后内存：\n");
	showMemoryInfo();
}
// 测试C语言里面循环10000次，赋值long，会不会导致内存泄漏
void testCLong()
{
	long ll = 0;
	system("pause");
	for (int i = 0; i < 10000; i++)
	{
		ll = i;
	}
}

// 测试C类型传入python,再有python返回，转换为C类型
int testType()
{
	PyObject *pModule = NULL;
	PyObject *pArg = NULL, *pValue = NULL;	// 传入参数，返回值
	const char* pszModuleName = "data_type_trans";	// python模块名
	pModule = PyImport_ImportModule(pszModuleName);	// 导入python模块，This function always uses absolute imports.
	if (pModule == nullptr)
	{
		std::cout << "[ERROR] Import Module " << pszModuleName << std::endl;
		return -1;
	}
	////////////////// int类型 ///////////////////////
	int nCNum = 50;
	pArg = Py_BuildValue("(i)", nCNum);
	PyObject* pFuncPyInt = NULL;
	pFuncPyInt = PyObject_GetAttrString(pModule, "py_int");
	if (!PyCallable_Check(pFuncPyInt))
	{
		std::cout << "[ERROR] Find py_int method " << std::endl;
		return -1;
	}
	pValue = PyObject_CallObject(pFuncPyInt, pArg);
	int nn = PyLong_AsLong(pValue);



	//////////////// double类型 //////////////////////
	double dCNum = 2.13;
	pArg = Py_BuildValue("(d)", dCNum);
	PyObject* pFuncPyDouble = NULL;
	pFuncPyDouble = PyObject_GetAttrString(pModule, "py_double");
	if (!PyCallable_Check(pFuncPyDouble))
	{
		std::cout << "[ERROR] Find py_int method " << std::endl;
		return -1;
	}
	pValue = PyObject_CallObject(pFuncPyDouble, pArg);
	double dn = PyFloat_AsDouble(pValue);


	/////////////// string类型 //////////////////////
//	std::string strGbk = "C语言\0python";		// \0截断
	std::string strGbk =
"def loop():\r\n   \
     x = 1\r\n   \
         while x < 5:\r\n    \
             if x == 3 :\r\n   \
                 break\r\n   \
             x = x + 1\r\n    \
             print x\r\n   \
     return x\r\n    \
 loop()";
	char* cGBk = "C语言python";
	std::string strUTF8 = "";
	GBK2UTF8(strGbk, strUTF8);
	pArg = Py_BuildValue("(s)", strUTF8.c_str());	// "s"：Convert a null-terminated C string to a Python str object using 'utf-8' encoding. 
	PyObject* pFuncPyString = NULL;
	pFuncPyString = PyObject_GetAttrString(pModule, "py_string");
	if (!PyCallable_Check(pFuncPyString))
	{
		std::cout << "[ERROR] Find py_string method " << std::endl;
		return -1;
	}
	pValue = PyObject_CallObject(pFuncPyString, pArg);
	char* pszStr = NULL;
	pszStr = PyUnicode_AsUTF8(pValue);	// "s": Unicode objects are converted to C strings using 'utf-8' encoding.

	std::string strRetUTF8(pszStr);
	std::string strRetGBK;
	UTF82GBK(strRetUTF8, strRetGBK);


	///////////// 输出结果 ////////////////
	std::cout << "\n-------------------------------------------------------" << std::endl;
	std::cout << "       C类型\t" << "\tPython类型" << std::endl;
	std::cout << "int:    " << nCNum << "\t\t" << nn << std::endl;
	std::cout << "double: " << dCNum << "\t\t" << dn << std::endl;
	std::cout << "string:\n " << strGbk.c_str() << "\t" << pszStr << "(\n" << strRetGBK.c_str() << ")" << std::endl;
	std::cout << "-------------------------------------------------------" << std::endl;

	///////////////////////////////////////////////
	char* cStr = "我机器人IRobot";
	printf("C类型: %s\n", cStr);
	int nnnn = 123;
	PyObject* pyStr = PyBytes_FromFormat("%s%d", cStr, nnnn);
	PyTuple_SetItem(pArg, 0, pyStr);
	pValue = PyObject_CallObject(pFuncPyString, pArg);
	pszStr = PyBytes_AsString(pValue);
	std::cout << "Python类型:" << pszStr << std::endl;

	// decrease reference
	Py_CLEAR(pFuncPyInt);
	Py_CLEAR(pFuncPyDouble);
	Py_CLEAR(pFuncPyString);
	Py_CLEAR(pyStr);
	Py_CLEAR(pArg);
	Py_DECREF(pValue);
	pValue = NULL;
	Py_CLEAR(pModule);

	////////////////// Tuple /////////////////////
	PyObject* pTu;
	int cn = 123;
	char *cc = "Hello";
	std::cout << "\nTuple:" << std::endl;
	std::cout << "    C数据:" << "cc=" << cc << " " << "cn=" << cn << std::endl;
	std::cout << "    通过Py_BuildValue(\"(si)\", cc, cn)生成Tuple：pTu." << std::endl;
	pTu = Py_BuildValue("(si)", cc, cn);
	int pynn = 0;
	char *pycc = NULL;
	PyArg_ParseTuple(pTu, "si", &pycc, &pynn);
	std::cout << "    使用PyArg_ParseTuple(pTu, \"si\", &pycc, &pynn)解析pTu:" << std::endl;
	std::cout << "    " << pycc << " " << pynn << std::endl;

	Py_CLEAR(pTu);
}
int CompileByPython()
{
		std::string strCode =
"def loop():\r\n\
    x = 0\r\n\
    while x < 5:\r\n\
        if x == 3 :\r\n\
            break\r\n\
        x = x + 1\r\n\
        print(x)\r\n\
    return x\r\n\
loop()";
	std::cout << "代码：\n" << strCode.c_str() << std::endl;;
	PyObject *pModule = NULL;
	PyObject *pArg = NULL, *pValue = NULL;	// 传入参数，返回值
	const char* pszModuleName = "pythonCodeCompile";	// python模块名
	pModule = PyImport_ImportModule(pszModuleName);	// 导入python模块，This function always uses absolute imports.
	if (pModule == nullptr)
	{
		std::cout << "[ERROR] Import Module " << pszModuleName << std::endl;
		return -1;
	}

	PyObject* pFuncPyCompile = NULL;
	pFuncPyCompile = PyObject_GetAttrString(pModule, "pythonCodeCompile");
	if (!PyCallable_Check(pFuncPyCompile))
	{
		std::cout << "[ERROR] Find py_string method " << std::endl;
		return -1;
	}
	std::string strUTF8 = "";
	GBK2UTF8(strCode, strUTF8);
	pArg = Py_BuildValue("(s)", strUTF8.c_str());	// "s"：Convert a null-terminated C string to a Python str object using 'utf-8' encoding. 
	pValue = PyObject_CallObject(pFuncPyCompile, pArg);
	
	char* pszStr = NULL;
	pszStr = PyUnicode_AsUTF8(pValue);	// "s": Unicode objects are converted to C strings using 'utf-8' encoding.
	std::string strRetUTF8(pszStr);
	std::string strRetGBK;
	UTF82GBK(strRetUTF8, strRetGBK);	// 不经过utf-8到GBK转换的话，汉字显示有问题

	std::cout << pszStr << std::endl;
	std::cout << strRetGBK.c_str() << std::endl;
	Py_CLEAR(pFuncPyCompile);
	Py_CLEAR(pArg);
	Py_CLEAR(pValue);
	pValue = NULL;
	Py_CLEAR(pModule);
}
void getError(std::vector<std::string> vecStrCodeLine)
{
	PyObject *type = NULL;
	PyObject *value = NULL;
	PyObject *traceback = NULL;
	PyErr_Fetch(&type, &value, &traceback);
	if (type)
	{
		std::cout << PyExceptionClass_Name(type) << ":" << std::endl;
	}

	if (value)
	{

		PyObject* line = PyObject_Str(value);

		if (line && (PyUnicode_Check(line)))
		{
			std::string strExcValueUTF8 =  PyUnicode_AsUTF8(line);
			std::string strExcValueGBK;
			UTF82GBK(strExcValueUTF8, strExcValueGBK);
			std::cout << strExcValueGBK.c_str() << std::endl;
		}
	}

	if (traceback)
	{
		PyTracebackObject *tb = (PyTracebackObject *)traceback;
		for (; NULL != tb; tb = tb->tb_next)
		{
			PyObject* line = PyUnicode_FromFormat("File \%U\", line %d, in %U\n",
				tb->tb_frame->f_code->co_filename, tb->tb_lineno,
				tb->tb_frame->f_code->co_name);
			std::cout << PyUnicode_AsUTF8(line);
			if (tb->tb_lineno < vecStrCodeLine.size())
			{
				std::cout << vecStrCodeLine.at(tb->tb_lineno-1).c_str() << std::endl;
			}
		}
	}
}

// 可以考虑用find()和substr()实现
std::vector<std::string> strSplit(std::string str, const char* delimiter)
{
	if (str.length()==0)
	{
		return std::vector<std::string>();
	}
	std::string strTmp;
	std::vector<std::string> vecSplitstr;
	char *pszStr;
	int len = str.length();
	pszStr = (char*)malloc((len + 1)*sizeof(char));
	str._Copy_s(pszStr, len, 0);

	char* pszTmp = strtok(pszStr, delimiter);
	while (pszTmp)
	{
		strTmp = pszTmp;
		vecSplitstr.push_back(strTmp);
		pszTmp = strtok(NULL, delimiter);
		
	}
	return vecSplitstr;
}
int compileByCAPI()
{
	std::string	strCode =
"import sys\n\
def loop():\n\
    x = 0\n\
    while x < 10:\n\
        if x == 3:\n\
            break\n\
        x = myAdd(x,1)\n\
        print(x)\n\
    return x\n\
def myAdd(a, b):\n\
    #m = 1/0\n\
    return a+b\n\
print(\"asdfasfdasf\")\n\
if __name__ == \"__main__\":\n\
    f_handler=open('out.log', 'a')\n\
    preOut = sys.stdout\n\
    sys.stdout = f_handler\n\
    print(\"12341234123\")\n\
    loop()\n\
    f_handler.close()\n\
    sys.stdout = preOut";
	std::vector<std::string> strLine;
	strLine = strSplit(strCode, "\n");
	std::string strUTF8 = "";
	GBK2UTF8(strCode, strUTF8);
	std::cout << strCode.c_str() << std::endl;;
	PyObject *pByteCode = NULL;
	PyObject *pArg = NULL, *pValue = NULL;	// 传入参数，返回值
	PyObject *m, *d, *v;
	m = PyImport_AddModule("__main__");
	d = PyModule_GetDict(m);
	pValue = PyRun_String(strUTF8.c_str(), Py_file_input, d, d);
	
	if (pValue == NULL)
	{
		getError(strLine);
		std::cout << "error" << std::endl;
	}
	/************************************/
	//int nR = 0;
	//nR = PyRun_SimpleString(strUTF8.c_str());	// PyRun_SimpleString:If there was an error, there is no way to get the exception information.
	//PyEval_EvalCode(pByteCode);
// 	if (nR == -1)
// 	{
// 		getError(strLine);
// 		PyErr_Print();
// 		std::cout << "error" << std::endl;
// 	}
	/*********************************************/
	Py_CLEAR(pArg);
	pValue = NULL;
	return 1;

}

union MyUnion
{
	int n;
	float f;
	double d;
};

class PythonEngine
{
public:
	PythonEngine();
	~PythonEngine();

	void Excute();
private:
	PyObject * pModule, *pGlobals, *pLocals;
};

PythonEngine::PythonEngine()
{
	Py_Initialize();
	pModule = PyImport_AddModule("__main__");
	pGlobals = PyModule_GetDict(pModule);
	pLocals = PyModule_GetDict(pModule);
}

PythonEngine::~PythonEngine()
{
	Py_Finalize();
}

void PythonEngine::Excute()
{
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	std::string	strCode =
"import sys\n\
def loop():\n\
    x = 0\n\
    while x < 10:\n\
        if x == 3:\n\
            break\n\
        x = myAdd(x,1)\n\
        print(x)\n\
    return x\n\
def myAdd(a, b):\n\
    m = 1/0\n\
    return a+b\n\
print(\"asdfasfdasf\")\n\
if __name__ == \"__main__\":\n\
    f_handler=open('out.log', 'a')\n\
    preOut = sys.stdout\n\
    sys.stdout = f_handler\n\
    print(\"12341234123\")\n\
    loop()\n\
    f_handler.close()\n\
    sys.stdout = preOut";

	std::vector<std::string> strLine;
	strLine = strSplit(strCode, "\n");
	std::string strUTF8 = "";
	GBK2UTF8(strCode, strUTF8);
	PyObject *pValue = NULL;	// 传入参数，返回值
	PyRun_SimpleString("import sys");
// 	PyRun_SimpleString("f_out = open('outputlog.txt', 'a')");
// 	PyRun_SimpleString("preOut = sys.stdout");
// 	PyRun_SimpleString("sys.stdout = f_out");
// 	PyRun_SimpleString("print(sys.stdout)");
// 	PyRun_SimpleString("print(__name__)");
	pValue = PyRun_String(strUTF8.c_str(), Py_file_input, pGlobals, pLocals);
	getError(strLine);
// 	PyRun_SimpleString("print(\"__name__:\", __name__)");
// 	PyRun_SimpleString("print(dir())");
// 	PyRun_SimpleString("f_handler.close()");
	Py_CLEAR(pValue);

	PyGILState_Release(gstate);
}
PythonEngine pyEngine;
DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	pyEngine.Excute();
	return 0;
}

int main()
{	
	pyEngine.Excute();
	HANDLE h = CreateThread(NULL, 0, MyThreadFunction, NULL, 0, 0);
	
// 	Py_Initialize();	// 初始化python解释器
// 	if (!Py_IsInitialized())
// 	{
// 		std::cout << "[ERROR] Python Initialize Failed!" << std::endl;
// 		return -1;
// 	}
// //--------------------------------------------------------------//
// //	testType();		// 测试C类型传入python,再有python返回，转换为C类型
// 	compileByCAPI();
// 	printf("\n====================== REFERENE CONUT==============================\n");
// //	testFromLong();		// 测试Py_DECREF()作用
// //--------------------------------------------------------------//
// 
// 	Py_Finalize();		// 释放python解释器
	char cWait;
	printf("\n请按任意键继续...");
	scanf_s(("%c"), &cWait);
	printf("\n请按任意键继续...");
	scanf_s(("%c"), &cWait);
	return 0;
}