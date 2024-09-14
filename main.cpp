
#include"ui/UartConfigWidget/eegstart.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EEGStart EEG_Start;

    EEG_Start.show();
    //
    return a.exec();
}
