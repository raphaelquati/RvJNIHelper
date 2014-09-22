#ifndef QJVMOBJECT_H
#define QJVMOBJECT_H

#include "rvjvmhelper_global.h"
#include <QObject>

class RvJVM;
class _jobject;

class QJVMHELPERSHARED_EXPORT RvJVMObject
{
public:
    RvJVMObject(RvJVM *jvm, void *object, bool global = false);
    RvJVMObject( const RvJVMObject & other );

    virtual ~RvJVMObject();

    void * callMethod(const QString name, const QString sign = "()V", ...);
//    void * createString(const QString & value);

    bool isValid();

private:
    bool isGlobal;
    void * vcallMethod(const QString name,  QString sign, va_list params);
    void * vcallMethod2(const QString name,  QString sign, va_list params);
    void deleteJNIObject();

    _jobject *jobj;
    RvJVM *qjvm;

    //    void * getFieldID(void * object, const QString & name, const QString & sign, JNIEnv *jenv);
    //QList<QMap> decodeAndTranslateParams(const QString sign, va_list params);
};

#endif // QJVMOBJECT_H
