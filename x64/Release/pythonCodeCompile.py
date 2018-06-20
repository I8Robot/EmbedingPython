import traceback

def pythonCodeCompile(pyCode):
    str = "OK"
    try:
        mylocals = {"__name__": "__console__", "__doc__": None} # 必须要有，否则，无法定义多个函数，提示函数未定义
                                                            # NameError
        bCode = compile(pyCode, "测试", "exec")
        exec(bCode, mylocals)
    except Exception as e:
        #print("异常:\n")
        str = traceback.format_exc()
        #print("str:", str)
    return str

if __name__ == "__main__":
    pyCode = """   
def myTestFun():
    x = 0
    while x < 5:
        if x == 3:
            break
        x = x + 1
        print(x)
    return x

myTestFun()"""
    
    pythonCodeCompile(pyCode)
    
