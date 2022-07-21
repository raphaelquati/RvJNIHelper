#include <QCoreApplication>
#include <rvjvmhelper.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RvJVMHelper* h = new RvJVMHelper(a.applicationDirPath());

    RvJVMObject *obj = h->createObject("Test");
    obj->callMethod("m");
    int *ret = (int *) obj->callMethod("sum", "(II)I", 10, 20);
    qDebug() << (QString("%1").arg(*ret));
    delete ret;

    obj->callMethod("printName", "(Ljava.lang.String;)V", "Teste de nova String via C++");

    char *str = (char *) obj->callMethod("returnName", "(I)Ljava.lang.String;", 10);

    qDebug() << str;

    delete str;

    obj->callMethod("makeException");

    delete obj;

    delete h;

    a.exit(0);

    return 0;
}
