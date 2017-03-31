#include "uhv2testapp.h"
#include "ui_uhv2testapp.h"
#include  <QTimer>

UHV2TestApp::UHV2TestApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UHV2TestApp),
    currentPump(new BinaryProtocol(0)),
    NowSI(new SerialInterface(""))
{
    ui->setupUi(this);
    listSerialPortsToComboBox();



    NowSI->moveToThread(&SerialInterfaceThread);
    QObject::connect(NowSI,SIGNAL(sigReadready(QByteArray)),this,SLOT(slotReceiveDataFromSerialInterface(QByteArray)));
    QObject::connect(this,SIGNAL(sigWriteToSerialInterface(QByteArray)),NowSI,SLOT(writeNow(QByteArray)));
    QObject::connect(this,SIGNAL(sigReconfiguretheSerialInterface(QString)),NowSI,SLOT(ReconfigSerialPort(QString)));
    QObject::connect(&SerialInterfaceThread,&QThread::finished,NowSI, &SerialInterface::deleteLater);
    QObject::connect(NowSI,SIGNAL(isConnected()),this,SLOT(serialInterfaceIsConnected()));
    QObject::connect(NowSI,SIGNAL(isNotConnected()),this,SLOT(serialInterfaceIsNoTConnected()));

    SerialInterfaceThread.start();

        ui->pushButton_CH1_HV->setCheckable(true);
        ui->pushButton_CH1_Protect->setCheckable(true);
        ui->pushButton_CH2_HV->setCheckable(true);
        ui->pushButton_CH2_Protect->setCheckable(true);
}

UHV2TestApp::~UHV2TestApp()
{
    SerialInterfaceThread.quit();
    SerialInterfaceThread.wait();
    delete ui;

}

void UHV2TestApp::listSerialPortsToComboBox()
{

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBoxSerial->addItem(info.portName());
    }
}

void UHV2TestApp::on_pushButton_SerialConnect_clicked(bool checked)
{
    emit sigReconfiguretheSerialInterface(ui->comboBoxSerial->currentText());
}

void UHV2TestApp::on_comboBoxSerial_currentIndexChanged(int index)
{
    ui->pushButton_SerialConnect->setText("Connect");
    ui->pushButton_SerialConnect->setDisabled(false);
}

void UHV2TestApp::slotReceiveDataFromSerialInterface(const QByteArray &data)
{
    qDebug() << data.toHex() << endl;
    qDebug() << data<< endl;
    QByteArray tmpData = data;
    if(tmpData.size() > 1)
    {
        while(tmpData.at(0) == 0x06)
        {
            tmpData.remove(0,1);
        }
        BinaryProtocol &tmp = BinaryProtocol::BP(tmpData);
        switch (tmp.GetCmd()) {
        case 0x5330: // readV

            if(tmp.GetChannel() == 1)
            {
                ui->labelCH1_V->setText("V: " + tmp.GetData());
            }
            if(tmp.GetChannel() == 2)
            {
                ui->labelCH2_V->setText("V: " +tmp.GetData());
            }
            break;
        case 0x5430: // readI
            if(tmp.GetChannel() == 1)
            {
                ui->labelCH1_I->setText("I: " +tmp.GetData());
            }
            if(tmp.GetChannel()  == 2)
            {
                ui->labelCH2_I->setText("I: " +tmp.GetData());
            }
            break;

        case 0x5530: // readP
            if(tmp.GetChannel() == 1)
            {
                ui->labelCH1_P->setText("P: " +tmp.GetData());
            }
            if(tmp.GetChannel() == 2)
            {
                ui->labelCH2_P->setText("P: " +tmp.GetData());
            }
            break;

        default:

            break;
        }
//      QByteArray anotherTmp;
//      anotherTmp.append(0x06);
//        emit sigWriteToSerialInterface(anotherTmp);

    }
}

void UHV2TestApp::serialInterfaceIsConnected()
{
    ui->pushButton_SerialConnect->setText("Connected");
    ui->pushButton_SerialConnect->setDisabled(true);


}

void UHV2TestApp::serialInterfaceIsNoTConnected()
{
    ui->pushButton_SerialConnect->setDisabled(false);
    ui->pushButton_SerialConnect->setText("Connect");
}

