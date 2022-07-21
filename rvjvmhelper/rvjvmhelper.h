#ifndef QJVMHELPER_H
#define QJVMHELPER_H

#include "rvjvmhelper_global.h"
#include <QtCore>

#include "rvjvmobject.h"


class QJVMHELPERSHARED_EXPORT RvJVMHelper
{
    
public:
    RvJVMHelper(const QString & classpath);
    RvJVMHelper(RvJVMHelper * vm);

    virtual ~RvJVMHelper();

    void deleteObject(void *jobj, bool isGlobal = true);
    RvJVMObject * createObject(QString name, QString sign = "()V", ...);
    RvJVMObject * createGlobalObject(QString name, QString sign, ...);

private:
    void *_createObject(QString name, QString sign, va_list args);

};

#endif // QJVMHELPER_H
