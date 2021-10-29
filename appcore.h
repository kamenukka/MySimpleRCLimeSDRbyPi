#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "treadObject.h"
#include <QThread>
#include <QSharedPointer>
#include <QMap>
#include <QDir>
#include <QSettings>



class AppCore : public QObject
{
    Q_OBJECT
public:
    explicit AppCore(QObject *parent = 0);


signals:
    // Сигнал для передачи данных в qml-интерфейс
    void sendToQml(bool FlagInit,float Res, QVariantList array2display);
    void sendToQmlMsg(bool flagConnect);
    void  sendCounterToQml(int counter);
    void  specValueChanged(QVariantList arr2spec);
    void sendToQmlMsgCustom(QString mess);
    void sendToQmlSettings(QString IP, QString port, double freq, double Fd,  QString mode, double Gain, double oversampling, short NumChanTx,
                           short NumChanRx, QString nameIn, QString nameOut, int SecondsToPlay, int NumOfCyclesRec, int LenArrayToRec,
                           int txfifosize, int txThVslat, int rxfifosize, int rxThVslat, QString fmt, QString theme);
    void sendToDopQML(QString stringFromServer);




public slots:
    void init();
    void client();
    void tCPThread(QString,QString);
    void tCPstopTCP();
    void SendSomething();
    void sendMessages();
    void sendSpec();
    void sendBreak();
    void writeSettings(QString IP, QString port, double freq, double Fd,  QString mode, double Gain, double oversampling, short NumChanTx,
                       short NumChanRx, QString nameIn, QString nameOut, int SecondsToPlay, int NumOfCyclesRec, int LenArrayToRec,
                       int txfifosize, int txThVslat, int rxfifosize, int rxThVslat, QString fmt, QString theme);
    bool readSettings();
    void SendSomethingToDop(QString mes);
    void sendSomething2(QString, QString,int, int,
                        int, int, int, int,
                        int, int, int,
                        int, int,
                        int, int,
                        QString);
    void getSomeFlag();
    void flagSpecSet(bool, bool, int);


private:
    float Res;
    float latPRD;
    float longPRD;
    float latPRM;
    float longPRM;
    int TimesToRec = 1;
    QVariantList array2display;
    QVariantList array2init;
    QVariantList array2displayMap;
    QVariantList ModeNames;
    QVariantList ModeValues;
    QVariantList arr2spec;
    bool FlagInit;
    bool flagToSend = false;
    int LenP = 0;
    int counter = 0;
    int counterFinishedWorkers = 0;
    bool flagSpec = false;
    bool SpecAnalyzerMode = false;
    struct Point
    {
        QString Name;
        double lat;
        double lon;
    };
    Point Points[400];
    Point BkpPoints[400];
    QThread thread_1;
    int NumOfThreads = 12;

    TreadObject dataFortreadTCP;
    QThread TCPthread;

    TreadObject dataFortreadTCPServer ;
    QThread TCPthreadServer;


    TreadObject * ArrayTreadForCalculate = new TreadObject [NumOfThreads];
    QThread * thread_array = new QThread[NumOfThreads];
    TreadObject treadForCalculate;
    QSharedPointer<QVariantList> array2Thread = QSharedPointer<QVariantList>(new QVariantList);
    const int secondsPerPx = 3;  //arc seconds per pixel (3 equals cca 90m)
    const int totalPx = 1201;
    const char* folder = "./srtm"; //C:/proj/getTropoLoss
    FILE* srtmFd = NULL;
    int srtmLat = 255; //default never valid
    int srtmLon = 255;
    unsigned char * srtmTile = NULL;
    QString str;
    QMap<QString, unsigned char *> mapHash;
    QMap<QString, bool> mapExist;
    QMutex mut;
    float angleFrom = 0;
    float angleTo = 360;
    float radius_small=10e3;
    float radius_big = 100e3;
    QString IP, port;


};

#endif // APPCORE_H
