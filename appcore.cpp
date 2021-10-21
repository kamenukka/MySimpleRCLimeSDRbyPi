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
    //connect(&dataFortreadTCP, &TreadObject::sendMessage,  this, &AppCore::SendSomething, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::endOfSend,  this, &AppCore::getSomeFlag, Qt::DirectConnection);
    dataFortreadTCP.moveToThread(&TCPthread);    // Передаём объекты в соответствующие потоки

}



void AppCore::init()
{
    array2display.clear();
    emit sendToQml(FlagInit,Res,array2display);
}


void AppCore::TCPThread()
{
    TCPthread.start();
    dataFortreadTCP.flagTCPIsActive = true;


}

void AppCore::TCPstopTCP()
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
qDebug()<<"Tread2Calc";
dataFortreadTCP.arrayPoints.clear();
dataFortreadTCP.arrayPoints.append(1.0);
dataFortreadTCP.arrayPoints.append(2.0);
dataFortreadTCP.arrayPoints.append("3.0");
qDebug()<<dataFortreadTCP.arrayPoints;


}

void AppCore::SendSomething2(QString s)
{
dataFortreadTCP.arrayPoints.clear();
QString fileName = "D:\\IQ.txt";
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
dataFortreadTCP.arrayPoints.append(1.0);
dataFortreadTCP.arrayPoints.append(1.0);
dataFortreadTCP.arrayPoints.append(0.0);
dataFortreadTCP.arrayPoints.append(0.0);
dataFortreadTCP.arrayPoints.append(1.0);
dataFortreadTCP.arrayPoints.append(1.0);
dataFortreadTCP.arrayPoints.append(0.7);
dataFortreadTCP.arrayPoints.append(array2display.length());



for (int i=0;i<array2display.length();i++)
{
  tmpVal = array2display[i].toFloat();
  dataFortreadTCP.arrayPoints.append((tmpVal*32767/tmpMax));
}
dataFortreadTCP.flagReady = true;

}
qDebug()<<"Tread2Calc";
}

void AppCore::client() {
/* */
}
