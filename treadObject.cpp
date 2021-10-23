#include "treadObject.h"
#include <QDebug>
#include <QThread>

#include "tcp/hdr/TcpClient.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
typedef unsigned char uchar;

#define TRANSFER_BLOCK 1024
/**
 * Convert human readable IPv4 address to UINT32
 * @param pDottedQuad   Input C string e.g. "192.168.0.1"
 * @param pIpAddr       Output IP address as UINT32
 * return 1 on success, else 0
 */
int ipStringToNumber (const char*       pDottedQuad,
                              unsigned int *    pIpAddr)
{
   unsigned int            byte3;
   unsigned int            byte2;
   unsigned int            byte1;
   unsigned int            byte0;
   char              dummyString[2];

   /* The dummy string with specifier %1s searches for a non-whitespace char
    * after the last number. If it is found, the result of sscanf will be 5
    * instead of 4, indicating an erroneous format of the ip-address.
    */
   if (sscanf (pDottedQuad, "%u.%u.%u.%u%1s",
                  &byte3, &byte2, &byte1, &byte0, dummyString) == 4)
   {
      if (    (byte3 < 256)
           && (byte2 < 256)
           && (byte1 < 256)
           && (byte0 < 256)
         )
      {
         *pIpAddr  =   (byte0 << 24)
                     + (byte1 << 16)
                     + (byte2 << 8)
                     +  byte3;

         return 1;
      }
   }

   return 0;
}

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

float bytesToFloat(uchar b0, uchar b1, uchar b2, uchar b3)
{
    float output;

    *((uchar*)(&output) + 3) = b0;
    *((uchar*)(&output) + 2) = b1;
    *((uchar*)(&output) + 1) = b2;
    *((uchar*)(&output) + 0) = b3;

    return output;
}



uint8_t * float2Bytes(float val){
  // Create union of shared memory space
    uint8_t      bytes[sizeof(float)];
    *(float*)(bytes)  = val;
    return bytes;
}


// Самый важный метод, в котором будет выполняться "полезная" работа объекта
void TreadObject::run()
{
        client();
}

//g++-10.1  -Wall -o "%e" "%f"  -I/home/pi/TcpServer-master/tcp/hdr -L/home/pi/TcpServer-master -o main -ltcp -lpthread -std=c++17
void TreadObject::client() {
  using namespace std::chrono_literals;
  TcpClient client;
  uint32_t HOST_IP;
  std::string str = IP.toStdString();
  const char* p = str.c_str();
  ipStringToNumber(p,&HOST_IP);
//  if (inet_ntop(AF_INET, &HOST_IP, "169.254.183.65", INET6_ADDRSTRLEN) == NULL) {
//      perror("inet_ntop");
//      exit(EXIT_FAILURE);
//  }

  qDebug()<<HOST_IP<<port.toInt();


  TcpClient::status status;
  QString my_qstring  = "Hello server";
  std::string str_ = my_qstring.toStdString();
  const char* tmp = str_.c_str();
  status = client.connectTo(HOST_IP, port.toInt());
  /*connected = 0,
  err_socket_init = 1,
  err_socket_bind = 2,
  err_socket_connect = 3,
  disconnected = 4*/
  if (int(status))
  {
      flagTCPIsActive = false;
  }
  emit sendMessageErrorFromServer();


  byte* bytes_array;
  uint8_t      bytes[sizeof(float)];
  float tmpValue;
  while (flagTCPIsActive)
  {
      status = client.getStatus();
      if (int(status))
      {
          flagTCPIsActive = false;
          emit sendMessageErrorFromServer();
      }
      if (flagReady){
          int cntr=0;
          int sizeOfTransferBuffer = arrayPoints.length()*sizeof(float);
          if ((sizeOfTransferBuffer%TRANSFER_BLOCK)!=0)
              sizeOfTransferBuffer = (int(sizeOfTransferBuffer/TRANSFER_BLOCK)+1)*TRANSFER_BLOCK;
          byte * arrayToTransfer = new byte [sizeOfTransferBuffer];
          for (int j=0;j<arrayPoints.length();j++)
          {
              *(float*)(bytes) = arrayPoints[j].toFloat();  // convert float to bytes
              for (int k=0;k<4;k++)
                  arrayToTransfer[j*sizeof(float)+k]=bytes[k];
          }
          //qDebug()<<"sizeOfTransferBuffer"<<sizeOfTransferBuffer<<"arrayPoints.length()"<<arrayPoints.length();
          arrayPoints.clear();
          cntr = ((sizeOfTransferBuffer%TRANSFER_BLOCK)!=0)?(int(sizeOfTransferBuffer/TRANSFER_BLOCK)+1):(int(sizeOfTransferBuffer/TRANSFER_BLOCK));

          //client.sendData(arrayToTransfer,  sizeBuff);
          for (int i=0;i<cntr;i++)
          {
              my_qstring  = QString::number(i);
              str_ = my_qstring.toStdString();
              tmp = str_.c_str();
              client.sendData(arrayToTransfer +TRANSFER_BLOCK*i, TRANSFER_BLOCK);
          }

          DataBuffer data = client.loadData();
         // std::cout << "Client[ " << data.size << " bytes ]: " << (const char*)data.data_ptr << '\n';
          //qDebug()<< "Client[ " << data.size << " bytes ]: " << (const char*)data.data_ptr << '\n';
          stringFromServer = (const char*)data.data_ptr;
          emit sendMessageFromServer();
          flagReady = false;
          emit endOfSend();
        }
  }

  //std::this_thread::sleep_for(5s);
  client.disconnect();
  emit sendMessageErrorFromServer();

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
