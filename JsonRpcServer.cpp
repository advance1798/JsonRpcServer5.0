#include "JsonRpcServer.h"

/*static void *Do_Work(void *arg)
{
	int cfd = *((int *) arg);

	int retVal = 0;
	char *buff;
	char *cstr;

	buff = (char *)malloc(sizeof(char)*N);
	char *p = buff;
	
	cstr = (char *)malloc( sizeof(char)*N);
	
	retVal = recv(cfd, buff, N, 0); 
	if(-1 == retVal)
	{
		perror("recv");
		return NULL;
	}

	int n;
	while(*buff)
	{
		if (*buff == '\r' || *buff == '\n')
    	    n++;
       else
   		    n=0;
        if (n==4)//找到报头结尾的地方
        break;

        buff++;
	}
	cout << buff << endl;

	string str = "HTTP/1.1 200 OK\r\n"
			      "Server: nginx/1.14.1\r\n"
			      "Date: Mon, 01 Apr 2019 09:20:45 GMT\r\n"
				  //"Transfer-Encoding: chunked\r\n"
				  "Connection: close\r\n"
			     // application/json: 
				  //Proxy-Connection: keep-alive
				  "\r\n\r\n"
				  "{\"jsonrpc\": \"2.0\", \"result\": 19, \"id\": 1}";
	
	strcpy(cstr,str.c_str());

	write(STDOUT_FILENO, cstr, N);

	retVal = send(cfd, cstr, N, 0);

	if(-1 == retVal)
	{
		perror("send");
		return NULL;
	}


	free(cstr);
	cstr = NULL;

	buff = p;
	free(buff);
	buff = NULL;

	close(cfd);
}*/
static void *ProcessData(void *arg);

struct context_t {
	std::map<string, JsonRpcServer::func> *map;
	int fd;
};

JsonRpcServer::JsonRpcServer()
{

}

int JsonRpcServer::StartServer(int PORTNUM)
{
	struct sockaddr_in addrServer;
	struct sockaddr_in addrClient;

	int cfd;
	int sfd;

	int retVal = 0;

	socklen_t addrlen = sizeof(addrServer);

	sfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sfd)
	{
		perror("socket");
		return -1;
	}

	//端口重用
	int reuse = 1;
	int ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(int));
	if( -1 == ret)
	{
		perror("setsockopt");
		return -1;
	}

	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(PORTNUM);
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);

	
	retVal = bind(sfd, (struct sockaddr*)&addrServer, sizeof(struct sockaddr_in));
	if(-1 == retVal)
	{
		perror("bind");
		return -1;
	}
	else 
	{
		cout << "binded" << endl;
	}

	retVal = listen(sfd,100);
	if(-1 == retVal)
	{
		perror("listen");
		return -1;
	}
	else 
	{
		cout << "listening" << endl;
	}

	socklen_t addrClientlen = sizeof(addrClient);

	while(1)
	{
		pthread_t th1 = 0;

		cfd = accept(sfd, (struct sockaddr*)&addrClient, &addrClientlen);
		if(-1 == cfd)
		{
			perror("accept");
			return -1;
		}
		else
			cout << "accept successfully" << endl;

		struct context_t *context = (struct context_t *)malloc(sizeof(struct context_t));
		//context->map = JsonRpcServer::mapfunction;
		context->map = &mapfunction;
		context->fd = cfd;

		int ret = pthread_create(&th1, NULL, ProcessData, context);
		if(-1 == ret)
		{
			perror("pthread_create");

			return -1;

		}

	}

	close(sfd);
	return 0;
}

//可插入很多客户端需要实现的功能
void JsonRpcServer::Register(const std::string & method, JsonRpcServer::func f)
{
	mapfunction.insert(pair<string, func>(method, f));
	//mapfunction["Print_Sth"] = &JsonRpcServer::Print_Sth;
}

static char* RecvData(int cfd)
{
	char *buff;
	int retVal = 0;

	buff = (char *)malloc(sizeof(char)*N);

	char *p = buff;

	retVal = recv(cfd, buff, N, 0); 
	if(-1 == retVal)
	{
		perror("recv");
	}

	int n;
	while(*buff)
	{
		if (*buff == '\r' || *buff == '\n')
    	    n++;
        else
   		    n=0;

        if (n==4)//找到报头结尾的地方
        	break;

        buff++;
	}

	return buff;
}

static void SendData(int cfd, const std::string &str)
{
	int retVal = 0;
	
	retVal = send(cfd, str.c_str(), str.length(), 0);

	if(-1 == retVal)
	{
		perror("send");
	}
}

static void *ProcessData(void *arg)
{

	struct context_t *context = (struct context_t *) arg;

	Json::Value result;
	JsonRpcServer::func function;

	string serverdata = RecvData(context->fd);//recvdata函数需要把接收到的客户端的报文传出来

	cout << serverdata << endl;

	JsonRpcRequest request(serverdata);//解析报文
	JsonRpcResponse response;

	response.SetJsonRpc(request.GetJsonRpc());

	string method = request.GetMethod();

	auto iter = context->map->find(method);

	if(iter != context->map->end())
		function = iter->second;

	int errorCode = function(request.GetParams(), result);

	if( errorCode == 0)
		response.Insert(result);
	else
		response.SetResult("{\"errorCode\":0}");

	response.SetId(request.GetId());

	cout << "data prepared to be sent" << endl;
	cout << response.ToString() << endl;

	SendData(context->fd, response.ToString());//senddata需要把服务器处理过的报文返回给客户端

	close(context->fd);
}











