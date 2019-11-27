#coding=utf-8
import os
import sys
from xml.dom import minidom
import codecs
from mako.template import Template
sys.path.append(os.getcwd())
xmlfilepath = "./common/"
outfilepath = "../../game/protocol/"
def GetEnumType():
    doc = minidom.parse(xmlfilepath+"enum.xml")
    root = doc.documentElement
    allnode = root.getElementsByTagName("enum")
    enumlist=dict()
    for node in allnode:
        datanode = node.getElementsByTagName("data")
        for data in datanode:
            enumlist[node.getAttribute("name")]="enm"+node.getAttribute("name")+"_"+data.getAttribute("name")
            break
    return enumlist#name,defualt value
class base:
    datatype=""
    default=""
    consttype=0
    def __init__(self,datatype,default,consttype):
        self.datatype=datatype
        self.default=default
        self.consttype=consttype
basetype=dict()
basetype["i8"]=base("int8_t","0",0)
basetype["u8"]=base("uint8_t","0",0)
basetype["i16"]=base("int16_t","0",0)
basetype["u16"]=base("uint16_t","0",0)
basetype["i32"]=base("int32_t","0",0)
basetype["u32"]=base("uint32_t","0",0)
basetype["i64"]=base("int64_t","0",0)
basetype["u64"]=base("uint64_t","0",0)
basetype["string"]=base("std::string","",1)
basetype["bool"]=base("bool","0",0)
basetype["double"]=base("double","0.0",0)
basetype["float"]=base("float","0.0",0)
basetype["map"]=base("std::map","",1)
basetype["multimap"]=base("std::multimap","",1)
basetype["unordered_map"]=base("std::unordered_map","",1)
basetype["unordered_multimap"]=base("std::unordered_multimap","",1)
basetype["set"]=base("std::set","",1)
basetype["multiset"]=base("std::multiset","",1)
basetype["unordered_set"]=base("std::unordered_set","",1)
basetype["unordered_multiset"]=base("std::unordered_multiset","",1)
basetype["vector"]=base("std::vector","",1)
basetype["list"]=base("std::list","",1)
basetype["forward_list"]=base("std::forward_list","",1)
basetype["deque"]=base("std::deque","",1)
class structdata:
    name=""
    desc=""
    datatype=""
    default=""
    consttype=0
    keycode=1
class structtype:
    name=""
    desc=""
    decode=1
    def __init__(self):
        self.data=list()
def ErrorFuc(msg,value):
    print msg,value
    raw_input()
    exit(1)
doc = minidom.parse(xmlfilepath+"struct.xml") 
root = doc.documentElement
allnode = root.getElementsByTagName("struct")
enumlist=GetEnumType()
structlist= list()
datalist=list()
for node in allnode:
    struct= structtype()
    struct.name=node.getAttribute("name")
    if struct.name in structlist:
        ErrorFuc("error type is repeat in struct",struct.name)
    structlist.append(struct.name)
    struct.desc=node.getAttribute("desc")
    datanode = node.getElementsByTagName("data")
    valuelist=list()
    for data in datanode:
        value = structdata()
        value.name=data.getAttribute("name")
        if value.name in valuelist:
             ErrorFuc( " error value is repeat",struct.name)
        valuelist.append(value.name)
        value.desc=data.getAttribute("desc")
        value.datatype=data.getAttribute("type")
        if value.datatype in enumlist:
            value.default = enumlist[value.datatype]
            value.datatype="E"+value.datatype
        elif value.datatype in structlist:
            value.consttype = 1
            value.datatype="S"+value.datatype
        elif value.datatype  in basetype:
            value.default = basetype[value.datatype].default
            value.consttype = basetype[value.datatype].consttype
            value.datatype=basetype[value.datatype].datatype
        else:
            ErrorFuc( "has no type",struct.name)
        arr=data.getAttribute("arr")
        if arr != "":
            value.default = ""
            if arr != "map" and arr != "vector":
                ErrorFuc("arr attr has error",struct.name)
            arrtype=data.getAttribute("arrtype")
            keytype=data.getAttribute("keytype")
            if arrtype == "":
                arrtype = arr
            if keytype != "":
                if keytype in enumlist:
                    keytype="E"+keytype
                else:
                    keytype=basetype[keytype].datatype
                value.datatype=basetype[arrtype].datatype+"<"+keytype+","+value.datatype+">"
            else:
                value.datatype=basetype[arrtype].datatype+"<"+value.datatype+">"
            value.consttype = 1
            if arr=="vector" and arrtype=="map":
                value.keycode=0
                struct.decode=0
        struct.data.append(value)
    datalist.append(struct)
fp = codecs.open(outfilepath+"protocolstruct.h", "wb", "utf_8_sig")
codetemp = Template(filename='structheadtemp.txt',output_encoding='utf-8')
fp.write(codetemp.render(arrstruct=datalist).decode('utf-8'))
fp.close()

fp = codecs.open(outfilepath+"protocolstruct.cpp", "wb", "utf_8_sig")
codetemp = Template(filename='structcpptemp.txt',output_encoding='utf-8')
fp.write(codetemp.render(arrstruct=datalist).decode('utf-8'))
fp.close()
#ErrorFuc( "build struct success","")
