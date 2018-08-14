#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "tester.h"

class hwi {
public:
    hwi();
    ~hwi();
    static Tester* tester;

private:
    QThread thread;
    QSemaphore semafore;
};

#endif // MEASURINGINTERFACE_H
