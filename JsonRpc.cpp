#include "JsonRpc.h"
#include <iostream>
#include <string.h>
#include <cstring>

using std::string;

using namespace std;

JsonRpcRequest::JsonRpcRequest()
{
	cout << "create JsonRpcRequest obj successfully" << endl;
}
JsonRpcRequest::JsonRpcRequest(const std::string &json)
{
	Json::Reader reader;
    //reader将Json字符串解析到json_str，json_str将包含Json里所有子元素
    if (!reader.parse(json,json_str))
    {
		cerr << "json parse failed" << endl;
	    return;
	}
}

int JsonRpcRequest::Validate()
{
	if((json_str.isMember("jsonrpc") != true))
	
		return NO_JSONRPC;
	
	if((json_str.isMember("method")) != true )
	
		return NO_METH;

	if((json_str.isMember("id")) != true )

		return NO_ID;

	if((json_str.isMember("params")) != true)

		return NO_PARAMS;
	else 

	return 0;
}

std::string JsonRpcRequest::ToString()
{
	Json::FastWriter writer;
 	//生成请求json格式报文
 	return writer.write(json_str);
 }

void JsonRpcRequest::SetJsonRpc(const std::string &jsonrpc) 
{
	json_str["jsonrpc"] = jsonrpc;
}

std::string JsonRpcRequest::GetJsonRpc()
{
	return (json_str["jsonrpc"].asString());
}

void JsonRpcRequest::SetMethod(const std::string &method)
{
	json_str["method"] = method;
}

std::string JsonRpcRequest::GetMethod()
{
	return (json_str["method"].asString());
}

//这是个数组,或者是嵌套的json数据格式，很多形式怎么操作呢？
void JsonRpcRequest::SetParams(const std::string &param)
{
	Json::Reader reader;
	Json::Value params;

    reader.parse(param,params);

    json_str["params"] = params;
}

Json::Value& JsonRpcRequest::GetParams()
{
    //Json::FastWriter writer;
	//return writer.write(json_str["params"]);
	return json_str["params"];
}

void JsonRpcRequest::SetId(int id)
{
	json_str["id"] = id;
}

int JsonRpcRequest::GetId()
{
	return (json_str["id"].asInt());
}

JsonRpcResponse::JsonRpcResponse()
{
	cout << "create JsonRpcResponse obj successfully" <<endl;
}

JsonRpcResponse::JsonRpcResponse(const std::string &json)
{
	Json::Reader reader;
    //reader将Json字符串解析到json_str，json_str将包含Json里所有子元素
    if (!reader.parse(json,json_str))
    {
		cerr << "json parse failed" << endl;
	    return;
	}
	else cout << "json parse successfully" << endl;
}

