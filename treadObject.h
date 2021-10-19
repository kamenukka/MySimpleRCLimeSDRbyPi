#ifndef TREADOBJECT_H
#define TREADOBJECT_H

#include <QObject>
#include <QVariantList>
#include <QMutex>


class TreadObject : public QObject
{
    Q_OBJECT
    // Свойство, управляющее работой потока
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    // Первое сообщение в объекте
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    // Второе сообщение, которое будем передавать через сигнал/слот во второй объект
    Q_PROPERTY(QString message_2 READ message_2 WRITE setMessage_2 NOTIFY message_2Changed)

    bool m_running;
    QString m_message;
    QString m_message_2;
    int count;  // Счётчик, по которому будем ориентироваться на то,
                // что потоки выполняются и работают

private:
    QMutex mMutex;


public:
    explicit TreadObject(QObject *parent = 0);
    bool running() const;
    QString message() const;
    QString message_2() const;
    QVariantList array2disp;
    QVariantList arrayPoints;
    QMap<QString, unsigned char *> tilesMap;
    QMap<QString, bool> tilesExist;
    QMutex mut;
    int counter;
    int Number = -10;

signals:
    void finished();    // Сигнал, по которому будем завершать поток, после завершения метода run
    void runningChanged(bool running);
    void messageChanged(QString message);
    void message_2Changed(QString message_2);
    void sendMessage(QVariantList array2disp);
    void sendMessageProc(int counter, int Number, double* arr);


public slots:
    void run(/*QVariantList arrayPoints,float AntennaHeigthPRD,float AntennaHeigthPRM,float KuPRD,float KuPRM,float Freq,float p,float Irain, bool flagDownload*/); // Метод с полезной нагрузкой, который может выполняться в цикле
//    void runThreadN();
    void calculate();
    void setRunning(bool running);
    void setMessage(QString message);
    void setMessage_2(QString message_2);
    void client();

};

#endif // TREADOBJECT_H
