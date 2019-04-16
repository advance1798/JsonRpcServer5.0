#include "JsonRpcServer.h"
#include <sstream>

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

//可插入功能
void JsonRpcServer::Register(const std::string & method, JsonRpcServer::func f)
{
	mapfunction.insert(pair<string, func>(method, f));
}

static char* RecvData(int cfd)
{
	char *buff;
	int retVal = 0;

	buff = (char *)malloc(sizeof(char)*N);

	string header("");

	while (header.find("\r\n\r\n") == header.npos) {//find返回一个索引值（int型），字符串所在的位置
		cout << "!!!" << endl;
		retVal = recv(cfd, buff, N, 0);
		if(-1 == retVal)
		{
			perror("recv");
		}
		
	//	cout << buff << endl;
		header.append(buff);
	}//接收一个完整的头

	//cout << header.c_str() << endl;

	char substr[] = "Content-Length: ";
	const char *res = strstr(header.c_str(), substr);
	
	string txt = string(res);
	size_t a = txt.find_first_of(':');
	size_t b = txt.find_first_of('\r');

	string scontentLength = txt.substr(a+1, b-a-1);

	int contentLength;
	stringstream ss;
	ss << scontentLength;
	ss >> contentLength;
	
	cout << contentLength << endl;

	
	int length = header.length() - header.find("\r\n\r\n") - 4;//find返回一个索引值（int型），字符串所在的位置

	char *message = (char *)malloc(sizeof(char) * contentLength);

	const char *Header = header.c_str();
	int n;

	while(*Header)
	{
		if (*Header == '\r' || *Header == '\n')
    	    n++;
        else
   		    n=0;

        if (n==4)//找到报头结尾的地方
        {	
        	Header++;
        	break;
        }

        Header++;
	}
	
	memcpy(message, Header, length);//将接收完整的头文件中可能携带的数据部分赋值给message

	while (length < contentLength) {
		cout << "###" << endl;
		//length += recv (cfd, message + length, contentLength - length, 0);
		retVal = recv (cfd, message + length, contentLength - length, 0);
		if (-1 == retVal)
		{
			perror("recv");
		}
		else 
			length += retVal;
	}

	//cout << message << endl;
	return message;
}

static int SendData(int cfd, const std::string &str)
{
	int retVal = 0;
	
	retVal = send(cfd, str.c_str(), str.length(), 0);

	if(-1 == retVal)
	{
		perror("send");
		return -1;
	}

	return 0;
}

static void *ProcessData(void *arg)
{

	struct context_t *context = (struct context_t *) arg;

	Json::Value result;
	JsonRpcServer::func function;

	string serverdata = RecvData(context->fd);//recvdata函数需要把接收到的客户端的报文传出来

	//cout << serverdata << endl;

	MJsonRpcRequest multiRequests(serverdata);

	//int flag = multiRequests.GetFlag();

	if(multiRequests.GetFlag())
	{
		MJsonRpcResponse multiResponses;

		for (int i = 0; i < multiRequests.GetSize(); ++i) 
		{
			JsonRpcResponse response;

			response.SetJsonRpc();

			response.SetId(multiRequests[i].GetId());

			string method = multiRequests[i].GetMethod();
	
			auto f = context->map->find(method);

			if(f != context->map->end())
			function = f->second;
			int errorCode = function(multiRequests[i].GetParams(),result);

			if(errorCode == 0)
				response.Insert(result);
			else
				response.SetResult("{\"errorCode\":0}");

			multiResponses.InsertJsonObj(response);
		}

		cout << "------------------------------" << endl;
		cout << "data prepared to be sent" << endl;
		cout << multiResponses.ToString() << endl;

		SendData(context->fd, multiResponses.ToString());//senddata需要把服务器处理过的报文返回给客户端
	}

	else
	{
		JsonRpcRequest request(serverdata);//解析报文
		JsonRpcResponse response;

		response.SetJsonRpc();

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
	}

		close(context->fd);
}











