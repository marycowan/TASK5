#include <iostream>
#include <unistd.h>
#include "sensor.h"
#include "iotDataQueue.h"
#include <stdlib.h>
#include <winsock.h>//dont forget to add wsock32.lib to linker dependencies
#include <sstream>
#include <string>
using namespace std;
#define BUFFERSIZE 1024
void die_with_error(char *errorMessage);
void die_with_wserror(char *errorMessage);
string IntToString(int);
int newTemparr[10];//I really tried not to do this, but couldn't figure out how to pass the ref to the array
// to proc, for eg. because of proc(this)- I couldn't pass array in and didn't need it there anyway so gave up:(
class Machine   //class machine whose behaviour depends on state,
{
    //so make state a nested class in Machine..
    class State *current;//machine has a pointer (*current) which points to state
public:
    Machine();//functions in machine
    void setCurrent(State *s)// a pointer to state is passed into setCurrent
    {
        current = s;
    }
    void sensing();//functions in machine
    void idle();
    void proc();
};

class State
{
public:
    virtual void sensing(Machine *m)//virtual functions in class state
    {
        cout << "   machine SENSING\n";
    }
    virtual void idle(Machine *m)
    {
        cout << "   machine IDLE\n";
    }
    // virtual void proc (Machine *m, int (*newTemp)[10])//trying to pass array to function,no joy
    virtual void proc (Machine *m)
    {
        cout << "  machine Processing\n";
    }
};

void Machine::sensing()//sensing is a function in class machine
{
    current->sensing(this);//current is a pointer to the current state of the machine
}

void Machine::idle()//idle is a function in class machine
{
    current->idle(this);//asking idle function of current state to run a
    // function associated with itself CALLBACK****
}
void Machine::proc()//proc is a function in class machine
{
    current->proc(this);//ASK PROC TO RUN A FUNCTION ASSOC WITH ITSELF.
}
class PROCE: public State//Class PROC is a CHILD of state, inherits from state
{
public:
    PROCE()// I had to change PROC to PROCE because of name conflict in a std library?!
    {
        cout << "   PROCE-ctor ";
    };
    ~PROCE()
    {
        cout << "   dtor-PROCE\n";
    };
    void idle(Machine *m);

};

class SENSING: public State
{
public:
    SENSING()
    {
        cout << "   SENSING-ctor ";
    };
    ~SENSING()
    {
        cout << "   dtor-SENSING\n";
    };
    void proc(Machine *m);
};
void SENSING::proc(Machine *m)
{
    int latestTemp;
    usleep(50000);
    iotDataQueue temperature;//create q object
    sensor tempSens;// sensor object
    temperature.init();//init temp q
    for(int num=0; num<10; num++)//loop to get 10 readings from sensors and put into queue.
    {
        latestTemp = (tempSens.getrdg()*num/5.0);// get reading from temp sensor and assign to latestTemp
        temperature.qput(latestTemp);//put latestTemp into  Q
        cout << "latest temp to be read is: " << latestTemp;
        cout << '\n';
        newTemparr[num]=temperature.qget();
        cout << newTemparr[num] << " degrees C is value stored in array\n";
    }
    cout << "   going from SENSING to processing";
    usleep(50000);
    m->setCurrent(new PROCE());
    delete this;
}

class IDLE: public State
{
public:
    IDLE()
    {
        cout << "   IDLE-ctor ";
    };
    ~IDLE()
    {
        cout << "   dtor-IDLE\n";
    };
    void sensing(Machine *m)
    {
        // int go;
        usleep(5000000);
//           cout << " press 1 to proceed from idle to sensing:";// User must press a button to move from idle to sensing.
//          while (go !=1)
//           cin >> go;// could use a timer either
        cout << "   going from IDLE to SENSING";
        m->setCurrent(new SENSING());
        delete this;
    }
};
void PROCE::idle(Machine *m)
{

    usleep(50000);
    cout << "Processed Contents of Temperature queue: \n";
    for(int num=0; num<10; num++)//loop to cout 10 times
    {
        newTemparr[num]= (newTemparr[num]*9/5+32);// processing is CONVERTING TO fAHRENH.
        cout << newTemparr[num]<< " degrees Fahrenheit\n";
    }


    cout << "   going from PROCessing to IDLE";
    usleep(50000);
    m->setCurrent(new IDLE());//I wanted to process data here but I can't get the array info in here to this function
    delete this;            // I need to pass it by reference but can't do it ;( too complicated with all the states.
}                           // will keep trying.if get time...

Machine::Machine()
{
    current = new IDLE(); // all new machines created in IDLE state
    cout << '\n';
}
int main()
{
    string request;
    string response;
    int resp_leng;

    char buffer[BUFFERSIZE];
    struct sockaddr_in serveraddr;
    int sock;
    string sensorData= "<reading><time>11.00</time><temperature>46</temperature></reading>";//left in to get strlength
    WSADATA wsaData;
    char *ipaddress = "127.0.0.1";
    int port = 8080;
    string sensorDataLengthStr = IntToString(sensorData.length());

    void(Machine:: *ptrs[])() =//an array of pointers to functions in class Machine
    {
        //   [0] means idle,[1] means sensing etc.
        Machine::idle, Machine::sensing, Machine::proc//these are the functions in machine that can be pointed at
    };
    Machine fsm;//instantiate an object of class machine
    cout << "FSM created in IDLE state: [0]";
    usleep(50000);

    int num  ;
    for(num=1; num<4; num++)//1 cycle of idle,sensing,processing before request to POST
    {

        (fsm.*ptrs[num%3])();

    }

    request+="POST /sendreading HTTP/1.0\r\n"; // request+="GET /test.html HTTP/1.0\r\n";
    request+="Host: localhost\r\n";
    request+="Content-Length: " + sensorDataLengthStr + "\r\n";
    request+="Accept-Charset: utf-8\r\n";
    request+="\r\n";
    int count =0;
    while(count<10)
    {
        string sensorData = IntToString(newTemparr[count])+",";
        cout << "sensordata is " << sensorData<< "....converted to degrees Fahrenheit" << endl;

        //init winsock
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            die_with_wserror("WSAStartup() failed");

        //open socket
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            die_with_wserror("socket() failed");

        //connect
        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family      = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(ipaddress);
        serveraddr.sin_port        = htons((unsigned short) port);
        if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
            die_with_wserror("connect() failed");

        //send request
        if (send(sock, request.c_str(), request.length(), 0) != request.length())
            die_with_wserror("send() sent a different number of bytes than expected");
        //Send Sensor Reading
        if (send(sock, sensorData.c_str(), sensorData.length(), 0) != sensorData.length())
            die_with_wserror("send() sent a different number of bytes than expected");

        //get response
        response = "";
        resp_leng= BUFFERSIZE;
        while (resp_leng == BUFFERSIZE)
        {
            resp_leng= recv(sock, (char*)&buffer, BUFFERSIZE, 0);
            if (resp_leng>0)
                response+= string(buffer).substr(0,resp_leng);
            //note: download lag is not handled in this code
        }

        //display response
        cout <<"The response is: "<< response << endl;

        //disconnect
        closesocket(sock);

        //cleanup
        WSACleanup();
        cout << "going to sleep\n";
        Sleep(3000);
        cout << "waking up!!\n";
        count++;
    }
    return 0;

}
void die_with_error(char *errorMessage)
{
    cerr << errorMessage << endl;
    exit(1);
}

void die_with_wserror(char *errorMessage)
{
    cerr << errorMessage << ": " << WSAGetLastError() << endl;
    exit(1);
}

string IntToString (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}


