#include <iostream>
#include <thread>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>

#include <tchar>
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>

#define SOCKET int

#endif

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stream.h"

using namespace rapidjson;


std::string getCPName() {
#ifdef _WIN32
    const int MAX_BUFFER_LEN = 500;
	char  szBuffer[MAX_BUFFER_LEN];
	DWORD dwNameLen;

	dwNameLen = MAX_BUFFER_LEN;
    GetComputerName((LPWSTR)szBuffer, &dwNameLen);
    return string(szBuffer);
    //dwNameLen = MAX_BUFFER_LEN;
    //GetUserName((LPWSTR)szBuffer, &dwNameLen);
#endif
    return "Linux";
}

std::string getUserName() {
#ifdef _WIN32
    const int MAX_BUFFER_LEN = 500;
	char  szBuffer[MAX_BUFFER_LEN];
	DWORD dwNameLen;

	//dwNameLen = MAX_BUFFER_LEN;
    //GetComputerName((LPWSTR)szBuffer, &dwNameLen);
    //return string(szBuffer);
    dwNameLen = MAX_BUFFER_LEN;
    GetUserName((LPWSTR)szBuffer, &dwNameLen);

    return string(szBuffer);
#endif
    return "Linux root";
}

std::string getJsonString() {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("computer");
    writer.String(getCPName().c_str());
    writer.Key("User");
    writer.String(getUserName().c_str());
    writer.Key("count");
    writer.Int64(1);
    writer.EndObject();

    const char* output = buffer.GetString();
    return std::string(output);
}

std::string getSendJson(std::string& str, unsigned long long count ) {
    rapidjson::Document doc;
    doc.Parse(str.c_str());

    doc["count"] = count;

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    doc.Accept(writer);
    return buffer.GetString();
}

int main() {
#ifdef _WIN32
    WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

    SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(23333);
    server.sin_addr.s_addr = inet_addr("47.240.69.128");

    connect(_socket, (sockaddr*)&server, sizeof(server));

    static unsigned long long count = 1;

    auto json = getJsonString();

    while (1) {
        json = getSendJson(json, count++);
        send(_socket, json.c_str(), json.size(),0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (count >= 0xFFFFFFFFFFFFFFFE)
            break;
    }

    close(_socket);

    return 0;
}