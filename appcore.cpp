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


#include "myFunc/fft.h"
#include "myFunc/complex.h"


const complex complex::j(0., 1.);

AppCore::AppCore(QObject *parent) : QObject(parent)
{

    connect(&TCPthread, &QThread::started, &dataFortreadTCP, &TreadObject::run);
    connect(&dataFortreadTCP, &TreadObject::finished, &TCPthread, &QThread::terminate);
    //connect(&dataFortreadTCP, SIGNAL(sendMessage(QVariantList)),  this, SLOT(SendSomething(QVariantList)), Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::endOfSend,  this, &AppCore::getSomeFlag, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageFromServer,  this, &AppCore::SendSomething, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageErrorFromServer,  this, &AppCore::sendMessages, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageSpec,  this, &AppCore::sendSpec, Qt::DirectConnection);

    dataFortreadTCP.moveToThread(&TCPthread);    // Передаём объекты в соответствующие потоки

    connect(&TCPthreadServer, &QThread::started, &dataFortreadTCPServer, &TreadObject::run);
    connect(&dataFortreadTCPServer, &TreadObject::finished, &TCPthreadServer, &QThread::terminate);
    //connect(&dataFortreadTCP, SIGNAL(sendMessage(QVariantList)),  this, SLOT(SendSomething(QVariantList)), Qt::DirectConnection);
    connect(&dataFortreadTCPServer,  &TreadObject::endOfSend,  this, &AppCore::getSomeFlag, Qt::DirectConnection);
    connect(&dataFortreadTCPServer,  &TreadObject::sendMessageFromServer,  this, &AppCore::SendSomething, Qt::DirectConnection);
    connect(&dataFortreadTCPServer,  &TreadObject::sendMessageErrorFromServer,  this, &AppCore::sendMessages, Qt::DirectConnection);
    dataFortreadTCPServer.moveToThread(&TCPthreadServer);    // Передаём объекты в соответствующие потоки


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
    dataFortreadTCP.isServer = false;
    TCPthread.start();
    dataFortreadTCP.flagTCPIsActive = true;

    /*dataFortreadTCPServer.IP = IP;
    dataFortreadTCPServer.port = port;
    dataFortreadTCPServer.isServer = true;
    TCPthreadServer.start();*/


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


QVariantList myFFT(QVariantList in,int NFFT)
{
    NFFT =(in.length()/2>8192)?8192:256;
    complex *s_in = new complex[NFFT];
    double *out = new double[NFFT];
    complex *s_out = new complex[NFFT];

    for (int i =0;i<NFFT;i++)
    {
        s_in[i] = in[2*i].toDouble() + complex::j *  in[2*i+1].toDouble();
    }
    Forward(s_in, s_out, NFFT);

    for (int i =0;i<NFFT/2;i++)
    {
        out[i+NFFT/2] = 10*log10(sqrt(s_out[i].norm())/2048/NFFT/NFFT);
       out[i] = 10*log10(sqrt(s_out[i+NFFT/2].norm())/2048/NFFT/NFFT);
    }


    QVariantList outArr;
    outArr.clear();
    for (int i =0;i<NFFT;i++)
    {
        if ((i%32)==0)
        {
            outArr.append(out[i]);
        }
    }


   delete [] s_out;
    delete [] out;
    delete [] s_in;
   return outArr;
}

void AppCore::flagSpecSet(bool flg)
{
    flagSpec = flg;
}



void AppCore::sendSpec()
{

      arr2spec.clear();
      QVariantList arr2spec2;
      arr2spec2.clear();
    if (flagSpec)
    {
    for (int i=0;i<dataFortreadTCP.arrayPointsOfSpec.length();i++)
    {
        arr2spec2.append(dataFortreadTCP.arrayPointsOfSpec[i]);
    }
    qDebug()<<"arr2spec.length()"<<arr2spec.length();

    arr2spec = myFFT(arr2spec2,1);

    qDebug()<<"AFTER arr2spec.length()"<<arr2spec.length();
    emit specValueChanged(arr2spec);
    }


}

void AppCore::sendSomething2(QString fileName,QString fileNameOut, int Mode,
                             int Freq, int Fd, int Gain, int OverSampling, int NumChanTrans,
                             int NumChanRec, int SecondsToPlay, int SizeOfRecArr,
                             int tx_streamfifoSize, int tx_streamthroughputVsLatency,
                             int rx_streamfifoSize, int rx_streamthroughputVsLatency,
                             QString FMT)
{
dataFortreadTCP.arrayPoints.clear();
QString line;

qDebug()<<fileNameOut<<fileNameOut.isEmpty();
if (fileNameOut.isEmpty())
{
        dataFortreadTCP.OutName = "";
        dataFortreadTCP.useDefaultOutName = true;
}
else
{
    dataFortreadTCP.OutName = fileNameOut;
    dataFortreadTCP.useDefaultOutName = false;
}

float tmpVal;
float tmpMax = -1e12;
array2display.clear();

if (!flagToSend)
{
flagToSend = true;
dataFortreadTCP.flagReady = false;
if (Mode)
{
    qDebug()<<fileName;
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
}
// Для первичной синхронизации передачи файла
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(0.0));
dataFortreadTCP.arrayPoints.append(float(0.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(1.0));
dataFortreadTCP.arrayPoints.append(float(Mode));
Mode?dataFortreadTCP.arrayPoints.append(array2display.length()):dataFortreadTCP.arrayPoints.append(float(0));
dataFortreadTCP.arrayPoints.append(float(Freq));
dataFortreadTCP.arrayPoints.append(float(Fd));
dataFortreadTCP.arrayPoints.append(float(Gain));
dataFortreadTCP.arrayPoints.append(float(OverSampling));
dataFortreadTCP.arrayPoints.append(float(NumChanTrans));
dataFortreadTCP.arrayPoints.append(float(NumChanRec));
dataFortreadTCP.arrayPoints.append(float(SecondsToPlay));
dataFortreadTCP.arrayPoints.append(float(SizeOfRecArr));
dataFortreadTCP.arrayPoints.append(float(tx_streamfifoSize));
dataFortreadTCP.arrayPoints.append(float(tx_streamthroughputVsLatency/100));
dataFortreadTCP.arrayPoints.append(float(rx_streamfifoSize));
dataFortreadTCP.arrayPoints.append(float(rx_streamthroughputVsLatency/100));
(FMT=="float32")?dataFortreadTCP.arrayPoints.append(float(1)):dataFortreadTCP.arrayPoints.append(float(0)); //FMT


dataFortreadTCP.samplesCountToRecieve = SizeOfRecArr;


//for (int i=0;i<array2display.length();i++)
if (Mode)
{
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
}
dataFortreadTCP.flagReady = true;

}
}

void AppCore::client() {
/* */
}
