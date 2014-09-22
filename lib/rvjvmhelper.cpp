#include "rvjvmhelper.h"
#include "rvjvm.h"
#include "rvjvmthreadscope.h"

RvJVM *jvm;

RvJVMHelper::RvJVMHelper(const QString & classpath)
{
    jvm = new RvJVM(classpath);
    jvm->VM();
}

RvJVMHelper::~RvJVMHelper()
{
    delete jvm;
}

void *RvJVMHelper::_createObject(QString name, QString sign, va_list args)
{
    JNIEnv *jenv;
    jobject local_jobj = 0;

    RvJVMThreadScope scope(jvm);
    jenv = scope.GetEnv();

    jclass clazz = jenv->FindClass(name.replace('.', '/').toLatin1().constData());

    if (jvm->CheckException(jenv) || clazz == 0)
    {
        return 0;
    }

    jmethodID method = jenv->GetMethodID(clazz, "<init>", sign.replace('.', '/').toLatin1().constData());

    if (jvm->CheckException(jenv) || method == 0)
    {
        return 0;
    }

    local_jobj = jenv->NewObjectV(clazz, method, args);
    if (jvm->CheckException(jenv))
    {
        return 0;
    }

    va_end(args);

    return local_jobj;
}

RvJVMObject* RvJVMHelper::createObject(QString name, QString sign, ...)
{
    va_list params;
    va_start(params, sign);

    jobject obj = (jobject) _createObject(name, sign, params);

    va_end(params);

    return new RvJVMObject(jvm, obj);
}


RvJVMObject *RvJVMHelper::createGlobalObject(QString name, QString sign, ...)
{
    jobject jobj = 0;
    jobject local_jobj;

    va_list params;
    va_start(params, sign);

    local_jobj = (jobject) _createObject(name, sign, params);

    if (local_jobj)
    {
        jobj = jvm->GetEnv()->NewGlobalRef(local_jobj);
    }
    va_end(params);

    return new RvJVMObject(jvm, jobj, true);
}


