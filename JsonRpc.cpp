#include "JsonRpc.h"
#include <iostream>
#include <string.h>
#include <cstring>

using std::string;

using namespace std;

JsonRpcRequest::JsonRpcRequest()
{
	json_str["jsonrpc"] = "2.0";
}

JsonRpcRequest::JsonRpcRequest(const std::string &json)
{
	Json::Reader reader;
    //reader将Json字符串解析到json_str，json_str将包含Json里所有子元素
    if (!reader.parse(json,json_str))
    {
    	json_str.clear();//来判断解析失败
	    return;
	}
}

JsonRpcRequest::JsonRpcRequest(const JsonRpcRequest &a) : json_str(a.json_str)
{

}

int JsonRpcRequest::Validate()
{
	if(json_str.empty())
		
		return PARSE_FAILED;

	if((json_str.isMember("jsonrpc") != true))
	
		return NO_JSONRPC;
	
	if((json_str.isMember("method")) != true )
	
		return NO_METH;

	if((json_str.isMember("params")) != true)

		return NO_PARAMS;

	if(!(json_str["params"].type() == Json::arrayValue || json_str["params"].type() == Json::objectValue))

		return INVALID_PARAMS;

	else 

	return 0;
}

bool JsonRpcRequest::IsNotify()
{
	if(json_str.isMember("id")!= true)
		return 1;
}

std::string JsonRpcRequest::ToString()
{
	Json::FastWriter writer;

 	return writer.write(json_str);
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
	json_str["jsonrpc"] = "2.0";
}

JsonRpcResponse::JsonRpcResponse(const std::string &json)
{
	Json::Reader reader;
    //reader将Json字符串解析到json_str，json_str将包含Json里所有子元素
    if (!reader.parse(json,json_str))
    {
    	json_str.clear();
	    return;
	}
}


JsonRpcResponse::JsonRpcResponse(const JsonRpcResponse &a):json_str(a.json_str)
{
 	
}

int JsonRpcResponse::Validate()
{
	if(json_str.empty())
		return PARSE_FAILED;

	if((json_str.isMember("jsonrpc") != true))
		return NO_JSONRPC;

	if(!( json_str.isMember("result") == true || json_str.isMember("error") == true ))
		return NO_RESULT_OR_ERROR;

	/*if(json_str["result"].isString()&&json_str["error"].isString())
		return INVALID_INCLUDE;*/
	if((json_str.isMember("result") == true)&&(json_str.isMember("error")== true))
		return INVALID_INCLUDE;

	if((json_str.isMember("error") == true))
	{
		if(json_str["error"].type() == Json::arrayValue || json_str["error"].type() == Json::objectValue)
			return GET_ERROR;
		else
			return INVALID_ERROR;
	}

	if((json_str.isMember("result") == true))
		return GET_RESULT;

	return 0;
}

bool JsonRpcResponse::IsNotify()
{
	if(json_str.isMember("id")!= true)
		return 1;
}

std::string JsonRpcResponse::ToString()
{
	Json::FastWriter writer;
 	//生成请求json格式报文
 	return writer.write(json_str); 	
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
	Json::Value jsondata;//设置成局部对象
	if(!reader.parse(json,jsondata))
	{
		cerr << "json parse failed" << endl;
		return;
	}

	Json::FastWriter writer;

	if(jsondata.isArray())
	{
		flag = true;
		n = jsondata.size();
		for(int i = 0; i < n; i++)
		{	
			JsonRpcRequest A(writer.write(jsondata[i]));
			V.push_back(A);
		}
	}
	else
	{	
		flag = false;
		n = 1;
		JsonRpcRequest B(writer.write(jsondata));
		V.push_back(B);
	}	
 
}

void MJsonRpcRequest::InsertJsonObj(JsonRpcRequest &obj)
{
	V.push_back(obj);
}

std::string MJsonRpcRequest::ToString()
{
		//这里不用区分flag，之前没有可以获得的数据
		std::string r("[");
		for (auto i : V)//将目标对象中的所有子对象进行遍历
		{	
			r += i.ToString()+",";
		}

		r.pop_back();
		r += "]";
		return r;
}

bool MJsonRpcRequest::IsMulti()
{
	if(flag)
		return 1;
	else
		return 0;
}

JsonRpcRequest & MJsonRpcRequest::operator[](int n)
{
 	return V[n];
}

int MJsonRpcRequest::GetSize()
{
	return n;
}

MJsonRpcResponse::MJsonRpcResponse()
{

}

MJsonRpcResponse::MJsonRpcResponse(const std::string &json)
{
	Json::Reader reader;
	Json::Value jsondata;
	Json::FastWriter writer;

	if(!reader.parse(json,jsondata))
	{
		cerr << "json parse failed" << endl;
		return;
	}

	if(jsondata.type() == Json::arrayValue)
	{
		n = jsondata.size();
		for(int i = 0;i < n; i++)
		{
			JsonRpcResponse A(writer.write(jsondata[i]));
			V.push_back(A);
		}
	}
	else
	{
		n = 1;
		JsonRpcResponse B(writer.write(jsondata));
		V.push_back(B);
	}
}

void MJsonRpcResponse::InsertJsonObj(JsonRpcResponse &obj)
{
	V.push_back(obj);
}

std::string MJsonRpcResponse::ToString()
{
	if(flag)
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
	else 
	{
		cout << "single" << endl;
		std::string r("");
		for(auto i : V)
			r = i.ToString();

		return r;
	}
	
}

void MJsonRpcResponse::SetMulti(bool a)
{
	flag = a;
}

JsonRpcResponse & MJsonRpcResponse::operator[](int n)
{
 	return V[n];
}

int MJsonRpcResponse::GetSize()
{
	return n;
}

