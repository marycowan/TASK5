#include "iotDataQueue.h"
#include <iostream>
using namespace std;

iotDataQueue::iotDataQueue()
{
    //ctor
}

iotDataQueue::~iotDataQueue()//abstract class iotDataQueue
{
    //dtor
}
void iotDataQueue::init()//define functions in class iotDataQueue
{
    // init initialises the queue
    rloc = sloc = 0;
}

//initialise the queue
void iotDataQueue::qput(int i)// qput puts the data into the queue until it's full
{
    if(sloc==10)
    {
        cout << "queue is full. \n";
        return;
    }
    sloc++;
    q[sloc] = i;

}

//get an integer from the queue
int iotDataQueue::qget()// qget gets the data from the queue
{
    if(rloc == sloc)
    {
        cout << "queue underflow. \n";
        return 0;
    }
    rloc++;
        return q[rloc];
}
