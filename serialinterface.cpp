#include "serialinterface.h"
#include <QMutex>
#include <QThread>
#include "binaryprotocol.h"

#define BUFFERFORWRITENOW 6


SerialInterface::SerialInterface(const QString &PortName):
    mPortName(* new QString (PortName)),
    mSerialPort( new QSerialPort(PortName, NULL)),
    mReadBufferSize(256),
    mTimeOut4Buffer(200),
    mIsConnected(false)
{

        mSerialPort.setReadBufferSize(mReadBufferSize); //NEED Discussed
        mSerialPort.setDataBits(QSerialPort::Data8);
        mSerialPort.setBaudRate(QSerialPort::Baud9600);
        mSerialPort.setStopBits(QSerialPort::OneStop);
        mSerialPort.setParity(QSerialPort::NoParity);   //NEED CHECK pg98
        mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

        connect(&mSerialPort, SIGNAL(readyRead()),this,SLOT(readNow()));
        connect(this,&SerialInterface::startTrasmission,this,&SerialInterface::doTransmission);
        ReconfigSerialPort(PortName);
}

SerialInterface::~SerialInterface()
{

    mSerialPort.close();

}

void SerialInterface::ReconfigSerialPort(const QString &PortName)
{
           mSerialPort.close();
           mSerialPort.setPortName(PortName);
           mPortName = PortName;
           //Validity of Connection Should Add Here
           if (mSerialPort.open(QIODevice::ReadWrite))
           {
               mIsConnected = true;
               emit isConnected();
           }

           else
           {
               mIsConnected = false;
               emit isNotConnected();
           }

}

void SerialInterface::writeNow(const QByteArray &dataToWrite)
{
//    static QList<QByteArray> tmpBuffer;
//    static qint8 counter = 0;
//    if(mIsConnected)
//    {
////
////        mSerialPort.write(dataToWrite);
////       // while(mSerialPort.waitForBytesWritten(mTimeOut4Buffer));

//        tmpBuffer.append(dataToWrite);
//        counter++;
//    }
//    if (counter == BUFFERFORWRITENOW)
//    {
//        counter = 0;
//        commandList.append(tmpBuffer);
//        tmpBuffer.clear();
//        emit startTrasmission();
//    }


    commandList.append(dataToWrite);
    emit startTrasmission();
}

void SerialInterface::readNow()
{
    QByteArray data;

//    data = mSerialPort.readAll();
    while(mSerialPort.waitForReadyRead(mTimeOut4Buffer))
    {
        data += mSerialPort.readAll();
    }
    qDebug()<<"emit sigReadready from readNow" << endl;
    emit sigReadready(data);
}

void SerialInterface::doTransmission()
{
    QMutex  mutex;
    QByteArray data;

    if(mIsConnected)
    {
        disconnect(&mSerialPort, SIGNAL(readyRead()),this,SLOT(readNow()));
        mutex.lock();
        while(commandList.size() != 0)
        {
            mSerialPort.write(commandList.at(0));
            qDebug() << "sending: " << commandList.at(0).toHex() << endl;

            QThread::msleep(50);
                if(mSerialPort.waitForReadyRead(mTimeOut4Buffer))
                {
                    data.clear();
                    data = mSerialPort.readAll();
                    while(mSerialPort.waitForReadyRead(mTimeOut4Buffer))
                    {
                        data+= mSerialPort.readAll();
                    }
                    qDebug() << "emit sigReadready from transmission thread" << endl;

                    emit sigReadready(data);
                    QThread::msleep(5);
//                    if(data.at(0) == 0x15)
//                    {
//                        mSerialPort.close();
//                        QThread::msleep(10);
//                        mSerialPort.open(QIODevice::ReadWrite);
//                    }
//                    else
//                    {
//                        commandList.removeAt(0);
//                        emit sigReadready(data);
//                    }
commandList.removeAt(0);
                }




        }
        mutex.unlock();
//        connect(&mSerialPort, SIGNAL(readyRead()),this,SLOT(readNow()));
    }

}
