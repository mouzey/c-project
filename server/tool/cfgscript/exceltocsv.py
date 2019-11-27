#coding=utf-8
import os
import sys
import codecs
import xlrd
import csv
from mako.template import Template
sys.path.append(os.getcwd())
pathname="./excelfile"
dstpath="../../game/config/"
clientpath="./client/"
serverpath="./server/"
class base:
    datatype=""
    default=""
    key=0#1 mease use for map key and not encode
    minval=0
    maxval=0
    def __init__(self,datatype,default,key,minval,maxval):
        self.datatype=datatype
        self.default=default
        self.key=key
        self.minval=minval
        self.maxval = maxval
basetype=dict()
basetype["i8"]=base("int8_t",0,1,-128,127)
basetype["u8"]=base("uint8_t",0,1,0,0xff)
basetype["i16"]=base("int16_t",0,1,-32768,32767)
basetype["u16"]=base("uint16_t",0,1,0,0xffff)
basetype["i32"]=base("int32_t",0,1,-2147483648,2147483647)
basetype["u32"]=base("uint32_t",0,1,0,0xffffffff)
basetype["i64"]=base("int64_t",0,1,-9223372036854775808,9223372036854775807)
basetype["u64"]=base("uint64_t",0,1,0,0xffffffffffffffff)
basetype["string"]=base("std::string","",0,0,0)
basetype["bool"]=base("bool","0",1,0,1)
basetype["double"]=base("double","0.0",0,0,0)
basetype["float"]=base("float","0.0",0,0,0)

class feilddata:
    datatype=""
    name=""
    default=""
    desc=""
def ErrorFuc(msg,value):
    print msg,value
    raw_input()
    exit(1)
def buildcsv(sheet,fileName,fileDic,pathname):
    csvfile = open(pathname+fileName.split('.')[0] +'.csv', 'wb')
    csvfile.write(codecs.BOM_UTF8) 
    writer = csv.writer(csvfile)
    i = 0
    while i < sheet.nrows:
        tmpvalue=list()
        for j in range(sheet.ncols):
            if sheet.cell(3,i).value != "" and sheet.cell(3,i).value != fileDic:
                continue
            if 0 == i :
                tmpvalue.append(sheet.cell(i,j).value)
                continue
            if sheet.cell(i,j).value == "":
                tmpvalue.append(basetype[sheet.cell(2,j).value].default)
                continue
            if sheet.cell(2,j).value == "string":
                try:
                    tmpvalue.append(sheet.cell(i,j).value.encode('utf-8'))
                except:
                    tmpvalue.append(sheet.cell(i,j).value)
                continue
            if basetype[sheet.cell(2,j).value].key == 0:
                tmpvalue.append(sheet.cell(i,j).value)
                continue
            try:
                if sheet.cell(i,j).value < basetype[sheet.cell(2,j).value].minval or sheet.cell(i,j).value > basetype[sheet.cell(2,j).value].maxval:
                    ErrorFuc("value max or min (%u,%u) type=%s min=%d max=%d"%(i+1,j+1,basetype[sheet.cell(2,j).value].datatype,
                                                                               basetype[sheet.cell(2,j).value].minval,basetype[sheet.cell(2,j).value].maxval),sheet.cell(i,j).value);
                strvalue='%0.f'%sheet.cell(i,j).value
                tmpvalue.append(strvalue)
            except:
                ErrorFuc("value has not match type(%u,%u)"%(i+1,j+1),"");
        writer.writerow(tmpvalue)
        if 0 == i:
            i = 3
        i += 1;
    csvfile.close()
def excelreadhead(filename,pathname):
    workbook = xlrd.open_workbook(pathname + "\\" +filename)
    sheet = workbook.sheet_by_index(0)
    buildcsv(sheet,filename,"client",clientpath)
    buildcsv(sheet,filename,"server",serverpath)
    valuename=list()
    datalist = list()
    i = 0
    while i < sheet.ncols:
        if sheet.cell(3,i).value == "client":
            i += 1
            continue
        data=feilddata()
        data.name = sheet.cell(0,i).value
        if data.name in valuename:
            ErrorFuc("feild repeat",data.name)
        valuename.append(data.name)
        data.name = "m_"+data.name
        data.desc = sheet.cell(1,i).value
        data.datatype = sheet.cell(2,i).value
        if data.datatype not in basetype:
            ErrorFuc("type not support",data.datatype)
        data.default = basetype[data.datatype].default
        data.datatype=basetype[data.datatype].datatype
        #print data.datatype,data.name,data.desc,data.default
        datalist.append(data)
        i += 1
    return datalist
def BuildCfg(filename,pathname):
    data = excelreadhead(filename,pathname)
    if 0 == len(data):
        return
    filename = filename.split('.')[0]
    codetemp = Template(filename='cfgheadtemp.txt',output_encoding='utf-8')
    tmpname = dstpath+filename+"cfg.h"
    fp = codecs.open(tmpname, "wb", "utf-8-sig")
    fp.write(codetemp.render(datalist=data,name=filename).decode('utf-8'))
    fp.close()
    tmpname = dstpath+filename+"loader.h"
    if not os.path.exists(tmpname):
        codetemp= Template(filename='cfgloadertemp.txt',output_encoding='utf-8')
        fp = codecs.open(tmpname, "wb", "utf-8-sig")
        fp.write(codetemp.render(name=filename,key=data[0].datatype).decode('utf-8'))
        fp.close()
    
        codetemp= Template(filename='cfgcpptemp.txt',output_encoding='utf-8')
        tmpname = dstpath+filename+"loader.cpp"
        fp = codecs.open(tmpname, "wb", "utf-8-sig")
        fp.write(codetemp.render(name=filename).decode('utf-8'))
        fp.close()
for root, dirs, files in os.walk(pathname):
    for filepath in files:
        BuildCfg(filepath,pathname)
#raw_input()
