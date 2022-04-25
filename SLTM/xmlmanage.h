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

//�����ַ���
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

//��valueֵ��ֵ��pָ��ĵ�ַ��pΪָ��
#define XML_ASSIGN_VALUE_TO_POINTER(value, p)	(*p = value)
//��pָ��ĵ�ֵַ��ֵ��value��pΪָ��
#define XML_ASSIGN_POINTER_TO_VALUE(p, value)	(value = *p)
//��value��ַָ���ֵ��ֵ��pָ��ĵ�ַ��value��p��Ϊָ��
#define XML_ASSIGN_POINTER_TO_POINTER(value, p)	(*p = *value)
//��value��ַָ���ֵ������p��ַָ��ĵ�ַ�ռ䣬value��p��Ϊָ��
#define XML_COPY_VALUE_TO_POINTER(src, dst, size)	memcpy(dst, src, size)


typedef int(*P_XML_GET_VALUE_CALL_FUNC)(string str, void* value);
typedef int(*P_XML_COPY_VALUE_CALL_FUNC)(void* valueIn, void* valueOut, int size);
typedef string(*P_XML_SET_VALUE_CALL_FUNC)(void* value);

//xml�ֶνṹ��
struct XmlNode
{
	string strName;		//�ֶ�����
	string strValue;	//�ֶε�ֵ
};

//mxml�ֶ�ֵ���ͽṹ��
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

//xml��ȡ�ֶ�ֵ�ṹ��
typedef struct
{
	char name[64];			//�ֶ�����
	E_XML_NODE_TYPE type;	//�ֶ�����
	void* value;			//��ֵ�ֶ�
	void* rele_value;		//����ֵ
	char opt[128];			//�ڵ�����
}ST_XML_GET_NODE_VALUE_TABLE;

//xml��ͬ�����ֶζ�Ӧ�Ĵ�������
typedef struct
{
	E_XML_NODE_TYPE type;	//xml�ֶ�ֵ������
	int	size;			//��Ӧ�ֶ����͵����ݴ�С
	P_XML_GET_VALUE_CALL_FUNC	getFunction;	//��ȡ��ͬ���Ͷ�Ӧֵ�Ĵ������������ַ���תΪ��ͬ�����ͣ�
	P_XML_COPY_VALUE_CALL_FUNC	copyFunction;	//������ͬ���Ͷ�Ӧֵ�Ĵ�����
	P_XML_SET_VALUE_CALL_FUNC	setFunction;	//���ò�ͬ�ֶζ�Ӧֵ�Ĵ����������Ѳ�ͬ������תΪ�ַ�����
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
