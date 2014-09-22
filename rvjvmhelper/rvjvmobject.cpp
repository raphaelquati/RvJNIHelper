#include "rvjvmobject.h"
#include "rvjvm.h"
#include "rvjvmthreadscope.h"

#include <jni.h>


RvJVMObject::RvJVMObject(RvJVM *jvm, void *object, bool global) : isGlobal(global)
{
    //qjvm = const_cast<QJVM *>(jvm);
    qjvm = jvm;
    jobj = (jobject) object;
}

RvJVMObject::RvJVMObject(const RvJVMObject &other)
    : isGlobal(other.isGlobal)
{
    qjvm = other.qjvm;
    jobj = (jobject) other.jobj;
}

RvJVMObject::~RvJVMObject()
{
    if (jobj) {
        deleteJNIObject();
    }
}

void *RvJVMObject::callMethod(const QString name, const QString sign, ...)
{
    va_list params;
    va_start(params, sign);

    void * ret = vcallMethod(name, sign, params);

    va_end(params);

    return ret;
}

bool RvJVMObject::isValid()
{
    return (jobj != 0); //TODO: Is it correct?
}


//QMap<QString, jvalue *> decodeAndTranslateParams(QString sign, va_list params) {
//    QString csign = sign.replace('.', '/');

//    bool lastToken = false;
//    jvalue *parms;
//    int n;

//    if (sign[0] != '(') {
//        return NULL;
//    }
//    QList<jvalue *> values;

//    for (n = 1; n < sign.length(); n++) {
//        if (sign[n] == 'Z') list << "jbyte";
//        if (sign[n] == 'B') list << "jbyte";
//        if (sign[n] == 'C') list << "jchar";
//        if (sign[n] == 'S') list << "jshort";
//        if (sign[n] == 'I') list << "jint";
//        if (sign[n] == 'J') list << "jlong";
//        if (sign[n] == 'F') list << "jfloat";
//        if (sign[n] == 'D') list << "jdouble";
//        if (sign[n] == 'V')
//            list << "VOID";

//        if (csign[n] == 'L') {
//            int found;
//            found = sign.indexOf(";", n);

//            if (found != -1)
//            {
//                parameterName = sign.mid(n + 1, found - n - 1).replace('.', '/');
//                list << parameterName;
//                n = found;
//            } else {
//                return NULL;
//            }
//        }

//        if (lastToken)
//            break;

//        if (csign[n] == ')') {
//            lastToken = true;
//        }
//    }

//    if (list.length() > 1) {
//        parms = new jvalue[list.length() - 1];
//    } else {
//        parms = NULL;
//    }


//    bool bbool;
//    char bbyte;
//    short bshort;
//    long bint; // Não é portável isso!!!
//    __int64 blong; // Não é portável isso!!!
//    float bfloat;
//    double bdouble;
//    char *bstr = NULL;
//    jobject obj = NULL;


//    for (n = 0; n < (int) list.length()-1; n++) {
//        memset(&parms[n], 0, sizeof(jvalue));

//        if (list[n] == "jboolean") {
//            bbool = va_arg(params, bool);
//            parms[n].z = (jboolean) bbool;
//        } else if (list[n] == "jbyte") {
//            bbyte = va_arg(params, char);
//            parms[n].b = (jbyte) bbyte;
//        } else if (list[n] == "jchar") {
//            bshort = va_arg(params, short);
//            parms[n].c = (jchar) bshort;
//        } else if (list[n] == "jshort") {
//            bshort = va_arg(params, char);
//            parms[n].s = (jshort) bshort;
//        } else if (list[n] == "jint") {
//            bint = va_arg(params, long);
//            parms[n].i = (jint) bint;
//        } else if (list[n] == "jlong") {
//            blong = va_arg(params, long);
//            parms[n].j = (jlong) blong;
//        } else if (list[n] == "jfloat") {
//            bfloat = va_arg(params, float);
//            parms[n].f = (jfloat) bfloat;
//        } else if (list[n] == "jdouble") {
//            bdouble = va_arg(params, double);
//            parms[n].d = (jdouble) bdouble;
//        } else if (list[n] == "java/lang/String") {
//            //Classes: tipo especial para Strings!!!!
//            bstr = _strdup(va_arg(params, char *));

