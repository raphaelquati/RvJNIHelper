#include "rvjvmthreadscope.h"

RvJVMThreadScope::RvJVMThreadScope(RvJVM *vm) :
    jvm(vm)
{
    jenv = jvm->attachCurrentThread(&attached);
}

RvJVMThreadScope::~RvJVMThreadScope()
{
    if (attached) {
        jvm->detachCurrentThread();
    }
}

JNIEnv *RvJVMThreadScope::GetEnv()
{
    return jenv;

}
