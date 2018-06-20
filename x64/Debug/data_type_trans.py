import sys

def py_int(nNum):
    print("python: %s:%d" %(type(nNum),nNum))
    return nNum

def py_double(dNum):
    '''
      Floating point numbers are usually implemented using double in C
    '''
    print("python:" + str(sys.float_info))
    print("python: %s:%f" %(type(dNum),dNum))
    return dNum

def py_string(str):
    print(type(str))
    print("python:" + str)
    return str