//            jstring jstr;
//            jstr = jenv->NewStringUTF(bstr);

//            parms[n].l = jstr;
//        } else {
//            obj = va_arg(params, jobject);
//            parms[n].l = obj;
//        }
//    }

//    //return new QMap<QString, jvalue *>()

//    //return QMap();
//}

//void * QJVMObject::vcallMethod2(const QString name,  QString sign, va_list params) {
//    QJVMThreadScope scope(qjvm);
//    JNIEnv *jenv = scope.GetEnv();

//    QList<QMap> map = decodeAndTranslateParams(sign, params);
//}



void * RvJVMObject::vcallMethod(const QString name,  QString sign, va_list params)
{
    /*
    Type Signature				Java Type
    Z							boolean
    B							byte
    C							char
    S							short
    I							int
    J							long
    F							float
    D							double
    L fully-qualified-class ;	fully-qualified-class
    [ type						type[]
    ( arg-types ) ret-type		method type
    */

    RvJVMThreadScope scope(qjvm);
    JNIEnv *jenv = scope.GetEnv();


    QString csign = sign.replace('.', '/');

    QList<QString> list;

    QString parameterName;

    bool lastToken = false;
    jvalue *parms;
    int n;

    if (sign[0] != '(') {
        return NULL;
    }

    for (n = 1; n < csign.length(); n++) {
        if (csign[n] == 'Z') list << "jbyte";
        if (csign[n] == 'B') list << "jbyte";
        if (csign[n] == 'C') list << "jchar";
        if (csign[n] == 'S') list << "jshort";
        if (csign[n] == 'I') list << "jint";
        if (csign[n] == 'J') list << "jlong";
        if (csign[n] == 'F') list << "jfloat";
        if (csign[n] == 'D') list << "jdouble";
        if (csign[n] == 'V')
            list << "VOID";

        if (csign[n] == 'L') {
            int found;
            found = csign.indexOf(";", n);

            if (found != -1)
            {
                parameterName = csign.mid(n + 1, found - n - 1).replace('.', '/');
                list << parameterName;
                n = found;
            } else {
                return NULL;
            }
        }

        if (lastToken)
            break;

        if (csign[n] == ')') {
            lastToken = true;
        }
    }

    if (list.length() > 1) {
        parms = new jvalue[list.length() - 1];
    } else {
        parms = NULL;
    }

    bool bbool;
    char bbyte;
    short bshort;
    long bint; // Não é portável isso!!!
    __int64 blong; // Não é portável isso!!!
    float bfloat;
    double bdouble;
    char *bstr = NULL;
    jobject obj = NULL;

    for (n = 0; n < (int) list.length()-1; n++) {
        memset(&parms[n], 0, sizeof(jvalue));

        if (list[n] == "jboolean") {
            bbool = va_arg(params, bool);
            parms[n].z = (jboolean) bbool;
        } else if (list[n] == "jbyte") {
            bbyte = va_arg(params, char);
            parms[n].b = (jbyte) bbyte;
        } else if (list[n] == "jchar") {
            bshort = va_arg(params, short);
            parms[n].c = (jchar) bshort;
        } else if (list[n] == "jshort") {
            bshort = va_arg(params, char);
            parms[n].s = (jshort) bshort;
        } else if (list[n] == "jint") {
            bint = va_arg(params, long);
            parms[n].i = (jint) bint;
        } else if (list[n] == "jlong") {
            blong = va_arg(params, long);
            parms[n].j = (jlong) blong;
        } else if (list[n] == "jfloat") {
            bfloat = va_arg(params, float);
            parms[n].f = (jfloat) bfloat;
        } else if (list[n] == "jdouble") {
            bdouble = va_arg(params, double);
            parms[n].d = (jdouble) bdouble;
        } else if (list[n] == "java/lang/String") {
            //Classes: tipo especial para Strings!!!!
            bstr = _strdup(va_arg(params, char *));

            jstring jstr;
            jstr = jenv->NewStringUTF(bstr);

            parms[n].l = jstr;
        } else {
            obj = va_arg(params, jobject);
            parms[n].l = obj;
        }
    }

    jclass clas;
    jmethodID mid;

    clas = jenv->GetObjectClass(jobj);
    if (qjvm->CheckException(jenv))
    {
        if (bstr) free(bstr);
        if (parms) delete[] parms;
        return NULL;
    }

    if (!clas)
    {
        if (bstr) free(bstr);
        if (parms) delete[] parms;
        return NULL;
    }

    mid = jenv->GetMethodID(clas, name.toLatin1().constData(), csign.toLatin1().constData());
    if (!mid)
    {
        if (bstr) free(bstr);
        if (parms) delete[] parms;
        return NULL;
    }

    //Vamos chamar!
    int ix = list.length() - 1;
    jstring jsret;
    char *sRET;
    void *vret = NULL;

    if (list[ix] == "java/lang/String") {
        jsret = (jstring) jenv->CallObjectMethodA(jobj, mid, parms);

        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            sRET = (char *) jenv->GetStringUTFChars(jsret, 0);
            vret = _strdup(sRET);
            jenv->ReleaseStringUTFChars(jsret, sRET);
        }
    } else if (list[ix] == "jboolean") {
        bbool = ( jenv->CallBooleanMethodA(jobj, mid, parms) ? true : false );
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            bool *bool_vret = new bool;
            *bool_vret = bbool;

            vret = bool_vret;
        }
    } else if (list[ix] == "jbyte") {
        bbyte = jenv->CallByteMethodA(jobj, mid, parms);
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            char *char_vret = new char[1];
            *char_vret = bbyte;

            vret = char_vret;
        }
    } else if (list[ix] == "jchar") {
        bshort = jenv->CallCharMethodA(jobj, mid, parms);
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            short *short_vret = new short;
            *short_vret = bbyte;

            vret = short_vret;
        }
    } else if (list[ix] == "jshort") {
        bshort = jenv->CallShortMethodA(jobj, mid, parms);
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            short *short_vret = new short;
            *short_vret = bbyte;

            vret = short_vret;
        }
    } else if (list[ix] == "jint") {
        bint = jenv->CallIntMethodA(jobj, mid, parms);
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            long *long_vret = new long;
            *long_vret = bint;

            vret = long_vret;
        }
    } else if (list[ix] == "jlong") {
        blong = jenv->CallIntMethodA(jobj, mid, parms);
        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            __int64 *_long_vret = new __int64;
            *_long_vret = blong;

            vret = _long_vret;
        }
    } else if (list[ix] == "jfloat") {
        bfloat = jenv->CallFloatMethodA(jobj, mid, parms);

        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            float *float_vret = new float;
            *float_vret = bfloat;

            vret = float_vret;
        }
    } else if (list[ix] == "jdouble") {
        bdouble = jenv->CallDoubleMethodA(jobj, mid, parms);

        if (qjvm->CheckException(jenv)) {
            vret = NULL;
        } else {
            double *double_vret = new double;
            *double_vret = bdouble;

            vret = double_vret;
        }
    } else if (list[ix] == "VOID") {
        jenv->CallVoidMethodA(jobj, mid, parms);

        if (qjvm->CheckException(jenv)) vret = NULL;

        vret = NULL;
    } else {
        vret = jenv->CallObjectMethodA(jobj, mid, parms);

        if (qjvm->CheckException(jenv)) vret = NULL;
    }

    if (bstr) free(bstr);
    if (parms) delete[] parms;

    return vret;
}

void RvJVMObject::deleteJNIObject()
{
    RvJVMThreadScope scope(qjvm);
    JNIEnv *jenv = scope.GetEnv();

    if ( jobj )
    {
        if ( isGlobal )
        {
            jenv->DeleteGlobalRef(jobj);
        }
        else
        {
            jenv->DeleteLocalRef(jobj);
        }
    }
}

