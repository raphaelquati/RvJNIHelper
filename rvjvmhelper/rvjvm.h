#ifndef QJVM_H
#define QJVM_H

#include <jni.h>

#ifdef WIN32
 #define WINDOWS
#elif WIN64
 #define WINDOWS
#endif
#ifdef WINDOWS
  #include <windows.h>
#endif

#include <QtCore>
#include <QLibrary>

#define PATH_SEPARATOR	';'
#define FILE_SEPARATOR	'\\'
#define MAXPATHLEN      MAX_PATH

#define JVM_CFG "\\lib\\jvm.cfg"

#define DOTRELEASE  "1.3" /* Same for 1.3.1, 1.3.2 etc. */
#define JRE_KEY	    "Software\\JavaSoft\\Java Runtime Environment"
#define JDK_KEY	    "Software\\JavaSoft\\JDK"


#ifdef WINDOWS
    #ifdef DEBUG
    #define JVM_DLL "jvm_g.dll"
    #define JAVA_DLL "java_g.dll"
    #else
    #define JVM_DLL "jvm.dll"
    #define JAVA_DLL "java.dll"
    #endif
#else
    #define JVM_DLL "libjvm.so"
    #define JAVA_DLL "libjava.so"

#endif


typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **env, void *args);
typedef jint (JNICALL *GetDefaultJavaVMInitArgs_t)(void *args);

class RvJVM
{
public:
    RvJVM(const QString & p_classpath);
    virtual ~RvJVM();

    JavaVM* VM();

    void detachCurrentThread();
    JNIEnv * attachCurrentThread(bool *bret);

    bool CheckException(JNIEnv *penv);
    bool CheckException(JNIEnv *penv, bool letThrowException);
    bool CheckException(JNIEnv *penv, bool letThrowException, bool getDescription);

    JNIEnv *GetEnv();
    QString getLastExceptionString();

private:
    jboolean GetJREPath(char *p_path, jint p_pathsize);
    jboolean GetPublicJREHome(char *buf, jint bufsize);
    jboolean GetStringFromRegistry(HKEY key, const char *name, char *buf, jint bufsize);
    jboolean GetJVMPath(const char *jrepath, char* jvmtype, char *jvmpath, jint jvmpathsize);
    jboolean LoadJavaVM(const char *jvmpath);
    jboolean InitializeJVM(JavaVM **pvm, JNIEnv **penv);

    void DestroyJVM();

    QString lastException;
    QString m_classpath;
    jboolean m_debug;

    CreateJavaVM_t m_CreateJavaVM;
    GetDefaultJavaVMInitArgs_t m_GetDefaultJavaVMInitArgs;

    JavaVMOption *m_JVMoptions;
    int m_numJVMOptions;
    JNIEnv *m_env;
    JavaVM *m_jvm;
    QLibrary *nativeLibrary;
};

#endif // QJVMLOADER_H
