#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H
#include <QSerialPort>
#include <QObject>
#include <QDebug>


class SerialInterface: public QObject
{
    Q_OBJECT
        QString &mPortName;
        QSerialPort mSerialPort;
        quint8 mReadBufferSize;
        int mTimeOut4Buffer;
        bool mIsConnected;
        QList<QByteArray> commandList;


public:
    explicit SerialInterface(const QString &PortName);
        ~SerialInterface();


public slots:
    void writeNow(const QByteArray &dataToWrite);
    void readNow();
    void ReconfigSerialPort(const QString &PortName);
    void doTransmission();

signals:
    void sigReadready(const QByteArray &data);
    void isConnected();
    void isNotConnected();
    void isDisconnected();
    void startTrasmission();
};

#endif // SERIALINTERFACE_H
