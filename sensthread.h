#ifndef SENSTHREAD_H
#define SENSTHREAD_H

#include "net.h"

class SensThread : public  awl::Net::TcpServerThread
{
public:
    SensThread();
    ~SensThread();

protected:
    void getmessage();
    void onstart();
    void onstop();
};

#endif // SENSTHREAD_H
