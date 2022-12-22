#include <stdio.h>
#include <sys/uio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
 
using namespace std;

uint32_t loading;

struct StockPid
{
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;
 
void Func_StockPid(const char *processtarget)
{
    stockthepid.pid_pipe = popen(processtarget, "r");
    fgets(stockthepid.buff, 512, stockthepid.pid_pipe);
    stockthepid.pid = strtoul(stockthepid.buff, NULL, 10);
 
    if (stockthepid.pid == 0)
    {
        printf("Jet Set Radio isn't running. \n");
        pclose(stockthepid.pid_pipe);
        exit(-1);
    }
    else
    {
        printf("Jet Set Radio is running - PID NUMBER -> {%d} \n", stockthepid.pid);
        pclose(stockthepid.pid_pipe);
    }
}
 
void ReadProcessMemory(int pid, uint32_t loading) {
	
	
    struct iovec in;
    in.iov_base = (void *) 0x98FAAC; //Loading
    in.iov_len = 1;
    struct iovec out;
    out.iov_base = &loading;
    out.iov_len = sizeof(loading);

    do {
        ssize_t nread = process_vm_readv(stockthepid.pid, &out, 1, &in, 1, 0);
        if(nread == -1) {
            printf("error!");
        } else if(nread != in.iov_len) {
            printf("error: short read of %li bytes", (ssize_t)nread);
        }
        

    } while(true);
}
 
 
void Client(int pid, uint32_t loading)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1)
	{
		cout << "Socket error!" << endl;
	}

	int port = 16834;
	string ipAddress = "192.168.1.193";

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
	
	const char* pausegametime = "pausegametime\r\n";
	const char* unpausegametime = "unpausegametime\r\n";
	const char* initgametime = "initgametime\r\n";

	if(connectRes == -1)
	{
		cout << "Couldn't connect!" << endl;
	}
	else
	{
		send(sock, initgametime, sizeof(initgametime) + 1, 0);
	}


	
	while(stockthepid.pid != 0)
	{


		if(loading == 1)
		{
			send(sock, pausegametime, sizeof(pausegametime) + 1, 0);
		}
		else if(loading == 0)
		{
			send(sock, unpausegametime, sizeof(unpausegametime) + 1, 0);
		}
	}
	
	
}
 
 
int main()
{

    Func_StockPid("pidof -s jetsetradio.exe");
    ReadProcessMemory(stockthepid.pid, loading);
	Client(stockthepid.pid, loading);
    return 0;
}
