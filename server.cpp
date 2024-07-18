#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Ws2tcpip.lib")
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
// #include <filesystem>

class Server {
    private:
        int port;
        char *ip;
        char buffer[1024];
    public:
        WSADATA wsaData;
        SOCKADDR_IN serverAddr, clientAddr;
        SOCKET serverSocket, clientSocket;
        int clientAddrSize = sizeof(clientAddr);
        Server(int x, char *y);
        void initLibs();
        void start();
        void connectClient();
        int getfile(std::string filename);
        int sendfile(std::string filename);
        void stop();
};

Server::Server(int x,char *y){ port = x; ip = y;};

void Server::initLibs(){
    WSAStartup(MAKEWORD(2, 2), &wsaData);};

void Server::start(){
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if(bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr))){
    std::cout << "error create server: " << WSAGetLastError() << std::endl;};
    setsockopt(serverSocket,SOL_SOCKET,SO_SNDBUF,(char *)&buffer,sizeof(buffer));
    setsockopt(serverSocket,SOL_SOCKET,SO_RCVBUF,(char *)&buffer,sizeof(buffer));
    listen(serverSocket, 1);};

void Server::connectClient(){
    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
    std::cout << "try Client connect" << std::endl;};

void Server::stop(){
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();};

int Server::getfile(std::string filename){

    std::ofstream file(filename, std::ios::binary);
        int byteRead;
        
        do{
        byteRead = recv(clientSocket, buffer, 1024, 0);
        if (byteRead > 0){
            file.write(buffer, byteRead);}; 
        if (byteRead == 0){
            std::cout << "error get file" << std::endl;
            return -1;};
    }while(byteRead > 0);
    file.close();
    std::cout << "get file succsess"<< std::endl;
    return 0;
};

int Server::sendfile(std::string filename){
 //to bin
    std::ifstream file(filename, std::ios::binary);
    if (!file){std::cout << "fail to open file" << std::endl; return -1;};
    int bytesRead;
    do {
        file.read(buffer, 1024);
        bytesRead = file.gcount();
 //if (bytesRead < 0)
        if (bytesRead > 0){
        send(clientSocket, buffer, bytesRead, 0);
         }; 
    if (bytesRead < 1024){
    if(file.eof()){std::cout << "200 : file send successfuly" << std::endl;
     }else{
        std::cout << "error read file" << std::endl;
        return -1;};};}while(!file.eof());
    file.close();
    return 0;
};

int fileStatus(std::string filename){
    std::ifstream file(filename.c_str());
    if (!file){
        file.close();
        return -1;
    }else{
        std::string line;
        std::getline(file, line);
        if (line == ""){ 
            file.close();
            return -1;
        }else{
            file.close();
            return 0;};
        }
    file.close();
    return -1;
};
int main(int argc,  char **args)
{
    int prt = std::stoi(args[1]);
    Server server(prt, args[2]);
    std::cout << "port: " <<args[1]<< " ip: "<< args[2] << std::endl; 
    server.initLibs();
    server.start();
    server.connectClient();
    int counter = 0;
    std::string command;
    unsigned int microsecond = 1000000;
    std::ofstream file("status.bin");
    file << "No data!";
    file.close();
        while(true){
        usleep(2 * microsecond);
        if (fileStatus("command.bin") != 0){continue;};
        server.sendfile("command.bin");
        std::system("del command.bin");
        server.stop();
        server.initLibs();
        server.start();
        server.connectClient();
        std::ofstream file1("status.bin");
        file1 << "No data!";
        file1.close();
        server.getfile("file.bin");
        std::ofstream file2("status.bin");
        file2 << "read";
        file2.close();
        server.stop();
        server.initLibs();
        server.start();
        server.connectClient();
        counter +=1;
    }
    std::cout << "end working server" << std::endl;
    server.stop();
    std::system("pause");
}
