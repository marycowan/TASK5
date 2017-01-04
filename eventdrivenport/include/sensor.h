#ifndef SENSOR_H
#define SENSOR_H


class sensor
{
public:
    sensor();
    virtual ~sensor();
    int getrdg();   //declare function getrdg which returns and integer
protected:      // the integer will be put into Q by qput in iotDataQ

private:
    int rdg;// declare int variable reading
};

#endif // SENSOR_H
