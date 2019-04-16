#ifndef OMA_DM_SAMPLE_JSON_RPC_H_
#define OMA_DM_SAMPLE_JSON_RPC_H_

#define NO_JSONRPC 3
#define NO_METH 4
#define NO_ID 5
#define NO_PARAMS 6

#define INVALID_INCLUDE 7
#define NO_RESULT_OR_ERROR 8
//#define NO_THIS_CODE 8
//#define NO_THIS_MESSAGE 9
//#define MISMATCH_CODE_MESSAGE 10


#include <string>
#include <vector>

#include <json/json.h>

class JsonRpcRequest {
private:
	Json::Value json_str;

public:
	JsonRpcRequest();//构造函数
    JsonRpcRequest(const std::string &json);//带参构造函数，初始化成员变量，json格式的字符串，转化为json对象类型
    JsonRpcRequest(const JsonRpcRequest &a);

    int Validate();
    std::string ToString();//

	void SetJsonRpc();
	std::string GetJsonRpc();

    void SetMethod(const std::string &method);
    std::string GetMethod();

    void SetParams(const std::string &param);
	Json::Value& GetParams();
	
	void SetId(int id);
	int GetId();
};

class JsonRpcResponse {
private:
	Json::Value json_str;

public:
    JsonRpcResponse();
    JsonRpcResponse(const std::string &json);
    JsonRpcResponse(const JsonRpcResponse &a);

    int Validate();
    std::string ToString();

    void SetJsonRpc();
    std::string GetJsonRpc();

	void SetResult(const std::string &result);
	Json::Value &GetResult();

    void SetError(const std::string &Error);
	std::string GetError();

	void SetId(int id);
	int GetId();

	void Insert(Json::Value &value);
};

class MJsonRpcRequest
{
private:
	Json::Value jsondata;
	std::vector<JsonRpcRequest> V;

	int n;
	int flag;
	//enum class type {multi, single} flag;

public:
	MJsonRpcRequest();
	MJsonRpcRequest(const std::string &json);//解析string

	void InsertJsonObj(JsonRpcRequest &obj);
	std::string ToString(); 

	JsonRpcRequest &operator[](int n);
	int GetSize();
	bool GetFlag();
};

class MJsonRpcResponse
{
private:
	Json::Value jsondata;
	std::vector<JsonRpcResponse> V;

	int n;
	int flag;

	//enum class type {multi, single} flag;
public:
	MJsonRpcResponse();
	MJsonRpcResponse(const std::string &json);//解析string

	void InsertJsonObj(JsonRpcResponse &obj);
	std::string ToString();

	JsonRpcResponse &operator[](int n);
	int GetSize();
	bool GetFlag();

};




#endif /* OMA_DM_SAMPLE_JSON_RPC_H_ */
