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
    connect(&dataFortreadTCP,  SIGNAL(sendMessageDopFromServer(QString)),  this, SLOT(SendSomethingToDop(QString)), Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageErrorFromServer,  this, &AppCore::sendMessages, Qt::DirectConnection);
    connect(&dataFortreadTCP,  &TreadObject::sendMessageBreakFromServer,  this, &AppCore::sendBreak, Qt::DirectConnection);

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


void AppCore::writeSettings(QString IP, QString port, double freq, double Fd,  QString mode, double Gain, double oversampling, short NumChanTx,
                            short NumChanRx, QString nameIn, QString nameOut, int SecondsToPlay, int NumOfCyclesRec, int LenArrayToRec,
                            int txfifosize, int txThVslat, int rxfifosize, int rxThVslat, QString fmt,QString theme)
{
    QSettings* settings = new QSettings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
    settings->clear();
    settings->beginGroup("IPconfig");
    settings->setValue("IP",IP);
    settings->setValue("port",port);
    settings->endGroup();
    settings->beginGroup("MainLimeSettings");
    settings->setValue("CentralFrequency",freq);
    settings->setValue("SampleRate",Fd);
    settings->setValue("Mode",mode);
    settings->setValue("TXGain",Gain);
    settings->setValue("RXOversampling",oversampling);
    settings->setValue("TXNumberOfChannel",NumChanTx);
    settings->setValue("RXNumberOfChannel",NumChanRx);
    settings->setValue("TXSecondsToPlay",SecondsToPlay);
    settings->setValue("RXNumberCyclesToRecord",NumOfCyclesRec);
    settings->setValue("RXLengthOfArrayToRecord",LenArrayToRec);
    settings->setValue("FormatOfData",fmt);
    settings->endGroup();
    settings->beginGroup("AdditionalLimeSettings");
    settings->setValue("TXstreamFIFOsize",txfifosize);
    settings->setValue("TXstreamThroughputVslatency",txThVslat);
    settings->setValue("RXstreamFIFOsize",rxfifosize);
    settings->setValue("RXstreamThroughputVslatency",rxThVslat);
    settings->endGroup();
    settings->beginGroup("SettingsOfFiles");
    settings->setValue("CustomNameOfInFiles",nameIn);
    settings->setValue("CustomNameOfOutFiles",nameOut);
    settings->setValue("DefaultNameOfOutFiles",nameOut);
    settings->endGroup();
    settings->beginGroup("SettingsOfGUI");
    settings->setValue("Theme",theme);
    settings->endGroup();
    settings->sync();
}

bool AppCore::readSettings()
{
    bool res = false;
    if (QFile(QDir::currentPath() + "/config.ini").exists())
    {

        res = true;
        QString IP;
        QString port;
        double freq;
        double Fd;
        QString mode;
        double Gain;
        double oversampling;
        short NumChanTx;
        short NumChanRx;
        QString nameIn;
        QString nameOut;
        int SecondsToPlay;
        int NumOfCyclesRec;
        int LenArrayToRec;
        int txfifosize;
        int txThVslat;
        int rxfifosize;
        int rxThVslat;
        QString fmt;

        QSettings* settings = new QSettings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
        settings->beginGroup("IPconfig");
        IP = settings->value("IP").toString();
        port = settings->value("port").toString();
        settings->endGroup();
        settings->beginGroup("MainLimeSettings");
        freq = settings->value("CentralFrequency").toInt();
        Fd = settings->value("SampleRate").toInt();
        mode = settings->value("Mode").toString();
        Gain = settings->value("TXGain").toInt();
        oversampling = settings->value("RXOversampling").toInt();
        NumChanTx = settings->value("TXNumberOfChannel").toInt();
        NumChanRx = settings->value("RXNumberOfChannel").toInt();
        SecondsToPlay = settings->value("TXSecondsToPlay").toInt();
        NumOfCyclesRec = settings->value("RXNumberCyclesToRecord").toInt();
        LenArrayToRec = settings->value("RXLengthOfArrayToRecord").toInt();
        fmt = settings->value("FormatOfData",fmt).toString();
        settings->endGroup();
        settings->beginGroup("AdditionalLimeSettings");
        txfifosize= settings->value("TXstreamFIFOsize").toInt();
        txThVslat = settings->value("TXstreamThroughputVslatency").toInt();
        rxfifosize = settings->value("RXstreamFIFOsize").toInt();
        rxThVslat = settings->value("RXstreamThroughputVslatency").toInt();
        settings->endGroup();
        settings->beginGroup("SettingsOfFiles");
        nameIn = settings->value("CustomNameOfInFiles").toString();
        nameOut = settings->value("CustomNameOfOutFiles").toString();
        QString nameDefault= settings->value("DefaultNameOfOutFiles").toString();
        settings->endGroup();
        settings->beginGroup("SettingsOfGUI");
        QString theme = settings->value("Theme").toString();
        settings->endGroup();
        settings->sync();

        emit sendToQmlSettings(IP, port, freq, Fd,   mode, Gain, oversampling, NumChanTx,
                                NumChanRx,  nameIn,  nameOut,  SecondsToPlay,  NumOfCyclesRec,  LenArrayToRec,
                                txfifosize,  txThVslat,  rxfifosize,  rxThVslat, fmt,theme);

    }
    return res;
}



