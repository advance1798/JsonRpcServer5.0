#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <map>
#include <iterator>

#include "JsonRpc.h"

using namespace std;

#define N 10000

using namespace std;

class JsonRpcServer
{
public:
	typedef int (*func)(Json::Value &parameter, Json::Value &result);
	
	JsonRpcServer();

	void Register(const std::string & method, JsonRpcServer::func f);
	int StartServer(int PORTNUM);
	

private:
	map<string, func> mapfunction;
};