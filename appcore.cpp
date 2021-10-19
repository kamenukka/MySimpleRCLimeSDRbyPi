#include "appcore.h"
#include <math.h>
#include <stdio.h>
#include <thread>

#include<QDebug>
#include <QString>

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
    connect(&dataFortreadTCP, &TreadObject::sendMessage,  this, &AppCore::SendSomething, Qt::DirectConnection);
    connect(&dataFortreadTCP, SIGNAL(sendMessageProc(int, int,double*)),  this, SLOT(dispProc(int, int,double*)), Qt::DirectConnection);
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
qDebug()<<"Tread2Calc";
dataFortreadTCP.arrayPoints.clear();
dataFortreadTCP.arrayPoints.append(s);;
qDebug()<<dataFortreadTCP.arrayPoints;


}

void AppCore::client() {
/* */
}