void AppCore::init()
{
    array2display.clear();
    FlagInit = true;
    Res = 0;
    emit sendToQml(FlagInit,Res,array2display);
    if (!readSettings())
        qDebug()<<"can't read settings";
}


void AppCore::tCPThread(QString IP_,QString port_)
{
    dataFortreadTCP.IP = IP_;
    dataFortreadTCP.port = port_;
    IP = IP_;
    port = port_;

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


void AppCore::SendSomethingToDop(QString stringFromServer)
{
    emit sendToDopQML(stringFromServer);
}


void AppCore::SendSomething()
{
//qDebug()<<"Trhtead send"<<dataFortreadTCP.stringFromServer;
QString tmpString = dataFortreadTCP.stringFromServer;

if (tmpString.contains("!Recieve"))
{
    if (tmpString.contains("Playing"))
    {
        tmpString = "!!Успешно отправил файл на Pi";
    }

}
if (tmpString.contains("!Please check you SDR"))
{
    tmpString = "!!Проверь подключение LimeSDR к Pi";
}
if (tmpString.contains("End streaming with status0"))
{
    tmpString = "!!LimeSDR успешно завершил передачу";
}
if (tmpString.contains("End recording with status0"))
{
    tmpString = "!!LimeSDR успешно завершил прием";
}
array2display.clear();
array2display.append(tmpString);
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


void AppCore::sendBreak()
{
    tCPstopTCP();
    emit sendToQmlMsgCustom("я хз что произошло");
    dataFortreadTCP.IP = IP;
    dataFortreadTCP.port = port;
    dataFortreadTCP.isServer = false;
    TCPthread.start();
    dataFortreadTCP.flagTCPIsActive = true;
    emit sendToQmlMsg(true);

}


QVariantList myFFT(QVariantList in,int NFFT)
{
    NFFT =(in.length()/2>10000)?8192:256;
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
    int numDiv = (NFFT==8192)?32:1;
    for (int i =0;i<NFFT;i++)
    {
        if ((i%numDiv)==0)
        {
            outArr.append(out[i]);
        }
    }


   delete [] s_out;
    delete [] out;
    delete [] s_in;
   return outArr;
}

void AppCore::flagSpecSet(bool flg, bool flg2, int TimesToRec_)
{
    flagSpec = flg;
    SpecAnalyzerMode = flg2;
    TimesToRec = TimesToRec_;
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

int numOfCycles = Mode?1:(SpecAnalyzerMode?TimesToRec:1);
dataFortreadTCP.numOfCycles = numOfCycles;
if (!dataFortreadTCP.flagReady)
{

dataFortreadTCP.arrayPoints.clear();
QString line;
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
int AlreadyOpened = 0;
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
dataFortreadTCP.arrayPoints.append(float(AlreadyOpened));


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
}


void AppCore::client() {
/* */
}
