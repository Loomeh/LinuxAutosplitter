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

uint32_t loading;

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

void ReadProcessMemory(int pid, uint32_t &loading) {
    struct iovec local, remote;
    local.iov_base = &loading;
    local.iov_len = sizeof(loading);
    remote.iov_base = (void *) 0x98FAAC;
    remote.iov_len = sizeof(loading);

    ssize_t nread = process_vm_readv(pid, &local, 1, &remote, 1, 0);
    if(nread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(nread != remote.iov_len) {
        cout << "Error reading process memory: short read of " << nread << " bytes" << endl;
        exit(-1);
    }
}

void Client(int pid, uint32_t& loading) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cout << "Socket error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    int port = 16834;
    string ipAddress = "192.168.1.193";

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

    int sendRes = send(sock, initgametime, strlen(initgametime), 0);
    if (sendRes == -1) {
        cout << "Couldn't send initgametime: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t prevLoading = 0;
    int pause_res = 0;
    int unpause_res = 0;
    while (true) {
        ReadProcessMemory(pid, loading);
        if (loading == 1 && prevLoading != 1) {
            pause_res = send(sock, pausegametime, strlen(pausegametime), 0);
        } else if (loading == 0 && prevLoading != 0) {
            unpause_res = send(sock, unpausegametime, strlen(unpausegametime), 0);
        }
        prevLoading = loading;

        // Check if jet set radio is running every 5 seconds and if not, start over
        if (pause_res == -1 || unpause_res == -1) {
            close(sock);
            cout << "Error sending message, retrying in 5 seconds\n";
            sleep(5);
            return Client(pid, loading);
        }
    }
}


int main(int argc, char *argv[]) {
    const char *processName = "pidof jetsetradio.exe";
    while (true) {
        Func_StockPid(processName);
        if (stockthepid.pid == 0) {
            cout << "Jet Set Radio isn't running. Retrying in 5 seconds...\n";
            sleep(5);
        } else {
            break;
        }
    }
    uint32_t loading = 0;
    Client(stockthepid.pid, loading);
    return 0;
}
