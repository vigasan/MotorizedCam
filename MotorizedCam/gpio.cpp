#include "gpio.h"

GPIO::GPIO(int pin, int type, void (*isrInput)(void), int inputmode, QObject *parent) : QObject(parent)
{
    wiringPiSetup();
    m_pin = pin;
    m_toggleStatus = 0;
    m_pwmValue = 0;
    m_inputMode = inputmode;

    switch(type)
    {
        case GPIO_INPUT:
        {
            pinMode(m_pin, INPUT);
            wiringPiISR(m_pin, m_inputMode, isrInput);
        } break;

        case GPIO_OUTPUT:
        {
            pinMode(m_pin, OUTPUT);

        } break;

        case GPIO_PWM:
        {
            softPwmCreate(m_pin, m_pwmValue, 100);
        } break;
    }
}

void GPIO::isrCallback()
{
    emit inputChanged(digitalRead(m_pin));
}

int GPIO::readInput()
{
    return digitalRead(m_pin);
}

void GPIO::pinHigh()
{

    digitalWrite(m_pin,HIGH);
}

void GPIO::pinLow()
{
    digitalWrite(m_pin,LOW);
}

void GPIO::pinToggle()
{
    m_toggleStatus ^= 1;
    digitalWrite(m_pin, m_toggleStatus);
}

void GPIO::setPwmValue(int value)
{
    softPwmWrite(m_pin, value);
}
