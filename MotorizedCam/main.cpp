#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QtCore>
#include <wiringPiSPI.h>
#include "filter.h"
#include "gpio.h"
#include "stepper.h"

#define SPI_CHANNEL 0

static void isrBusy1();
static void isrBusy2();


static GPIO stp_busy1(3, GPIO_INPUT, isrBusy1, INT_EDGE_BOTH);      // Input Pin for Stepper 1 Busy
static GPIO stp_busy2(25, GPIO_INPUT, isrBusy2, INT_EDGE_BOTH);     // Input Pin for Stepper 2 Busy

static void isrBusy1()
{
    stp_busy1.isrCallback();
}

static void isrBusy2()
{
    stp_busy2.isrCallback();
}


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    wiringPiSPISetupMode(0, 500000, 3);
    wiringPiSPISetupMode(1, 500000, 3);

    GPIO stp_stpClk1(0, OUTPUT);
    GPIO stp_stpClk2(23, OUTPUT);
    GPIO stp_cs1(10, OUTPUT);
    GPIO stp_cs2(11, OUTPUT);

    Stepper stepper1(&stp_cs1, &stp_stpClk1, 0, "stepper1", &stp_busy1);
    Stepper stepper2(&stp_cs2, &stp_stpClk2, 1, "stepper2", &stp_busy2);

    QObject::connect(&stp_busy1, SIGNAL(inputChanged(int)), &stepper1, SLOT(cmdCompleted()));
    QObject::connect(&stp_busy2, SIGNAL(inputChanged(int)), &stepper2, SLOT(cmdCompleted()));


    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("appDir", QGuiApplication::applicationDirPath());
    ctx->setContextProperty("stepperX", &stepper1);
    ctx->setContextProperty("stepperY", &stepper2);

    qmlRegisterType<Filter>("test.opencv.qt", 1, 0, "CVFilter");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);



    bool rv =  app.exec();

    return rv;
}

