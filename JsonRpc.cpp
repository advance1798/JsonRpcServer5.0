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

JsonRpcRequest::JsonRpcRequest(const JsonRpcRequest &a) : json_str(a.json_str)
{

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

 	return writer.write(json_str);
}

void JsonRpcRequest::SetJsonRpc() 
{
	json_str["jsonrpc"] = "2.0";
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

JsonRpcResponse::JsonRpcResponse(const JsonRpcResponse &a):json_str(a.json_str)
{
 	
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
}

std::string JsonRpcResponse::ToString()
{
	Json::FastWriter writer;
 	//生成请求json格式报文
 	return writer.write(json_str); 	
}

void JsonRpcResponse::SetJsonRpc() 
{
	json_str["jsonrpc"] = "2.0";
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
	
	for(auto iter = mem.begin();iter != mem.end(); iter++ )
	{
		json_str[*iter] = value[*iter];
	}		
	//for (auto iter : value.getMemberNames()) 
	//{
	//	json_str[*iter] = value[*iter];
	//}
}

MJsonRpcRequest::MJsonRpcRequest()
{

}

MJsonRpcRequest::MJsonRpcRequest(const std::string &json)
{
	Json::Reader reader;
	if(!reader.parse(json,jsondata))
	{
		cerr << "json parse failed" << endl;
		return;
	}
	n = jsondata.size();

	for(int i = 0;i < n;i ++)
	{	
		Json::FastWriter writer;
		JsonRpcRequest A(writer.write(jsondata[i]));
		V.push_back(A);
	}

	if(jsondata.type() == Json::arrayValue)
		flag = 1;
	else
		flag = 0;
}

void MJsonRpcRequest::InsertJsonObj(JsonRpcRequest &obj)
{
	V.push_back(obj);
}

std::string MJsonRpcRequest::ToString()
{
	std::string r("[");
	for (auto i : V)//将目标对象中的所有子对象进行遍历
	{	
		r += i.ToString()+",";
	}

	r.pop_back();
	r += "]";
	return r;
}

JsonRpcRequest & MJsonRpcRequest::operator[](int n)
{
 	return V[n];
}

int MJsonRpcRequest::GetSize()
{
	return n;
}

bool MJsonRpcRequest::GetFlag()
{
	return flag;
}

MJsonRpcResponse::MJsonRpcResponse()
{

}

MJsonRpcResponse::MJsonRpcResponse(const std::string &json)
{
	Json::Reader reader;
	if(!reader.parse(json,jsondata))
	{
		cerr << "json parse failed" << endl;
		return;
	}
	n = jsondata.size();

	for(int i = 0;i < n; i++)
	{
		Json::FastWriter writer;
		JsonRpcResponse A(writer.write(jsondata[i]));
		V.push_back(A);
	}

	if(jsondata.type() == Json::arrayValue)
		flag = 1;
	else
		flag = 0;
}

void MJsonRpcResponse::InsertJsonObj(JsonRpcResponse &obj)
{
	V.push_back(obj);
}

std::string MJsonRpcResponse::ToString()
{
	std::string r("[");
	for (auto i : V)//将目标对象中的所有子对象进行遍历
		r += i.ToString()+",";

	r.pop_back();
	r += "]";
	return r;
}

JsonRpcResponse & MJsonRpcResponse::operator[](int n)
{
 	return V[n];
}

int MJsonRpcResponse::GetSize()
{
	return n;
}

bool MJsonRpcResponse::GetFlag()
{
	return flag;
}