#ifndef OMA_DM_SAMPLE_JSON_RPC_H_
#define OMA_DM_SAMPLE_JSON_RPC_H_

#define PARSE_FAILED 2
#define NO_JSONRPC 3
#define NO_METH 4
#define NO_ID 5
#define NO_PARAMS 6
#define INVALID_PARAMS 7

#define INVALID_INCLUDE 8
#define NO_RESULT_OR_ERROR 9
#define INVALID_ERROR 10
#define GET_RESULT 11
#define GET_ERROR 12
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
    bool IsNotify();
   
    std::string ToString();//

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
    bool IsNotify();
  
    std::string ToString();

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
	std::vector<JsonRpcRequest> V;
	int n;
	bool flag;

public:
	MJsonRpcRequest();
	MJsonRpcRequest(const std::string &json);//解析string

	void InsertJsonObj(JsonRpcRequest &obj);
	std::string ToString(); 
	bool IsMulti();

	JsonRpcRequest &operator[](int n);
	int GetSize();
};

class MJsonRpcResponse
{
private:
	std::vector<JsonRpcResponse> V;
	int n;
	bool flag;

public:
	MJsonRpcResponse();
	MJsonRpcResponse(const std::string &json);//解析string

	void InsertJsonObj(JsonRpcResponse &obj);
	std::string ToString();

	void SetMulti(bool a);

	JsonRpcResponse &operator[](int n);
	int GetSize();
};




#endif /* OMA_DM_SAMPLE_JSON_RPC_H_ */
