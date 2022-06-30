#include "sensthread.h"

SensThread::SensThread()
{

}

SensThread::~SensThread()
{

}

void SensThread::getmessage()
{
    std::cout << awl::Core::byteArrayToString(tba) << std::endl;
}
void SensThread::onstart()
{

}
void SensThread::onstop()
{

}
