#ifndef GPIO_H
#define GPIO_H

#include <QObject>
#include "wiringPi.h"
#include "softPwm.h"

#define GPIO_INPUT   0
#define GPIO_OUTPUT  1
#define GPIO_PWM     2

class GPIO : public QObject
{
    Q_OBJECT
public:
    explicit GPIO(int pin, int type, void (*isrInput)(void) = nullptr, int inputmode = INT_EDGE_BOTH,  QObject *parent = nullptr);
    void isrCallback();
    int readInput();

private:
    int m_toggleStatus;
    int m_pin;
    int m_pwmValue;
    int m_inputMode;

signals:
    void inputChanged(int value);

public slots:
    void pinHigh();
    void pinLow();
    void pinToggle();
    void setPwmValue(int);
};

#endif // GPIO_H
