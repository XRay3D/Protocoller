#include "tester.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QTimer>

#define Dbg 1

Tester::Tester(QObject* parent)
    : QObject(parent) {
    m_port = new TesterPort(this);
    m_port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, m_port, &QObject::deleteLater);
    connect(this, &Tester::Open, m_port, &TesterPort::Open);
    connect(this, &Tester::Close, m_port, &TesterPort::Close);
    connect(this, &Tester::Write, m_port, &TesterPort::Write);
    m_portThread.start(QThread::NormalPriority);
}

Tester::~Tester() {
    m_portThread.quit();
    m_portThread.wait();
}

QString Tester::errorString() const {
    return m_port->errorString();
}

bool Tester::setBaudRate(qint32 baudRate) {
    QMutexLocker locker(&m_mutex);
    emit Close();
    if(!m_semaphore.tryAcquire(1, 1000))
        return false;

    if(baudRate)
        m_port->setBaudRate(baudRate);

    emit Open(QIODevice::ReadWrite);
    if(!m_semaphore.tryAcquire(1, 1000))
        return false;

    return true;
}

bool Tester::setPortName(const QString& name) {
    QMutexLocker locker(&m_mutex);
    emit Close();
    if(!m_semaphore.tryAcquire(1, 1000))
        return false;

    if(!name.isEmpty())
        m_port->setPortName(name);

    emit Open(QIODevice::ReadWrite);
    if(!m_semaphore.tryAcquire(1, 1000))
        return false;

    return true;
}

void Tester::RxNullFunction(const QByteArray& data) {
    qDebug() << "RxNullFunction" << data;
    //m_semaphore.release();
}

////////////////////////////////
/// \brief SerialPort::SerialPort
/// \param manInterface
///
TesterPort::TesterPort(Tester* testerInterface)
    : m_isOpen(false)
    , m_tester(testerInterface)
//    , m_f(QVector<TesterPort::func>(0x100, &Tester::RxNullFunction))
{
    setBaudRate(QSerialPort::Baud9600);
    setParity(QSerialPort::NoParity);
    setFlowControl(QSerialPort::NoFlowControl);
    connect(this, &QSerialPort::readyRead, this, &TesterPort::ReadyRead);
}

TesterPort::~TesterPort() { }

void TesterPort::Open(int mode) {
    if(open(static_cast<OpenMode>(mode)))
        m_tester->m_semaphore.release();
}

void TesterPort::Close() {
    close();
    m_tester->m_semaphore.release();
}

void TesterPort::Write(const QByteArray& data) {
    qDebug() << "Write" << data.toHex().toUpper() << write(data);
}

void TesterPort::ReadyRead() {
    QMutexLocker locker(&m_mutex);
    m_data.append(readAll());
    qDebug() << "ReadyRead" << m_data.toHex().toUpper();
    for(int i = 0; i < m_data.size() - 3; ++i) {
        const Parcel_t* const parcel = reinterpret_cast<const Parcel_t*>(m_data.constData() + i);
        if(parcel->start == RX) {
            if((parcel->size + i) <= m_data.size()) {
                m_tmpData = m_data.mid(i, parcel->size);
                if(CheckData(m_tmpData)) {
                    m_tester->Read(m_tmpData);
                } else {
                    m_tester->Error("CRC_ERROR", m_tmpData);
                    m_data.clear();
                }
                m_data.remove(0, i + parcel->size);
                i = -1;
            }
        }
    }
}
