#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <fstream>
using namespace std;

void *join(void *);

static int connFd;

int main(int argc, char* argv[])
{
    string FILES_LOCATION = "/home/cecs326/Desktop/CECS327/MultithreadSocket/Stored";

    int pId, portNo, listenFd;
    socklen_t len; //store size of the address
    bool loop = false;
    struct sockaddr_in svrAdd, clntAdd;
    
    pthread_t threadA[3];
    
    if (argc < 2)
    {
        cerr << "Syntam : ./server <port>" << endl;
        return 0;
    }
    
    portNo = atoi(argv[1]);
    
    if((portNo > 65535) || (portNo < 2000))
    {
        cerr << "Please enter a port number between 2000 - 65535" << endl;
        return 0;
    }
    
    //create socket
    //listenFd = socket(AF_INET, SOCK_STREAM, 0);
    listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    

    if(listenFd < 0)
    {
        cerr << "Cannot open socket" << endl;
        return 0;
    }
    
    bzero((char*) &svrAdd, sizeof(svrAdd));
    
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(portNo);
    
    //bind socket
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
        cerr << "Cannot bind" << endl;
        return 0;
    }
    
    listen(listenFd, 5);
    
    len = sizeof(clntAdd);
    
    int noThread = 0;

    while (noThread < 3)
    {
        
        cout << "Listening" << endl;

        //this is where client connects. svr will hang in this mode until client conn
        connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

        if (connFd < 0)
        {
            cerr << "Cannot accept connection" << endl;
            return 0;
        }
        else
        {
            cout << "Connection successful" << endl;
        }
        
        pthread_create(&threadA[noThread], NULL, join, NULL); 
        
        noThread++;
    }
    
    for(int i = 0; i < 3; i++)
    {
        pthread_join(threadA[i], NULL);
    }
    
    
}

void *join (void *dummyPt)
{
    cout << "Thread No: " << pthread_self() << endl;
    char test[300];
    bzero(test, 300);
    bool loop = false;
    int counter = 200;
    while(counter > 0)
    {    
        bzero(test, 300);
        
        
        read(connFd, test, 300);
        
        string tester (test);
        cout << tester << endl;
        
    }
    cout << "\nClosing thread and conn" << endl;
    close(connFd);
}
void *publish (void *dummyPt)
{
    cout << "Thread No: " << pthread_self() << endl;
    char test[300];
    bzero(test, 300);
    bool loop = false;
    while(!loop)
    {    
        bzero(test, 300);
        
        
        read(connFd, test, 300);
        
        string tester (test);
        cout << tester << endl;
        
        
        if(tester == "exit")
            break;
    }
    cout << "\nClosing thread and conn" << endl;
    close(connFd);
}
void *search (void *dummyPt)
{
    cout << "Thread No: " << pthread_self() << endl;
    char filename[300];
    bzero(filename, 300);
        
    read(connFd, filename, 300);
        
    string filenameString (filename);
    
    cout << filenameString << endl;
    ofstream file (filenameString);
    if(file.is_open())
    {
	cout<< filenameString << " exists"<<endl;
	cout<< "It exists at: " << FILES_LOCATION<<endl;

    }
    else 
    {
	cout<<"File not found"<<endl;
    }
}
