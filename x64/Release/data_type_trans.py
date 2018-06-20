import sys

def py_int(nNum):
    print("python: %s: %d" %(type(nNum),nNum))
    return nNum+50

def py_double(dNum):
    '''
      Floating point numbers are usually implemented using double in C
    '''
    #print("python:" + str(sys.float_info))
    py_int(30)
    print("python: %s: %f" %(type(dNum),dNum))
    return dNum+20

def py_string(str):
    print("python: %s: %s" %(type(str), str))
    try:
        #print(ord(str[1]))
        s = str.decode('GBK')
        print("python decode as GBK:\n %s" % s)
    except AttributeError as Attr_e:
        print(Attr_e)
    except TypeError as Type_e:
        print(Type_e)
    return str

if __name__ == "__main__":
    py_double(10.1)
    str = '我机器人IROBOT'.encode('GBK')
    py_string(str)
