#include "main.h"

int Print_Sth(Json::Value &parameter,Json::Value &result);

int main()
{

	JsonRpcServer one;
	one.Register("Print_Sth", Print_Sth);

	one.StartServer(6666);
}

int Print_Sth(Json::Value &parameter, Json::Value &result)
{	
	result["result"] = parameter;
	
	return 0;
}

