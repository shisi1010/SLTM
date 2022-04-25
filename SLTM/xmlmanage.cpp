#include "pch.h"
#include "xmlmanage.h"
static BOOL string2bool(string str)
{
	if (0 == str.compare("true"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static INT_ARRAY string2intValue(string str)
{
	INT_ARRAY stTestValue;
	int i;
	const char *flag = ",";
	char *p = NULL;
	char strchar[64] = { 0 };
	memset(&stTestValue, 0, sizeof(stTestValue));

	for (i = 0; i < str.length(); i++)
	{
		strchar[i] = str.c_str()[i];
	}

	//    p = strtok(strchar, flag);
	strtok_s(strchar, flag, &p);
	while (NULL != p)
	{
		stTestValue.value[stTestValue.num] = atoi(p);
		stTestValue.num++;
		//        p = strtok(NULL, flag);
		strtok_s(NULL, flag, &p);
	}

	return stTestValue;
}

static int string2logLevel(string str)
{
	int level = DEBUG_LOG;

	if (0 == str.compare("Debug"))
	{
		level = DEBUG_LOG;
	}
	else if (0 == str.compare("Key"))
	{
		level = KEY_LOG;
	}
	else if (0 == str.compare("Error"))
	{
		level = ERROR_LOG;
	}
	else if (0 == str.compare("Test"))
	{
		level = TEST_LOG;
	}
	else
	{
		level = OTHER_LOG;
	}

	return level;
}

/**	@fn		static int xmlNode2Bool(string str, void* value)
*	@brief	将xml报文的字段解析成BOOL类型
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2Bool(string str, void* value)
{
	BOOL result = TRUE;
	BOOL* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_BOOL_POINTER(value);

	result = string2bool(str);

	XML_ASSIGN_VALUE_TO_POINTER(result, p);

	return XML_SUCCESS;
}

/**	@fn		static int xmlNode2Int(string str, void* value)
*	@brief	将xml报文的字段解析成int类型
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2Int(string str, void* value)
{
	int result = 0;
	int* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_INT_POINTER(value);

	result = stoi(str.data(), 0, 10);

	XML_ASSIGN_VALUE_TO_POINTER(result, p);

	return XML_SUCCESS;
}

/**	@fn		static int xmlNode2Float(string str, void* value)
*	@brief	将xml报文的字段解析成float类型
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2Float(string str, void* value)
{
	float result = 0;
	float* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_FLOAT_POINTER(value);

	result = stof(str.data());

	XML_ASSIGN_VALUE_TO_POINTER(result, p);

	return XML_SUCCESS;
}

/**	@fn		static void xmlNode2LogLevel(string str, VALUE_TYPE* value)
*	@brief	将xml报文的字段解析成日志等级类型（整型）
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2LogLevel(string str, void* value)
{
	int result = 0;
	int* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_INT_POINTER(value);
	result = string2logLevel(str);

	XML_ASSIGN_VALUE_TO_POINTER(result, p);

	return XML_SUCCESS;
}

/**	@fn		static int xmlNode2IntArray(string str, void* value)
*	@brief	将xml报文的字段解析成IntArray类型
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2IntArray(string str, void* value)
{
	INT_ARRAY result;
	INT_ARRAY* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_INT_ARRAY_POINTER(value);

	memset(&result, 0, sizeof(result));
	result = string2intValue(str);

	XML_ASSIGN_VALUE_TO_POINTER(result.num, &p->num);
	XML_COPY_VALUE_TO_POINTER(result.value, p->value, sizeof(result.value));

	return XML_SUCCESS;
}

/**	@fn		static int xmlNode2String(string str, void* value)
*	@brief	将xml报文的字段解析成字符串类型
*	@param [in]		string str				xml报文字符串
*	@param [out]	void* value	xml报文对应字符串的值
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlNode2String(string str, void* value)
{
	char result[MAX_XML_NODE_STRING_LENGTH] = { 0 };
	char* p = NULL;

	if (NULL == value)
	{
		return XML_NODE_INPUT_ERROR;
	}

	p = XML_TO_CHAR_POINTER(value);
	sprintf(result, "%s", str.data());

	XML_COPY_VALUE_TO_POINTER(result, p, MAX_XML_NODE_STRING_LENGTH);

	return XML_SUCCESS;
}

/**	@fn		static int xmlCopyValue(void* valueIn, void* valueOut, int size)
*	@brief	将valueIn值拷贝给valueOut
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return		XML_SUCCESS		成功
XML_NODE_INPUT_ERROR	入参错误
*/
static int xmlCopyValue(void* valueIn, void* valueOut, int size)
{
	if ((NULL == valueIn) || (NULL == valueOut))
	{
		return XML_NODE_INPUT_ERROR;
	}

	XML_COPY_VALUE_TO_POINTER(valueIn, valueOut, size);

	return XML_SUCCESS;
}

/**	@fn		static string xmlSetBool(VALUE_TYPE* value)
*	@brief	将BOOL值转为字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetBool(void* value)
{
	string str;
	BOOL src = TRUE;

	if (NULL == value)
	{
		return NULL;
	}

	XML_ASSIGN_POINTER_TO_VALUE(XML_TO_BOOL_POINTER(value), src);

	if (src)
	{
		str = "true";
	}
	else
	{
		str = "false";
	}

	return str;
}

/**	@fn		static string xmlSetInt(void* value)
*	@brief	将INT值转为字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetInt(void* value)
{
	string str;
	int src = 0;

	if (NULL == value)
	{
		return NULL;
	}

	XML_ASSIGN_POINTER_TO_VALUE(XML_TO_INT_POINTER(value), src);
	str = to_string(src);

	return str;
}

/**	@fn		static string xmlSetFloat(void* value)
*	@brief	将float值转为字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetFloat(void* value)
{
	string str;
	float src = 0;

	if (NULL == value)
	{
		return NULL;
	}

	XML_ASSIGN_POINTER_TO_VALUE(XML_TO_FLOAT_POINTER(value), src);

	str = to_string(src);

	return str;
}

/**	@fn		static string xmlSetIntArray(void* value)
*	@brief	将整型数组值转为字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetIntArray(void* value)
{
	int i = 0;
	string str;
	INT_ARRAY src;

	if (NULL == value)
	{
		return NULL;
	}

	memset(&src, 0, sizeof(src));

	XML_COPY_VALUE_TO_POINTER(XML_TO_INT_ARRAY_POINTER(value), &src, sizeof(INT_ARRAY));

	for (i = 0; i < src.num; i++)
	{
		str += to_string(src.value[i]);
		if (i < src.num - 1)
		{
			str += ",";
		}
	}

	return str;
}

/**	@fn		static string xmlSetString(VALUE_TYPE* value)
*	@brief	将char数组值转为string字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetString(void* value)
{
	string str;
	char src[MAX_XML_NODE_STRING_LENGTH] = { 0 };

	if (NULL == value)
	{
		return NULL;
	}

	XML_COPY_VALUE_TO_POINTER(XML_TO_CHAR_POINTER(value), &src, sizeof(src));
	str = src;

	return str;
}

/**	@fn		static string xmlSetString(VALUE_TYPE* value)
*	@brief	将日志等级数组值转为string字符串
*	@param [in]		void* valueIn
*	@param [out]	void* valueOut
*	@return
*/
static string xmlSetLogLevel(void* value)
{
	string str;
	int src = 0;

	if (NULL == value)
	{
		return NULL;
	}

	XML_ASSIGN_POINTER_TO_VALUE(XML_TO_INT_POINTER(value), src);

	switch (src)
	{
	case DEBUG_LOG:
		str = "Debug";
		break;
	case KEY_LOG:
		str = "Key";
		break;
	case ERROR_LOG:
		str = "Error";
		break;
	}

	return str;
}

int XmlGetChildValueByLocatName(string name, XmlNode* strList, int strCount)
{
	int i = 0;
	for (i = 0; i < strCount; i++)
	{
		if (0 == strList->strName.compare(name))
		{
			return i;
		}
		else
		{
			strList++;
		}
	}

	return XML_NODE_NOT_FOUNT;
}

int getXmlNodeList(char* p_buf, char* str, XmlNode* xmlNodeList)
{
	//解析xml报文
	char* p = NULL;
	string strName;
	string strValue;
	int cmdCount = 0;
	TiXmlDocument myXml;

	p = strstr((char*)p_buf, str);
	myXml.Parse(p);
	TiXmlElement* node = myXml.RootElement();
	TiXmlElement* nodeTemp = NULL;

	node = node->FirstChildElement();
	do
	{
		strName = node->Value();

		if (NULL == node->FirstChildElement())
		{
			strValue = node->GetText();
			xmlNodeList->strName = strName;
			xmlNodeList->strValue = strValue;
			node = node->NextSiblingElement();
			xmlNodeList++;
			cmdCount++;
		}
		else
		{
			nodeTemp = node;
			node = node->FirstChildElement();
		}

		if ((NULL == node) && (NULL != nodeTemp))
		{
			node = nodeTemp->NextSiblingElement();
		}

	} while (NULL != node);

	return cmdCount;
}

ST_XML_GET_FUNCTION_TABLE g_type_fun_tbl[] =
{
	{ XML_NODE_BOOL, sizeof(BOOL), xmlNode2Bool, xmlCopyValue, xmlSetBool },
	{ XML_NODE_INT, sizeof(int), xmlNode2Int, xmlCopyValue, xmlSetInt },
	{ XML_NODE_FLOAT, sizeof(float), xmlNode2Float, xmlCopyValue, xmlSetFloat },
	{ XML_NODE_STRING, MAX_XML_NODE_STRING_LENGTH, xmlNode2String, xmlCopyValue, xmlSetString },
	{ XML_NODE_LOG_LEVEL, sizeof(int), xmlNode2LogLevel, xmlCopyValue, xmlSetLogLevel },
	{ XML_NODE_INT_ARRAY, sizeof(INT_ARRAY), xmlNode2IntArray, xmlCopyValue, xmlSetIntArray },
};

void getXmlNodeValue(ST_XML_GET_NODE_VALUE_TABLE* p_xmlNodeTbl, int tblCount, XmlNode* p_xmlNodeList, int listCount)
{
	int i = 0;
	int j = 0;
	int xmlNodeNum;
	int typeSize = 0;	//类型的大小
	P_XML_GET_VALUE_CALL_FUNC getFuntion = NULL;
	P_XML_COPY_VALUE_CALL_FUNC copyFuntion = NULL;
	ST_XML_GET_NODE_VALUE_TABLE* xmlNodeTbl = NULL;

	if ((NULL == p_xmlNodeTbl) || (NULL == p_xmlNodeList))
	{
		return;
	}

	xmlNodeTbl = p_xmlNodeTbl;

	for (i = 0; i < tblCount; i++)
	{
		if (NULL == xmlNodeTbl->value)
		{
			//赋值指针为空，不需要赋值，直接continue
			xmlNodeTbl++;
			continue;
		}

		if ((XML_NODE_HEAD == xmlNodeTbl->type) || (XML_NODE_TAIL == xmlNodeTbl->type))
		{
			//报文头尾节点不需要解析
			xmlNodeTbl++;
			continue;
		}

		//获取不同类型数据的处理函数
		for (j = 0; j < sizeof(g_type_fun_tbl) / sizeof(g_type_fun_tbl[0]); j++)
		{
			if (xmlNodeTbl->type == g_type_fun_tbl[j].type)
			{
				getFuntion = g_type_fun_tbl[j].getFunction;
				copyFuntion = g_type_fun_tbl[j].copyFunction;
				typeSize = g_type_fun_tbl[j].size;
				break;
			}
		}

		//在p_xmlNodeList中找到指定名字的节点的值（字符串）
		xmlNodeNum = XmlGetChildValueByLocatName(xmlNodeTbl->name, p_xmlNodeList, listCount);

		if (XML_NODE_NOT_FOUNT != xmlNodeNum)
		{
			if (NULL != getFuntion)
			{
				getFuntion(p_xmlNodeList[xmlNodeNum].strValue, xmlNodeTbl->value);
			}

			//关联值不为空则将关联值赋值
			if ((NULL != xmlNodeTbl->rele_value) && (NULL != copyFuntion))
			{
				copyFuntion(xmlNodeTbl->value, xmlNodeTbl->rele_value, typeSize);
			}
		}

		xmlNodeTbl++;
	}
}

int setXmlNodeValue(ST_XML_GET_NODE_VALUE_TABLE* p_xmlNodeTbl, int tblCount, char* rootNodeName, char* xmlBuf, int xmlBufSize)
{
	int i = 0;
	int j = 0;
	int deep = 0;
	int dwRet;
	string str;
	CXmlBase xmlNode[10];
	P_XML_SET_VALUE_CALL_FUNC setFuntion = NULL;
	ST_XML_GET_NODE_VALUE_TABLE* xmlNodeTbl = NULL;

	if ((NULL == p_xmlNodeTbl) || (NULL == rootNodeName) || (NULL == xmlBuf))
	{
		return XML_NODE_INPUT_ERROR;
	}

	xmlNodeTbl = p_xmlNodeTbl;

	//构造root节点
	xmlNode[deep].CreateRoot(rootNodeName);
	xmlNode[deep].SetAttribute("version", "1.0");
	xmlNode[deep].SetAttribute("encoding", "UTF-8");

	for (i = 0; i < tblCount; i++)
	{
		//获取不同类型数据的处理函数
		for (j = 0; j < sizeof(g_type_fun_tbl) / sizeof(g_type_fun_tbl[0]); j++)
		{
			if (xmlNodeTbl->type == g_type_fun_tbl[j].type)
			{
				setFuntion = g_type_fun_tbl[j].setFunction;
				break;
			}
		}

		if (XML_NODE_HEAD == xmlNodeTbl->type)
		{
			deep++;	//报文深度 +1
			if (10 <= deep)
			{
				return XML_NODE_INPUT_ERROR;
			}
			xmlNode[deep].CreateRoot(xmlNodeTbl->name);
		}
		else if (XML_NODE_TAIL == xmlNodeTbl->type)
		{
			xmlNode[deep - 1].AddNode(xmlNode[deep]);
			xmlNode[deep - 1].OutOfElem();
			memset(&xmlNode[deep], 0, sizeof(xmlNode[deep]));
			deep--;//报文深度 -1
		}
		else
		{
			str = setFuntion(xmlNodeTbl->value);
			xmlNode[deep].AddNode(xmlNodeTbl->name, str);
			if ('\0' != xmlNodeTbl->opt[0])
			{
				xmlNode[deep].SetAttribute("opt", xmlNodeTbl->opt);
			}
			xmlNode[deep].OutOfElem();
		}

		xmlNodeTbl++;
	}

	if (0 != deep)
	{
		return XML_NODE_DEEP_ERROR;
	}
	else
	{
		xmlNode[0].WriteToBuf(xmlBuf, xmlBufSize, dwRet);
	}
}

