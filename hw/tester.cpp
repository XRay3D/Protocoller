#include "tester.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QTimer>

#define Dbg 1

Tester::Tester(QObject* parent)
    : QObject(parent) {
    port_ = new TesterPort(this);
    port_->moveToThread(&portThread_);
    connect(&portThread_, &QThread::finished, port_, &QObject::deleteLater);
    connect(this, &Tester::Open, port_, &TesterPort::Open);
    connect(this, &Tester::Close, port_, &TesterPort::Close);
    connect(this, &Tester::Write, port_, &TesterPort::Write);
    portThread_.start(QThread::NormalPriority);
}

Tester::~Tester() {
    portThread_.quit();
    portThread_.wait();
}

QString Tester::errorString() const {
    return port_->errorString();
}

bool Tester::setBaudRate(qint32 baudRate) {
    QMutexLocker locker(&mutex_);
    emit Close();
    if(!semaphore_.tryAcquire(1, 1000))
        return false;

    if(baudRate)
        port_->setBaudRate(baudRate);

    emit Open(QIODevice::ReadWrite);
    if(!semaphore_.tryAcquire(1, 1000))
        return false;

    return true;
}

bool Tester::setPortName(const QString& name) {
    QMutexLocker locker(&mutex_);
    emit Close();
    if(!semaphore_.tryAcquire(1, 1000))
        return false;

    if(!name.isEmpty())
        port_->setPortName(name);

    emit Open(QIODevice::ReadWrite);
    if(!semaphore_.tryAcquire(1, 1000))
        return false;

    return true;
}

void Tester::RxNullFunction(const QByteArray& data) {
    qDebug() << "RxNullFunction" << data;
    //semaphore_.release();
}

////////////////////////////////
/// \brief SerialPort::SerialPort
/// \param manInterface
///
TesterPort::TesterPort(Tester* testerInterface)
    : isOpen_(false)
    , tester_(testerInterface)
//    , f_(QVector<TesterPort::func>(0x100, &Tester::RxNullFunction))
{
    setBaudRate(QSerialPort::Baud9600);
    setParity(QSerialPort::NoParity);
    setFlowControl(QSerialPort::NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &TesterPort::ReadyRead);
}

TesterPort::~TesterPort() { }

void TesterPort::Open(int mode) {
    if(open(static_cast<OpenMode>(mode)))
        tester_->semaphore_.release();
}

void TesterPort::Close() {
    close();
    tester_->semaphore_.release();
}

void TesterPort::Write(const QByteArray& data) {
    qDebug() << "Write" << data.toHex().toUpper() << write(data);
}

void TesterPort::ReadyRead() {
    QMutexLocker locker(&mutex_);
    data_.append(readAll());
    qDebug() << "ReadyRead" << data_.toHex().toUpper();
    for(int i = 0; i < data_.size() - 3; ++i) {
        const Parcel_t* const parcel = reinterpret_cast<const Parcel_t*>(data_.constData() + i);
        if(parcel->start == RX) {
            if((parcel->size + i) <= data_.size()) {
                tmpData_ = data_.mid(i, parcel->size);
                if(CheckData(tmpData_)) {
                    tester_->Read(tmpData_);
                } else {
                    tester_->Error("CRC_ERROR", tmpData_);
                    data_.clear();
                }
                data_.remove(0, i + parcel->size);
                i = -1;
            }
        }
    }
}
