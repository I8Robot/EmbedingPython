#-*- coding:gbk -*-  
  
import sys  
import os  
import shutil  
  
#��ó���������ģ���·��  
def getModulesPath() :  
    lst = []  
    #sys.modules��һ���ֵ䣬���ݸ�ʽ���£�  
    #{'site': <module 'site' from 'D:\Python27\lib\site.pyc'>,  
    for v in sys.modules.values():  
        s = str(v)  
        if "from" in s:  
            data = s.split("'")  
            lst.append(data[-2])  
        else :  
            print("module : ", s)  
    return lst  
      
#��ȡ�ļ�  
def extractFiles(destDir, files) :  
    destDir.replace("/", "\\")  
    if destDir[-1] != '\\' :  
        destDir += '\\'  
          
    for f in files :  
        dest = filiterPath(destDir, f)  
        copyF(dest, f)  
          
#����·�� ȥ��������·��  
def filiterPath(destDir, srcFile) :  
    dest = destDir  
    maxLen = 0  
    for path in sys.path :  
        lenp = len(path)  
        if lenp < len(srcFile) and path == srcFile[:lenp] :  
            if maxLen < lenp :  
                dest = destDir + srcFile[lenp+1:]  
                maxLen = lenp  
              
    dest.replace("/", "\\")  
    if '.' in dest : #ȥ���ļ���  
        p = dest.rfind('\\')  
        if p >= 0 :  
            dest = dest[:p]  
    return dest  
  
#�����ļ�  
#���Ŀ��·�������ڣ��򴴽�  
def copyF(destDir, srcFile) :  
    if not os.path.isfile(srcFile) :  
        print("error : file %s not exist!" % srcFile)  
        return False  
    if not os.path.isdir(destDir) :  
        os.mkdir(destDir)  
        print("make dir:", destDir)  
    try :  
        shutil.copy2(srcFile, destDir)  
        print("copy file : %s to %s" % (srcFile, destDir))  
    except IOError:  
        print("error : copy %s to %s faild" % (srcFile, destDir))  
        return False  
    return True  
  
def getmodules() :  
    a = getModulesPath()  
    extractFiles("testpg\\", a) #��ȡ����ļ���ŵ�testpgĿ¼��

getmodules()
