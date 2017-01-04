#include "sensor.h"
#include<stdlib.h>
using namespace std;
sensor::sensor()
{
    //ctor
}

sensor::~sensor()
{
    //dtor
}
int sensor::getrdg() //getrdg is a function in class sensor. It returns and int.
{
     rdg = 15+(rand()%(35-18));
    return (rdg);
}
