#include "rvjvm.h"

#ifndef QJVMTHREADSCOPE_H
#define QJVMTHREADSCOPE_H

class RvJVMThreadScope
{
public:
    explicit RvJVMThreadScope(RvJVM *jvm);
    virtual ~RvJVMThreadScope();

    JNIEnv* GetEnv();
private:
    RvJVM* jvm;
    JNIEnv* jenv;
    bool attached;
};

#endif // QJVMTHREADSCOPE_H
