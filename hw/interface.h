#pragma once

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
