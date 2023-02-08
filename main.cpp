#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/uio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>

using namespace std;

string ipAddress = "";

uint32_t loadingCondition; //isloadingCondition
uint32_t startCondition; //start
uint32_t splitCondition; //split

struct StockPid {
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

void Func_StockPid(const char *processtarget) {
    stockthepid.pid_pipe = popen(processtarget, "r");
    if (!fgets(stockthepid.buff, 512, stockthepid.pid_pipe)) {
        cout << "Error reading process ID: " << strerror(errno) << endl;
    }

    stockthepid.pid = strtoul(stockthepid.buff, nullptr, 10);

    if (stockthepid.pid == 0) {
        cout << "Jet Set Radio isn't running.\n";
        pclose(stockthepid.pid_pipe);
    } else {
        cout << "Jet Set Radio is running - PID NUMBER -> " << stockthepid.pid << endl;
        pclose(stockthepid.pid_pipe);
    }
}

void ReadProcessMemory(int pid, uint32_t &loadingCondition, uint32_t &startCondition, uint32_t &splitCondition) {
    //loadingCondition
    struct iovec loadingConditionLocal, loadingConditionRemote;
    loadingConditionLocal.iov_base = &loadingCondition;
    loadingConditionLocal.iov_len = sizeof(loadingCondition);
    loadingConditionRemote.iov_base = (void *) 0x98FAAC;
    loadingConditionRemote.iov_len = sizeof(loadingCondition);

    ssize_t loadingConditionnread = process_vm_readv(pid, &loadingConditionLocal, 1, &loadingConditionRemote, 1, 0);
    if(loadingConditionnread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(loadingConditionnread != loadingConditionRemote.iov_len) {
        cout << "Error reading process memory: short read of " << loadingConditionnread << " bytes" << endl;
        exit(-1);
    }

    //startCondition
    struct iovec startConditionLocal, startConditionRemote;
    startConditionLocal.iov_base = &startCondition;
    startConditionLocal.iov_len = sizeof(startCondition);\
    startConditionRemote.iov_base = (void *) 0xB5A278;
    startConditionRemote.iov_len = sizeof(startCondition);

    ssize_t startConditionnread = process_vm_readv(pid, &startConditionLocal, 1, &startConditionRemote, 1, 0);
    if(startConditionnread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(startConditionnread != startConditionRemote.iov_len) {
        cout << "Error reading process memory: short read of " << startConditionnread << " bytes" << endl;
        exit(-1);
    }

    //splitCondition
    struct iovec splitConditionLocal, splitConditionRemote;
    splitConditionLocal.iov_base = &splitCondition;
    splitConditionLocal.iov_len = sizeof(splitCondition);\
    splitConditionRemote.iov_base = (void *) 0x98FB1C;
    splitConditionRemote.iov_len = sizeof(splitCondition);

    ssize_t splitConditionnread = process_vm_readv(pid, &splitConditionLocal, 1, &splitConditionRemote, 1, 0);
    if(splitConditionnread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(splitConditionnread != splitConditionRemote.iov_len) {
        cout << "Error reading process memory: short read of " << splitConditionnread << " bytes" << endl;
        exit(-1);
    }
}

void Client(int pid, string ipAddress, uint32_t& loadingCondition, uint32_t& startCondition, uint32_t& splitCondition) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cout << "Socket error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    int port = 16834;

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) != 1) {
        cout << "Invalid IP address" << endl;
        exit(EXIT_FAILURE);
    }

    int connectRes = connect(sock, (struct sockaddr*) &hint, sizeof(hint));
    if (connectRes == -1) {
        cout << "Couldn't connect: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    const char* pausegametime = "pausegametime\r\n";
    const char* unpausegametime = "unpausegametime\r\n";
    const char* initgametime = "initgametime\r\n";
    const char* starttimer = "starttimer\r\n";
    const char* split = "split\r\n";

    int sendRes = send(sock, initgametime, strlen(initgametime), 0);
    if (sendRes == -1) {
        cout << "Couldn't send initgametime: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t prevloadingCondition = 0;
    int pause_res = 0;
    int unpause_res = 0;

    uint32_t prevstartCondition = 0;
    int startTimer_res = 0;

    uint32_t prevsplitCondition = 0;
    int split_res = 0;
    
    while (true) {
        ReadProcessMemory(pid, loadingCondition, startCondition, splitCondition);

        if (loadingCondition == 1 && prevloadingCondition != 1) {
            pause_res = send(sock, pausegametime, strlen(pausegametime), 0);
        } else if (loadingCondition == 0 && prevloadingCondition != 0) {
            unpause_res = send(sock, unpausegametime, strlen(unpausegametime), 0);
        }
        prevloadingCondition = loadingCondition;

        if(startCondition != 1 && prevstartCondition == 1) {
            startTimer_res = send(sock, starttimer, strlen(starttimer), 0);
        }
        prevstartCondition = startCondition;

        if(splitCondition == 1 && prevsplitCondition != 1) {
            split_res = send(sock, split, strlen(split), 0);
        }
        prevsplitCondition = splitCondition;
    }
}


int main(int argc, char *argv[]) {

    cout << "What is your local IP address (LiveSplit Server settings will tell you if you don't know.)\n" << endl;
    cin >> ipAddress;

    const char *processName = "pidof jetsetradio.exe";
    while (true) {
        Func_StockPid(processName);
        if (stockthepid.pid == 0) {
            cout << "Jet Set Radio isn't running. Retrying in 5 seconds...\n";
            sleep(5);
            system("clear");
        } else {
            break;
        }
    }
    uint32_t loadingCondition = 0;
    uint32_t startCondition = 0;
    uint32_t splitCondition = 0;
    Client(stockthepid.pid, ipAddress, loadingCondition, startCondition, splitCondition);
    return 0;
}
