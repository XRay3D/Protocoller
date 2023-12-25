#ifndef MY_PROTOCOL_H
#define MY_PROTOCOL_H

#include "myprotokol.h"
#include <QComboBox>
#include <QDebug>
#include <QElapsedTimer>
#include <QMutex>
#include <QObject>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <QWaitCondition>
#include <stdint.h>

class TesterPort;
class Tester : public QObject, private MyProtokol {
    Q_OBJECT
    friend class TesterPort;

public:
    Tester(QObject* parent = nullptr);
    ~Tester();
    bool setBaudRate(qint32 baudRate);
    bool setPortName(const QString& name);
    QString errorString() const;

signals:
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    void Read(const QByteArray& data);
    void Error(const QString& errString, const QByteArray& data);

private:
    TesterPort* port_;
    QMutex mutex_;
    QThread portThread_;
    int stage_;
    bool ready_;
    mutable QSemaphore semaphore_;
    mutable bool result_;
    void RxNullFunction(const QByteArray& data);
};

class TesterPort : public QSerialPort, private MyProtokol {
    Q_OBJECT

public:
    TesterPort(Tester* testerInterface);
    ~TesterPort();
    void Open(int mode);
    void Close();
    void Write(const QByteArray& data);
    bool isOpen_;
    Tester* tester_;
    //    typedef void (Tester::*func)(const Parcel_t&);
    //    QVector<func> f_;

private:
    void ReadyRead();
    QByteArray data_;
    QByteArray tmpData_;
    QMutex mutex_;
    int couter_;
};

#endif // MY_PROTOCOL_H
