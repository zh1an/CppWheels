#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else

#include <sys/socket.h>
#include <sys/time.h>

#define SOCKET int
#endif

#include "source/minHeapTimer.hpp"

std::string getLogTime() {
    char date[50] = {0};
#ifdef _WIN32
    SYSTEMTIME st = {0};
    GetLocalTime(&st);

    sprintf(date, "%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
    struct timeval tv;
    struct timezone tz;
    struct tm* tm;

    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);

    sprintf(date, "%02d:%02d:%02d.%03d", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
#endif
    return date;
}

int main() {
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif
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