int JsonRpcResponse::Validate()
{
	if((json_str.isMember("result"))&&(json_str.isMember("error")))
		return INVALID_INCLUDE;

	if ((json_str.isMember("jsonrpc") != true))
		return NO_JSONRPC;

	if ( json_str.isMember("result") != true && json_str.isMember("error") != true )
		return NO_RESULT_OR_ERROR;

	return 0;
	
/*以下是code不符合
	if(json_str.isMember("error"))
	{

	if( !((json_str["error"]["code"].asInt() == -32700) 
			|| (json_str["error"]["code"].asInt() == -32600) 
			|| (json_str["error"]["code"].asInt() == -32601) 
			|| (json_str["error"]["code"].asInt() == -32602) 
			|| (json_str["error"]["code"].asInt() == -32603) 
			|| ((json_str["error"]["code"].asInt() < -32000) && (json_str["error"]["code"].asInt()) > -32099)))
		
		return NO_THIS_CODE;
//以下是message不符合
    if(strcmp(json_str["error"]["message"].asString().c_str(),"Parse error") != 0 
		&& strcmp(json_str["error"]["message"].asString().c_str(),"Invalid Request") != 0 
		&& strcmp(json_str["error"]["message"].asString().c_str(),"Method not found") != 0 
		&& strcmp(json_str["error"]["message"].asString().c_str(),"Invalid params") != 0 
		&& strcmp(json_str["error"]["message"].asString().c_str(),"Internal error") != 0 
		&& strcmp(json_str["error"]["message"].asString().c_str(),"Server error") != 0 ) 

		return NO_THIS_MESSAGE;
//以下是code,message不匹配
	if(json_str["error"]["code"].asInt() == -32700) 
	{
		strcmp(json_str["error"]["message"].asString().c_str(),"Parse error") != 0;
		return MISMATCH_CODE_MESSAGE;
	}

	if(json_str["error"]["code"].asInt() == -32600) 
	{
		strcmp(json_str["error"]["message"].asString().c_str(),"Invalid Request") != 0;
		return MISMATCH_CODE_MESSAGE;
	}

	if(json_str["error"]["code"].asInt() == -32601)
	{
		strcmp(json_str["error"]["message"].asString().c_str(),"Method not found") != 0;
		return MISMATCH_CODE_MESSAGE;
	}

	if(json_str["error"]["code"].asInt() == -32602)
	{
		strcmp(json_str["error"]["message"].asString().c_str(),"Invalid params") != 0;
		return MISMATCH_CODE_MESSAGE;
	}

	if(json_str["error"]["code"].asInt() == -32603)
	{
		strcmp(json_str["error"]["message"].asString().c_str(),"Internal error") != 0;
		return MISMATCH_CODE_MESSAGE;
	}

	if(json_str["error"]["code"].asInt() > -32099 && json_str["error"]["code"].asInt() < -32000)
	{
			strcmp(json_str["error"]["message"].asString().c_str(),"Server error") != 0;
			return MISMATCH_CODE_MESSAGE;
	}

	}

//以下判断的匹配重复
//    if(strcmp(json_str["error"]["message"].asString().c_str(),"Parse error") == 0)
//	{
//		json_str["error"]["code"].asInt() != -32700;
//		return MISMATCH_CODE;
//	}
//
//	if(strcmp(json_str["error"]["message"].asString().c_str(),"Invalid Request") == 0)
//	{
//		json_str["error"]["code"].asInt() != -32600;
//		return MISMATCH_CODE;
//	}
//
//	if(strcmp(json_str["error"]["message"].asString().c_str(),"Method not found") == 0)
//	{
//		json_str["error"]["code"].asInt() != -32601;
//		return MISMATCH_CODE;
//	}
//
//	if(strcmp(json_str["error"]["message"].asString().c_str(),"Invalid params") == 0)
//	{
//		json_str["error"]["code"].asInt() != -32602;
//		return MISMATCH_CODE;
//	}
//
//	if(strcmp(json_str["error"]["message"].asString().c_str(),"Internal error") == 0)
//	{
//		json_str["error"]["code"].asInt() != -32603;
//		return MISMATCH_CODE;
//	}
//	if(strcmp(json_str["error"]["message"].asString().c_str(),"Server error") == 0)
//	{
//		(json_str["error"]["code"].asInt() < -32099 && json_str["error"]["code"].asInt()> -32000);
//		return MISMATCH_CODE;
//	}
	*/
	
}

std::string JsonRpcResponse::ToString()
{
	Json::FastWriter writer;
	
 	return writer.write(json_str);
}

void JsonRpcResponse::SetJsonRpc(const std::string &jsonrpc) 
{
	json_str["jsonrpc"] = jsonrpc;
}

std::string JsonRpcResponse::GetJsonRpc()
{
	return (json_str["jsonrpc"].asString());
}

void JsonRpcResponse::SetResult(const std::string &result)
{
		json_str["result"] = result;
}

Json::Value& JsonRpcResponse::GetResult()
{
	return (json_str["result"]) ;
}


/*void JsonRpcResponse::SetResult(const std::string &Result)
{
	Json::Value result;
	Json::Reader reader;


	reader.parse(Result,result);

	json_str["result"] = result;
}


std::string JsonRpcResponse::GetResult()
{
	Json::FastWriter writer;

	return writer.write(json_str["result"]);
}*/

void JsonRpcResponse::SetError(const std::string &Error)
{
		Json::Reader reader;
		Json::Value error;
		
		reader.parse(Error,error);

		json_str["error"] = error;//对象嵌套
}

std::string JsonRpcResponse::GetError()
{
	Json::FastWriter writer;
	return writer.write(json_str["error"]);
}

void JsonRpcResponse::SetId(int id)
{
	json_str["id"] = id;
}

int JsonRpcResponse::GetId()
{
	return (json_str["id"].asInt());
}

void JsonRpcResponse::Insert(Json::Value &value)
{
	Json::Value::Members mem = value.getMemberNames();
	
	//这里读到value里的“result”key就会赋值，就是赋值json对象
	for(auto iter = mem.begin();iter != mem.end(); iter++ )
	{
		json_str[*iter] = value[*iter];
	}	
		
	//for (auto iter : value.getMemberNames()) 
	//{
	//	json_str[*iter] = value[*iter];
	//}
}

