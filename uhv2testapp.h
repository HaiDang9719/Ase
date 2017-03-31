#ifndef UHV2TESTAPP_H
#define UHV2TESTAPP_H

#include <QMainWindow>
#include "binaryprotocol.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include "serialinterface.h"
namespace Ui {
class UHV2TestApp;
}

class UHV2TestApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit UHV2TestApp(QWidget *parent = 0);
    ~UHV2TestApp();

private slots:
    void on_pushButton_SerialConnect_clicked(bool checked);

    void on_comboBoxSerial_currentIndexChanged(int index);
    void slotReceiveDataFromSerialInterface(const QByteArray &data);
    void serialInterfaceIsConnected();
    void serialInterfaceIsNoTConnected();
    void on_pushButton_clicked();

    void on_pushButton_CH1_HV_clicked(bool checked);

    void on_pushButton_CH1_HV_toggled(bool checked);

    void on_pushButton_CH1_Protect_clicked(bool checked);

    void on_pushButton_CH2_HV_clicked(bool checked);

    void on_pushButton_CH2_Protect_clicked(bool checked);

    void slotCollectDataOnCH1();
    void slotCollectDataOnCH2();

    void on_pushButton_CH1_HV_clicked();

    void on_pushButtonACK_clicked();

signals:
    void quitSerialThread();
    void sigWriteToSerialInterface(const QByteArray &data);
    void sigReconfiguretheSerialInterface(const QString &Portname);

private:
    Ui::UHV2TestApp *ui;
    BinaryProtocol *currentPump;
    void listSerialPortsToComboBox();
    QThread SerialInterfaceThread;
    SerialInterface *NowSI;


};

#endif // UHV2TESTAPP_H