void UHV2TestApp::on_pushButton_clicked()
{
    BinaryProtocol &tmp = BinaryProtocol::BP(0);
    //emit sigWriteToSerialInterface(tmp.ReadDevType().GenMsg());
    //emit sigWriteToSerialInterface(tmp.Ch1().HVSwitch().On().GenMsg());
    emit sigWriteToSerialInterface(tmp.Ch2().ReadP().GenMsg());
    //emit sigWriteToSerialInterface(tmp.Ch1().Read().HVSwitch().GenMsg());
   // emit sigWriteToSerialInterface(tmp.noCh().Read().SerialProperty().GenMsg());
    //  emit sigWriteToSerialInterface(tmp.Ch1().Read().HVSwitch().GenMsg());


}

void UHV2TestApp::on_pushButton_CH1_HV_clicked(bool checked)
{
    static QTimer tmpTimer;
    BinaryProtocol &tmp = BinaryProtocol::BP(0);
    if(checked)
    {
        emit sigWriteToSerialInterface(tmp.Ch1().HVSwitch().On().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_HV->setText("HV OFF");
        QObject::connect(&tmpTimer,&QTimer::timeout,this,&UHV2TestApp::slotCollectDataOnCH1);

        tmpTimer.start(10000);

    }
    else
    {
        emit sigWriteToSerialInterface(tmp.Ch1().HVSwitch().Off().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_HV->setText("HV ON");

        tmpTimer.stop();
    }

}


void UHV2TestApp::on_pushButton_CH1_HV_toggled(bool checked)
{

}

void UHV2TestApp::on_pushButton_CH1_Protect_clicked(bool checked)
{
    BinaryProtocol &tmp = BinaryProtocol::BP(0);
    if(checked)
    {
        emit sigWriteToSerialInterface(tmp.Ch1().ProtectSwitch().On().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_Protect->setText("Protect OFF");
    }
    else
    {
        emit sigWriteToSerialInterface(tmp.Ch1().ProtectSwitch().Off().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_Protect->setText("Protect ON");
    }
}

void UHV2TestApp::on_pushButton_CH2_HV_clicked(bool checked)
{
    static QTimer tmpTimer;
    BinaryProtocol &tmp = BinaryProtocol::BP(0);
    if(checked)
    {
        emit sigWriteToSerialInterface(tmp.Ch2().HVSwitch().On().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_HV->setText("HV OFF");
        QObject::connect(&tmpTimer,&QTimer::timeout,this,&UHV2TestApp::slotCollectDataOnCH2);

        tmpTimer.start(30000);
    }
    else
    {
        emit sigWriteToSerialInterface(tmp.Ch2().HVSwitch().Off().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_HV->setText("HV ON");
        tmpTimer.stop();
    }
}

void UHV2TestApp::on_pushButton_CH2_Protect_clicked(bool checked)
{
    BinaryProtocol &tmp = BinaryProtocol::BP(0);
    if(checked)
    {
        emit sigWriteToSerialInterface(tmp.Ch2().ProtectSwitch().On().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_Protect->setText("Protect OFF");
    }
    else
    {
        emit sigWriteToSerialInterface(tmp.Ch2().ProtectSwitch().Off().GenMsg());
        qDebug()  << checked;
        ui->pushButton_CH1_Protect->setText("Protect ON");
    }
}

void UHV2TestApp::slotCollectDataOnCH1()
{
    BinaryProtocol &tmp = BinaryProtocol::BP(0);

    emit sigWriteToSerialInterface(tmp.Ch1().ReadP().GenMsg());
    emit sigWriteToSerialInterface(tmp.Ch1().ReadV().GenMsg());
    emit sigWriteToSerialInterface(tmp.Ch1().ReadI().GenMsg());
}


void UHV2TestApp::slotCollectDataOnCH2()
{
    BinaryProtocol &tmp = BinaryProtocol::BP(0);

    emit sigWriteToSerialInterface(tmp.Ch2().ReadP().GenMsg());
    //emit sigWriteToSerialInterface(tmp.Ch2().ReadV().GenMsg());
   // emit sigWriteToSerialInterface(tmp.Ch2().ReadI().GenMsg());
}

void UHV2TestApp::on_pushButton_CH1_HV_clicked()
{

}

void UHV2TestApp::on_pushButtonACK_clicked()
{

//    QByteArray tmp;
//    tmp.append(0x06);
//    emit sigWriteToSerialInterface(tmp);

    emit sigWriteToSerialInterface(BinaryProtocol::BP(0).noCh().WriteSerialReset().On().GenMsg());
}

void UHV2TestApp::on_pushButton_2_clicked()
{
    this->close();
}
