#coding=utf-8
import os
import sys
from xml.dom import minidom
import codecs
from mako.template import Template
sys.path.append(os.getcwd())
xmlfilepath = "./common/"
outfilepath = "../../game/protocol/"
class base:
    datatype=""
    minval=0
    maxval=0
    def __init__(self,datatype,minval,maxval):
        self.datatype=datatype
        self.minval=minval
        self.maxval = maxval
basetype=dict()
basetype["i8"]=base("int8_t",-128,127)
basetype["u8"]=base("uint8_t",0,0xff)
basetype["i16"]=base("int16_t",-32768,32767)
basetype["u16"]=base("uint16_t",0,0xffff)
basetype["i32"]=base("int32_t",-2147483648,2147483647)
basetype["u32"]=base("uint32_t",0,0xffffffff)
basetype["i64"]=base("int64_t",-9223372036854775808,9223372036854775807)
basetype["u64"]=base("uint64_t",0,0xffffffffffffffff)
class enumdata:
    name=""
    value=""
    desc=""
class enumtype:
    name=""
    enumtype=""
    desc=""
    def __init__(self):
        self.data=list()
def ErrorFuc(msg,value):
    print msg,value
    raw_input()
    exit(1)
doc = minidom.parse(xmlfilepath+"enum.xml") 
root = doc.documentElement
allnode = root.getElementsByTagName("enum")
allenum=list()
enumlist=list()
for node in allnode:
    enum = enumtype()
    enum.name=node.getAttribute("name")
    if enum.name in enumlist:
        ErrorFuc("error enum type is repeat",enum.name)
    enumlist.append(enum.name)
    enum.enumtype = node.getAttribute("type")
    if enum.enumtype not in basetype:
        ErrorFuc("error type no support",enum.enumtype)
    enum.desc=node.getAttribute("desc")
    datanode = node.getElementsByTagName("data")
    valuelist=list()
    for data in datanode:
        value = enumdata()
        value.name=data.getAttribute("name")
        if value.name in valuelist:
             ErrorFuc( " error value is repeat",enum.name)
        valuelist.append(value.name)
        value.value = data.getAttribute("value")
        if value.value:
            tempval = 0
            if value.value[0:2] == "0x":
                tempval=int(value.value,16)
            else:
                tempval = int(value.value)
            if tempval < basetype[enum.enumtype].minval or tempval > basetype[enum.enumtype].maxval:
                ErrorFuc("value is max or min than base type",tempval)
        value.desc = data.getAttribute("desc")
        enum.data.append(value)
    enum.enumtype=basetype[enum.enumtype].datatype
    allenum.append(enum)
fp = codecs.open(outfilepath+"protocolenum.h", "wb", "utf_8_sig")
codetemp= Template(filename='enumtemp.txt',output_encoding='utf-8')
fp.write(codetemp.render(arrenum=allenum).decode('utf-8'))
fp.close()
#ErrorFuc( "build enum type success","")
