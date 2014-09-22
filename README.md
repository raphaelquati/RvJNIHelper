RvJNIHelper
===========

Helper classes to load java classes in an Qt project thru JNI.

This helper was created a long time ago with pure C++. I started to port this to Qt.

The idea is to make java calls easily in Qt:

    RvJVMHelper* h = new RvJVMHelper(QString("."));

    RvJVMObject *obj = h->createObject("Test");
    obj->callMethod("m"); //void method. No parameters
    
    int *ret = (int *) obj->callMethod("sum", "(II)I", 10, 20); 
    
    qDebug() << (QString("%1").arg(*ret));
    delete ret;

    obj->callMethod("printName", "(Ljava.lang.String;)V", "Teste de nova String via C++");

    char *str = (char *) obj->callMethod("returnName", "(I)Ljava.lang.String;", 10);

    qDebug() << str;

    delete str;


At signature names, I've used different approach (using java.lang.String instead of java/lang/String) to make calls more close to Java use.

Qt has a module named QAndroidExtras. But it's no compiled for Desktop andit's using Qt private headers (qjni.h).

