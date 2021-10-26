#include "treadObject.h"
#include <QDebug>
#include <QThread>

#include "tcp/hdr/TcpClient.h"
#include "tcp/hdr/TcpServer.h"
#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <QFile>
#include <QDate>
#include <QTime>
#include <QDir>

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

//Parse ip to std::string
std::string getHostStr(const TcpServer::Client& client) {
    uint32_t ip = client.getHost ();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
            std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
            std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
            std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
            std::to_string( client.getPort ());
}



// Самый важный метод, в котором будет выполняться "полезная" работа объекта
void TreadObject::run()
{   
    if (!isServer)
    {
    client();
    }
    else
    {
        if (!flagInitServer)
        {
            TcpServer server( 4002,

            [](DataBuffer data, TcpServer::Client& client){ // Data handler
              std::cout << "Client "<<getHostStr(client)<<" send data [ " << data.size << " bytes ]: " << (char*)data.data_ptr << '\n';
//              client.sendData("Hello, client!", sizeof("Hello, client!"));
            },

            [](TcpServer::Client& client) { // Connect handler
              //std::cout << "Client " << getHostStr(client) << " connected\n";
            },


            [](TcpServer::Client& client) { // Disconnect handler
              //std::cout << "Client " << getHostStr(client) << " disconnected\n";
            },

            {1, 1, 1} // Keep alive{idle:1s, interval: 1s, pk_count: 1}
            );
            //Start server
            if(server.start() == TcpServer::status::up) {
                std::cout<<"Server listen on port:"<<server.getPort()<<std::endl;
                server.joinLoop();
            } else {
                std::cout<<"Server start error! Error code:"<< int(server.getStatus()) <<std::endl;
            }
            flagInitServer = false;
        }

    }
}

int16_t rec_buffer[255999996];


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
          stringFromServer = (const char*)data.data_ptr;
          emit sendMessageFromServer();

          DataBuffer data_ = client.loadData();
          stringFromServer = (const char*)data_.data_ptr;
          emit sendMessageFromServer();

            QString str2reg = QString(stringFromServer);
            QRegExp rx("Server Up with send Data (\\d+)(\\s*) bloks");
            int pos = rx.indexIn(str2reg);
            QStringList list = rx.capturedTexts();
             int NumOfBlocks = 0;
            if (!list.isEmpty())
            {
                NumOfBlocks = list[1].toInt();
                qDebug()<<list<<list[1]<<NumOfBlocks;
            }

          int cntrFloatSymbols = 0;
          for (int cntrLoopRec = 0;cntrLoopRec<NumOfBlocks;cntrLoopRec++)
          {
          DataBuffer data__ = client.loadData();
          //qDebug()<<"data__"<<data__.size<<"bytes";
          if (data__.size)
          {
          for (int k=0;k<1024/sizeof(int16_t);k++)
          {
              if (cntrFloatSymbols<samplesCountToRecieve*2)
                rec_buffer[cntrFloatSymbols++] = *(int16_t*)(data__.data_ptr+k*sizeof(int16_t));
             //qDebug()<<cntrFloatSymbols-1<<":"<< rec_buffer[cntrFloatSymbols-1];
                        for (int k=0;k<1e4;k++)
                        {
                          asm("nop"); // TODO: убрать не костыль а КОСТЫЛИЩЕ (надобно сделать неблочащие друг друга события
                        }

          }
//          for (int k=0;k<1e3;k++)
//          {
//            asm("nop");
//          }
          }
          qDebug()<<"Recieve data block "<<cntrLoopRec;
          }

          if (NumOfBlocks)
          {
              stringFromServer = "!Принял файл, записанный Lime";
              emit sendMessageFromServer();
//              QString OutName;
//              bool useDefaultOutName = false;
               QString filenameOutD;
              if (useDefaultOutName){
                  QDate cd = QDate::currentDate();
                  QTime ct = QTime::currentTime();
                  QDir dir(QDir::currentPath() + "/res/");
                  if (!dir.exists()){
                      dir.mkdir(".");
                  }
                filenameOutD = QDir::currentPath() + "/res/DataOutput-" +cd.toString("dd-MM-yyyy") +"-" + ct.toString("HH-mm-ss") +  ".txt";
                qDebug()<<filenameOutD;
               }
               else {
                    filenameOutD = OutName;
               }
              QFile file(filenameOutD);
              if (file.open(QIODevice::ReadWrite)) {
                  QTextStream stream(&file);
                  for (int k=0;k<samplesCountToRecieve*2;k++)
                  {
                  stream << rec_buffer[k] << endl;
                  }
                  stringFromServer = "!Записал файл на диск";
                  emit sendMessageFromServer();

              }

                arrayPointsOfSpec.clear();
                for (int k=0;k<samplesCountToRecieve;k++)
                {
                    arrayPointsOfSpec.append(rec_buffer[k*2]);
                }
                emit sendMessageSpec();

          }
          else
          {
              DataBuffer data_ = client.loadData();
              stringFromServer = (const char*)data_.data_ptr;
              emit sendMessageFromServer();
          }
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
