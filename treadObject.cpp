#include "treadObject.h"
#include <QDebug>

#include "tcp/hdr/TcpClient.h"
#include <stdlib.h>
#include <math.h>


extern QMap<QString, unsigned char *> mapHash;

TreadObject::TreadObject(QObject *parent) :
    QObject(parent),
    m_message(""),
    m_message_2("")
{

}

bool TreadObject::running() const
{
    return m_running;
}

QString TreadObject::message() const
{
    return m_message;
}

QString TreadObject::message_2() const
{
    return m_message_2;
}


void TreadObject::calculate()
{
       double * a;
      emit sendMessageProc(1,Number,a);

}


// Самый важный метод, в котором будет выполняться "полезная" работа объекта
void TreadObject::run()
{
        client();
}



void TreadObject::client() {
  using namespace std::chrono_literals;
  TcpClient client;
  const void* hellostring = "QWERTY";
  client.connectTo(LOCALHOST_IP, 8080);

  while (true)
  {
      if (arrayPoints.length()>0){
          qDebug()<<arrayPoints<<arrayPoints.length();
          std::string str = arrayPoints[0].toString().toStdString();
          const char* p= str.c_str();
          int sizeBuff = pow(2,16)-1;
          client.sendData("privet",  sizeBuff);
          DataBuffer data = client.loadData();
         // std::cout << "Client[ " << data.size << " bytes ]: " << (const char*)data.data_ptr << '\n';
          qDebug()<< "Client[ " << data.size << " bytes ]: " << (const char*)data.data_ptr << '\n';
          arrayPoints.clear();
        }
  }

  //std::this_thread::sleep_for(5s);
  client.disconnect();
 // std::clog << "Socket closed!\n";
}


void TreadObject::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}

void TreadObject::setMessage(QString message)
{
    if (m_message == message)
        return;

    m_message = message;
    emit messageChanged(message);
}

void TreadObject::setMessage_2(QString message_2)
{
    if (m_message_2 == message_2)
        return;

    m_message_2 = message_2;
    emit message_2Changed(message_2);
}
