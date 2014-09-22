#include "rvjvm.h"
#include "rvjvmobject.h"

RvJVM::RvJVM(const QString & p_classpath ) :
    m_classpath(p_classpath),
    m_debug( JNI_FALSE ),
    m_CreateJavaVM( nullptr ),
    m_GetDefaultJavaVMInitArgs( nullptr ),
    m_numJVMOptions( 0 ),
    m_env( nullptr ),
    m_jvm( nullptr )
{

    char *l_jvmtype = 0;
    char l_jrepath[MAXPATHLEN], l_jvmpath[MAXPATHLEN];

    if ( !GetJREPath( l_jrepath, sizeof(l_jrepath ) ) ) {
        return;
    }

    l_jvmpath[0] = '\0';
    if (!GetJVMPath( l_jrepath, "client", l_jvmpath, sizeof( l_jvmpath ) ) ) {
        if (!GetJVMPath( l_jrepath, "server", l_jvmpath, sizeof( l_jvmpath ) ) ) {
            fprintf(stderr, "Error: no `%s' JVM at `%s'.\n", l_jvmtype, l_jvmpath);
            return;
        }
    }

    m_CreateJavaVM = 0;
    m_GetDefaultJavaVMInitArgs = 0;

    if ( !LoadJavaVM( l_jvmpath ) ) {
        return;
    }

    char l_Mclasspath[8192];

    sprintf_s(l_Mclasspath, sizeof(l_Mclasspath), "-Djava.class.path=.;%s;", m_classpath.toLocal8Bit().constData());
    m_JVMoptions[0].optionString = _strdup(l_Mclasspath);
    m_JVMoptions[1].optionString = _strdup("-Xrs");
#ifndef _DEBUG
    m_numJVMOptions = 2;
#else
    m_JVMoptions[2].optionString = _strdup("-Xdebug");
    m_JVMoptions[3].optionString = _strdup("-Xnoagent");
    m_JVMoptions[4].optionString = _strdup("-Djava.compiler=none");
    m_JVMoptions[5].optionString = _strdup("-Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=1242");
    m_numJVMOptions = 6;
#endif

    if ( !InitializeJVM( &m_jvm, &m_env ) ) {
        fprintf(stderr, "Could not create the Java virtual machine.\n");
        return;
    }
}

RvJVM::~RvJVM()
{
    DestroyJVM();

    nativeLibrary->unload();
    delete nativeLibrary;
}


jboolean RvJVM::GetJREPath(char *p_path, jint p_pathsize)
{
    /* Look for a public JRE on this machine. */
    if (GetPublicJREHome(p_path, p_pathsize)) {
        return JNI_TRUE;
    }

    fprintf(stderr, "Error: could not find %s\n", JVM_DLL);
    return JNI_FALSE;
}

jboolean RvJVM::GetPublicJREHome(char *buf, jint bufsize) {
    char version[MAXPATHLEN];

#ifdef WINDOWS
    HKEY key, subkey;
    /* Find the current version of the JRE */
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, JRE_KEY, 0, KEY_READ, &key) != 0) {
        fprintf(stderr, "Error opening registry key '%s'\n", JRE_KEY);
        return JNI_FALSE;
    }

    if (!GetStringFromRegistry(key, "CurrentVersion", version, sizeof(version))) {
        fprintf(stderr, "Failed reading value of registry key:\n\t%s\\CurrentVersion\n", JRE_KEY);
        RegCloseKey(key);
        return JNI_FALSE;
    }

    /* Find directory where the current version is installed. */
    if (RegOpenKeyExA(key, version, 0, KEY_READ, &subkey) != 0) {
        fprintf(stderr, "Error opening registry key '%s\\%s'\n", JRE_KEY, version);
        RegCloseKey(key);
        return JNI_FALSE;
    }

    if (!GetStringFromRegistry(subkey, "JavaHome", buf, bufsize)) {
        fprintf(stderr, "Failed reading value of registry key:\n\t%s\\%s\\JavaHome\n", JRE_KEY, version);
        RegCloseKey(key);
        RegCloseKey(subkey);
        return JNI_FALSE;
    }

    RegCloseKey(key);
    RegCloseKey(subkey);
#else
#endif
    if (qEnvironmentVariableIsEmpty("JAVA_HOME")) {
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start("java -XshowSettings:properties");

        QByteArray data;

        if(!process.waitForStarted())
            return JNI_FALSE;

        while(process.waitForReadyRead())
            data.append(process.readAll());

        QString output(data.data());
        qDebug() << output;

        bool found = false;

        QStringList list = output.split("\r\n");
        foreach (QString token, list) {
            QString trim = token.trimmed();

            if (trim.startsWith("sun.boot.library.path")) {         //java.home
                QString val = trim.mid(trim.indexOf("=") + 1).trimmed();
                strncpy_s(buf, bufsize, val.toLatin1().constData(),  val.size());
                found = true;
                break;
            }
        }

        if (!found) {
            return JNI_FALSE;
        }
    } else {
        //TODO: test beacause Windows && Linux are different.
        QString path(qgetenv("JAVA_HOME"));
        strncpy_s(buf, bufsize, path.toLatin1().constData(),  path.size());
    }

    return JNI_TRUE;
}

