#include "appcore.h"
#include <math.h>
#include <stdio.h>
#include <thread>

#include<QDebug>
#include <QString>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h> //int16_t
#include <windows.h>
#include <string>
#include <iostream>
#include <complex>



AppCore::AppCore(QObject *parent) : QObject(parent)
{

    connect(&TCPthread, &QThread::started, &dataFortreadTCP, &TreadObject::run);
    connect(&dataFortreadTCP, &TreadObject::finished, &TCPthread, &QThread::terminate);
    //connect(&dataFortreadTCP, SIGNAL(sendMessage(QVariantList)),  this, SLOT(SendSomething(QVariantList)), Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::endOfSend,  this, &AppCore::getSomeFlag, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageFromServer,  this, &AppCore::SendSomething, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageErrorFromServer,  this, &AppCore::sendMessages, Qt::DirectConnection);

    dataFortreadTCP.moveToThread(&TCPthread);    // Передаём объекты в соответствующие потоки

}



void AppCore::init()
{
    array2display.clear();
    FlagInit = true;
    Res = 0;
    emit sendToQml(FlagInit,Res,array2display);
}


void AppCore::tCPThread(QString IP,QString port)
{
    dataFortreadTCP.IP = IP;
    dataFortreadTCP.port = port;
    TCPthread.start();
    dataFortreadTCP.flagTCPIsActive = true;


}

void AppCore::tCPstopTCP()
{
    dataFortreadTCP.flagTCPIsActive = false;
    TCPthread.terminate();
}


void  AppCore::getSomeFlag()
{
    qDebug()<<"End of Sending via TCP";
    flagToSend = false;
}

void AppCore::SendSomething()
{
//qDebug()<<"Trhtead send"<<dataFortreadTCP.stringFromServer;
array2display.clear();
array2display.append(dataFortreadTCP.stringFromServer);
FlagInit = false;
Res = 1;
emit sendToQml(FlagInit,Res,array2display);
}

void AppCore::sendMessages()
{
 if (dataFortreadTCP.flagTCPIsActive)
    emit sendToQmlMsg(true);
 else
     emit sendToQmlMsg(false);

}

void AppCore::sendSomething2(QString fileName)
{
dataFortreadTCP.arrayPoints.clear();
qDebug()<<fileName;
QString line;
float tmpVal;
float tmpMax = -1e12;
array2display.clear();

if (!flagToSend)
{
flagToSend = true;
dataFortreadTCP.flagReady = false;
QFile inputFile(fileName);
if (inputFile.open(QIODevice::ReadOnly))
{
   QTextStream in(&inputFile);
   while (!in.atEnd())
   {
      line = in.readLine();
      tmpVal = line.toFloat();
      if (abs(tmpVal)>abs(tmpMax))
          tmpMax = abs(tmpVal);
      array2display.append(tmpVal);
   }
   inputFile.close();
}
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(0.0));
dataFortreadTCP.arrayPoints.append(float(0.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(0.7));
dataFortreadTCP.arrayPoints.append(array2display.length());



//for (int i=0;i<array2display.length();i++)
int lenTrash = 0;
for (int i=0;i<array2display.length();i++)

{
  tmpVal = array2display[i].toFloat();
  if ((i%(1024/4)==0)&&(i>0))
  {
      for (int j=0;j<lenTrash;j++)
        dataFortreadTCP.arrayPoints.append(0);
  }
  dataFortreadTCP.arrayPoints.append((tmpVal)); //*32767/tmpMax
//  dataFortreadTCP.arrayPoints.append(float(i));

}
dataFortreadTCP.flagReady = true;

}
}

void AppCore::client() {
/* */
}
