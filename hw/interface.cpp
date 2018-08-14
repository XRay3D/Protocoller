#include "interface.h"

Tester* hwi::tester = nullptr;

hwi::hwi()
{
    if (!semafore.available()) {
        tester = new Tester;
        tester->moveToThread(&thread);
        thread.connect(&thread, &QThread::finished, tester, &QObject::deleteLater);
        thread.start(QThread::NormalPriority);
    }
    semafore.release();
}

hwi::~hwi()
{
    semafore.acquire();
    if (!semafore.available()) {
        thread.quit();
        thread.wait();
        tester = nullptr;
    }
}