jboolean RvJVM::GetStringFromRegistry(HKEY key, const char *name, char *buf, jint bufsize)
{

#ifdef WINDOWS
    DWORD type, size;

    if (RegQueryValueExA(key, name, 0, &type, (LPBYTE) 0, &size) == 0
            && type == REG_SZ
            && (size < (unsigned int)bufsize)) {
        if (RegQueryValueExA(key, name, 0, 0, (LPBYTE) buf, &size) == 0) {
            return JNI_TRUE;
        }
    }
#endif

    return JNI_FALSE;
}

jboolean RvJVM::GetJVMPath(const char *jrepath, char* jvmtype, char *jvmpath, jint jvmpathsize) {
#ifndef WINDOWS
    sprintf_s(jvmpath, jvmpathsize, "%s\\bin\\%s\\%s", jrepath, jvmtype, JVM_DLL);
#else
    sprintf_s(jvmpath, jvmpathsize, "%s\\%s\\%s", jrepath, jvmtype, JVM_DLL);
#endif

    if (QFile::exists(QString(jvmpath)))
    {
        return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

jboolean RvJVM::LoadJavaVM(const char *jvmpath)
{
    m_CreateJavaVM = 0;
    m_GetDefaultJavaVMInitArgs = 0;
    nativeLibrary = new QLibrary(jvmpath);
    if (nativeLibrary->load()) {
        m_CreateJavaVM = (CreateJavaVM_t) nativeLibrary->resolve("JNI_CreateJavaVM");
        m_GetDefaultJavaVMInitArgs = (GetDefaultJavaVMInitArgs_t) nativeLibrary->resolve("JNI_GetDefaultJavaVMInitArgs");
    }

    if (m_CreateJavaVM == 0 || m_GetDefaultJavaVMInitArgs == 0) {
        fprintf(stderr, "Error: can't find JNI interfaces in: %s\n", jvmpath);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

jboolean RvJVM::InitializeJVM(JavaVM **pvm, JNIEnv **penv) {
    JavaVMInitArgs args;
    jint r;

    memset(&args, 0, sizeof(args));
    args.version  = JNI_VERSION_1_2;
    args.nOptions = m_numJVMOptions;
    args.options  = m_JVMoptions;
    args.ignoreUnrecognized = JNI_FALSE;

    r = m_CreateJavaVM(pvm, (void **)penv, &args);

    return r == JNI_OK;
}

bool RvJVM::CheckException(JNIEnv *penv) {
    return CheckException(penv, false, false);
}

bool RvJVM::CheckException(JNIEnv *penv, bool letThrowException) {
    return CheckException(penv, letThrowException, false);
}


bool RvJVM::CheckException(JNIEnv *penv, bool letThrowException, bool getDescription)
{
    RvJVMObject exception(this, penv->ExceptionOccurred());

    //QJVMObject *exception = new QJVMObject(this, jenv->ExceptionOccurred());


    if (exception.isValid()) {
        penv->ExceptionDescribe();

        if (!letThrowException) {
            penv->ExceptionClear();
        }

        if (getDescription) {
            char *toString = (char *) exception.callMethod("toString", "()Ljava.lang.String;"); //TODO: Check recursive

            if (toString) {
                lastException.clear();
                lastException.append(toString);
                delete toString;
            }
        }

        return true;
    }

    return false;
}

JNIEnv *RvJVM::GetEnv()
{
    return m_env;
}

QString RvJVM::getLastExceptionString()
{
    return lastException;
}

void RvJVM::DestroyJVM()
{
    if (m_jvm != NULL)
    {
        m_jvm->DetachCurrentThread();
        m_jvm->DestroyJavaVM();
    }
}

JavaVM* RvJVM::VM()
{
    return m_jvm;
}

void RvJVM::detachCurrentThread()
{
    m_jvm->DetachCurrentThread();
}


JNIEnv * RvJVM::attachCurrentThread(bool *bret)
{
   *bret = false;

   JNIEnv *env;
   jint ret;

   ret = m_jvm->GetEnv((void **) &env, JNI_VERSION_1_2);
   if (ret == JNI_EDETACHED) {
       m_jvm->AttachCurrentThread((void **) &env, NULL);
       *bret = true;
   }

   return env;
}

