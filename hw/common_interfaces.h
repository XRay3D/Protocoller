#ifndef COMMON_INTERFACES_H
#define COMMON_INTERFACES_H

#include <QString>

class CommonInterfaces {
public:
    virtual bool ping(const QString& portName = QString()) = 0;
    virtual bool IsConnected() const { return m_connected; }

protected:
    bool m_connected = false;
};

#endif // COMMON_INTERFACES_H
