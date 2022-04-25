#ifndef __XML_MANAGE_H__
#define __XML_MANAGE_H__


#include "xmlbase.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define XML_SUCCESS				0
#define XML_NODE_NOT_FOUNT		-1
#define XML_NODE_INPUT_ERROR	-2
#define XML_NODE_DEEP_ERROR		-3

#define MAX_INT_ARRAY_VALUE_COUNT	21
#define MAX_XML_NODE_STRING_LENGTH	64

//整型字符串
typedef struct
{
	int num;
	int value[MAX_INT_ARRAY_VALUE_COUNT];
}INT_ARRAY;

#define XML_TO_BOOL_POINTER(p)		(BOOL*)(p)
#define XML_TO_CHAR_POINTER(p)		(char*)(p)
#define XML_TO_INT_POINTER(p)		(int*)(p)
#define XML_TO_FLOAT_POINTER(p)		(float*)(p)
#define XML_TO_INT_ARRAY_POINTER(p)	(INT_ARRAY*)(p)

//将value值赋值给p指向的地址，p为指针
#define XML_ASSIGN_VALUE_TO_POINTER(value, p)	(*p = value)
//将p指向的地址值赋值给value，p为指针
#define XML_ASSIGN_POINTER_TO_VALUE(p, value)	(value = *p)
//将value地址指向的值赋值给p指向的地址，value和p均为指针
#define XML_ASSIGN_POINTER_TO_POINTER(value, p)	(*p = *value)
//将value地址指向的值拷贝到p地址指向的地址空间，value和p均为指针
#define XML_COPY_VALUE_TO_POINTER(src, dst, size)	memcpy(dst, src, size)


typedef int(*P_XML_GET_VALUE_CALL_FUNC)(string str, void* value);
typedef int(*P_XML_COPY_VALUE_CALL_FUNC)(void* valueIn, void* valueOut, int size);
typedef string(*P_XML_SET_VALUE_CALL_FUNC)(void* value);

//xml字段结构体
struct XmlNode
{
	string strName;		//字段名称
	string strValue;	//字段的值
};

//mxml字段值类型结构体
typedef enum
{
	XML_NODE_HEAD,
	XML_NODE_TAIL,
	XML_NODE_BOOL,
	XML_NODE_INT,
	XML_NODE_FLOAT,
	XML_NODE_STRING,
	XML_NODE_LOG_LEVEL,
	XML_NODE_INT_ARRAY,
}E_XML_NODE_TYPE;

//xml获取字段值结构体
typedef struct
{
	char name[64];			//字段名称
	E_XML_NODE_TYPE type;	//字段类型
	void* value;			//赋值字段
	void* rele_value;		//关联值
	char opt[128];			//节点属性
}ST_XML_GET_NODE_VALUE_TABLE;

//xml不同类型字段对应的处理函数表
typedef struct
{
	E_XML_NODE_TYPE type;	//xml字段值的类型
	int	size;			//对应字段类型的数据大小
	P_XML_GET_VALUE_CALL_FUNC	getFunction;	//获取不同类型对应值的处理函数（即把字符串转为不同的类型）
	P_XML_COPY_VALUE_CALL_FUNC	copyFunction;	//拷贝不同类型对应值的处理函数
	P_XML_SET_VALUE_CALL_FUNC	setFunction;	//设置不同字段对应值的处理函数（即把不同的类型转为字符串）
}ST_XML_GET_FUNCTION_TABLE;

enum LOG_LEVEL
{
	DEBUG_LOG,
	KEY_LOG,
	ERROR_LOG,
	TEST_LOG,

	OTHER_LOG,
};

int XmlGetChildValueByLocatName(string name, XmlNode* strList, int strCount);
int getXmlNodeList(char* p_buf, char* str, XmlNode* xmlNodeList);
void getXmlNodeValue(ST_XML_GET_NODE_VALUE_TABLE* p_xmlNodeTbl, int tblCount, XmlNode* p_xmlNodeList, int listCount);
int setXmlNodeValue(ST_XML_GET_NODE_VALUE_TABLE* p_xmlNodeTbl, int tblCount, char* rootNodeName, char* xmlBuf, int xmlBufSize);

#endif
