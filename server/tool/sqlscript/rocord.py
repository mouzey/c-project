from mako.template import Template
from mako.runtime import Context
import re
import pymysql
import os
import io
import config
import codecs
conn = pymysql.connect(host=config.host_str,port=config.port_num,user=config.user_str,passwd=config.passwd_str,db=config.db_name_str,charset='utf8')
cursor=conn.cursor()
def output_to_file(filename,strbuf):
    f = codecs.open(filename, "w", "utf_8_sig")
    f.write(strbuf)
    f.close()
def GetType(field,sqltype,default):
    #print(default,type(default))
    field['escape']= 0
    field['format']= "%s"
    if re.search('bigint',sqltype):
        if re.search('unsigned',sqltype):
            field['Type']="uint64_t"
            field['format']= '%" PRIu64 "'
        else:
            field['Type']="int64_t"
            field['format']= '%" PRIi64 "'
        if default == None:
            field['Default']='0'
        else:
            field['Default']= default
    elif re.search('tinyint',sqltype):
        if re.search('unsigned',sqltype):
            field['Type']="uint8_t"
            field['format']= "%hhu"
        else:
            field['Type']="int8_t"
            field['format']= "%hhd"
        if default == None:
            field['Default']='0'
        else:
            field['Default']= default
    elif re.search('smallint',sqltype):
        if re.search('unsigned',sqltype):
            field['Type']="uint16_t"
            field['format']= "%hu"
        else:
            field['Type']="int16_t"
            field['format']= "%hd"
        if default == None:
            field['Default']='0'
        else:
            field['Default']= default
    elif re.search('int',sqltype):
        if re.search('unsigned',sqltype):
            field['Type']="uint32_t"
            field['format']= "%u"
        else:
            field['Type']="int32_t"
            field['format']= "%d"
        if default ==None:
            field['Default']='0'
        else:
            field['Default']= default
    elif re.search('datetime',sqltype):
        field['Type']="std::string"
        if default == None:
            field['Default']=''
        else:
            field['Default']= default
    elif re.search('text',sqltype) or re.search('blob',sqltype) or re.search('char',sqltype):
        field['Type']="std::string"
        field['Default']= ""
        field['escape']= 1
    else:
        raise Exception("this type not support%s"%sqltype)
def do_table(tb_name):
    print('[',tb_name,']')
    field_s = []
    cursor.execute("show columns from `%s`"%(tb_name)) 
    for column_info in cursor.fetchall():
            field = dict()
            GetType(field,column_info[1],column_info[4])
            field['Field'] = str(column_info[0])
            if re.search('PRI',column_info[3]):
                field["key"] = 1
            else:
                field["key"] = 0
            if re.search('auto_increment',column_info[5]):
                if field["key"] == 0:
                    raise Exception("auto col only pri key%s"%field['Field'])
                field["auto"] = 1
            else:
                field["auto"] = 0
            field['table']=tb_name
            field_s.append(field)
    #render xxxRecord.h
    template_h = Template(filename='Templatehead.txt')
    strbuf=template_h.render(fields=field_s,table=tb_name)
    output_to_file(config.out_dir + tb_name + 'record.h' , strbuf )
    
    ##render xxxxRecord.cpp
    template_cpp = Template(filename='Templatecpp.txt')
    strbuf = template_cpp.render(fields=field_s,table=tb_name)
    output_to_file(config.out_dir + tb_name +'record.cpp' , strbuf )
    
table_names=[]
cursor.execute("show tables")

for row in cursor.fetchall():
    table_names.append(row[0])

for tbname in table_names:
	do_table(tbname)
