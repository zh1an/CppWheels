#include <iostream>
#include <thread>
#include <string>

#include <winsock2.h>


#include "source/minHeapTimer.hpp"

std::string getLogTime() {
    SYSTEMTIME st = {0};
    GetLocalTime(&st);
    char date[50] = {0};
    sprintf(date, "%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return date;
}

int main() {
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);

    SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);

    minHeapTimer min_heap;

    min_heap.timerAdd(1000, [=] { std::cout << getLogTime() << ": this is callback_1" << std::endl; });
    min_heap.timerAdd(5000, [=] { std::cout << getLogTime() << ": this is callback_2" << std::endl; });
    struct timeval el;
    while (1) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(_socket, &read_set);
        el = min_heap.getTopElapsedTimeval();

        select(0, &read_set, NULL, NULL, &(el));
        min_heap.timerProcess();
    }

    return 0;
}
