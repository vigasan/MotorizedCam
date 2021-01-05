#include "stepper.h"
#include <QGuiApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <wiringPiSPI.h>

Stepper::Stepper(GPIO *cs, GPIO *stpclk, int spiChannel, QString fileParams, GPIO *busyPin, QObject *parent) : QObject(parent)
{
    m_cs = cs;
    m_stpClk = stpclk;
    m_StepperBusy = false;


    m_channel = spiChannel;
    m_busy = busyPin;
    m_cs->pinHigh();

    m_jsonFileName = QCoreApplication::applicationDirPath() + "/" + fileParams;

    QObject::connect(m_busy, SIGNAL(inputChanged(int)), this, SLOT(cmdCompleted()));

    qDebug()<< m_jsonFileName;

    m_StepMode = DEFLT_STEP_MODE;
    m_ThresholdSpeed = DEFLT_THSPEED;
    m_MaxSpeed = DEFLT_MAX_SPEED;
    m_MinSpeed = DEFLT_MIN_SPEED;
    m_Acceleration = DEFLT_ACC;
    m_Deceleration = DEFLT_DEC;
    m_KValHold = DEFLT_KV_HOLD;
    m_KValRun = DEFLT_KV_RUN;
    m_KValAcc = DEFLT_KV_ACC;
    m_KValDec = DEFLT_KV_DEC;
    m_KTherm = DEFLT_K_THERM;
    m_OcdThreshold = DEFLT_OCD_TH;
    m_StallThreshold = DEFLT_STALL_TH;


    QFile jsonFile(m_jsonFileName);
    if(jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<< "Read Stepper Parameters";
        QTextStream file_text(&jsonFile);
        QString json_string = file_text.readAll();
        jsonFile.close();

        QByteArray json_bytes = json_string.toLocal8Bit();
        QJsonDocument json_doc = QJsonDocument::fromJson(json_bytes);

        if(!json_doc.isEmpty())
        {
            if(json_doc.isObject())
            {
                m_jsonObj = json_doc.object();

                if(!m_jsonObj.isEmpty())
                {
                    QVariantMap json_map = m_jsonObj.toVariantMap();

                    // Reading Key/value
                    m_StepMode = quint8(json_map[KEY_STEP_MODE].toUInt());
                    m_ThresholdSpeed = quint16(json_map[KEY_THRESHOLD_FULL_SPEED].toUInt());
                    m_MaxSpeed = quint16(json_map[KEY_MAX_SPEED].toUInt());
                    m_MinSpeed = quint16(json_map[KEY_MIN_SPEED].toUInt());
                    m_Acceleration = quint16(json_map[KEY_ACCELERATION].toUInt());
                    m_Deceleration = quint16(json_map[KEY_DECELERATION].toUInt());
                    m_KValHold = quint8(json_map[KEY_KVAL_HOLD].toUInt());
                    m_KValRun = quint8(json_map[KEY_KVAL_RUN].toUInt());
                    m_KValAcc = quint8(json_map[KEY_KVAL_ACC].toUInt());
                    m_KValDec = quint8(json_map[KEY_KVAL_DEC].toUInt());
                    m_KTherm = quint8(json_map[KEY_K_THERMIC].toUInt());
                    m_OcdThreshold = quint8(json_map[KEY_OCD_THRESHOLD].toUInt());
                    m_StallThreshold = quint8(json_map[KEY_STALL_THRESHOLD].toUInt());
                }
            }
        }
    } else
    {
       setDefaultParameters();
    }
    sendParameters();
}

void Stepper::updateParameters()
{
    emit stepModeChanged();
    emit thresholdSpeedChanged();
    emit maxSpeedChanged();
    emit minSpeedChanged();
    emit accelerationChanged();
    emit decelerationChanged();
    emit kValHoldChanged();
    emit kValRunChanged();
    emit kValAccChanged();
    emit kValDecChanged();
    emit kThermChanged();
    emit ocdThresholdChanged();
    emit stallThresholdChanged();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Public Slots
/////////////////////////////////////////////////////////////////////////////////////////////////////
void Stepper::setDefaultParameters()
{
    m_StepMode = DEFLT_STEP_MODE;
    m_ThresholdSpeed = DEFLT_THSPEED;
    m_MaxSpeed = DEFLT_MAX_SPEED;
    m_MinSpeed = DEFLT_MIN_SPEED;
    m_Acceleration = DEFLT_ACC;
    m_Deceleration = DEFLT_DEC;
    m_KValHold = DEFLT_KV_HOLD;
    m_KValRun = DEFLT_KV_RUN;
    m_KValAcc = DEFLT_KV_ACC;
    m_KValDec = DEFLT_KV_DEC;
    m_KTherm = DEFLT_K_THERM;
    m_OcdThreshold = DEFLT_OCD_TH;
    m_StallThreshold = DEFLT_STALL_TH;

    saveParameters();
}

void Stepper::sendParameters()
{
    setStepMode(m_StepMode);
    setFullStepSpeed(m_ThresholdSpeed);
    setMaxSpeed(m_MaxSpeed);
    setMinSpeed(m_MinSpeed);
    setAcceleration(m_Acceleration);
    setDeceleration(m_Deceleration);
    setKvalHold(m_KValHold);
    setKvalRun(m_KValRun);
    setKvalAcc(m_KValAcc);
    setKvalDec(m_KValDec);
    setKTherm(m_KTherm);
    setOCDThreshold(m_OcdThreshold);
    setStallThreshold(m_StallThreshold);
}

void Stepper::cmdCompleted()
{
    if(m_StepperBusy)
    {
        if(m_busy->readInput() == 1)
        {
            //qDebug()<< "Commando eseguito " + m_jsonFileName;
            m_StepperBusy = false;
            emit positionChanged();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: Position
/////////////////////////////////////////////////////////////////////////////////////////////////////
qint32 Stepper::Position()
{
    m_Position = getAbsPosition();
    return m_Position;
}


void Stepper::setPosition(qint32 position)
{
    m_Position = position;
    setAbsPosition(position);
    emit positionChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: StepMode
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::StepMode()
{
    //m_StepMode = getStepMode();
    return m_StepMode;
}

void Stepper::setStepMode(quint8 val)
{
    softHiZ();
    m_StepMode = val;
    SetByteParam(REG_STEP_MODE, val);
    //emit stepModeChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: ThresholdSpeed
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint16 Stepper::ThresholdSpeed()
{
    //m_ThresholdSpeed = getFullStepSpeed();
    return m_ThresholdSpeed;
}

void Stepper::setFullStepSpeed(quint16 thSpeed)
{
    m_ThresholdSpeed = thSpeed;
    SetWordParam(REG_FS_SPD, thSpeed);
    //emit thresholdSpeedChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: MaxSpeed
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint16 Stepper::MaxSpeed()
{
    //m_MaxSpeed = getMaxSpeed();
    return m_MaxSpeed;
}

void Stepper::setMaxSpeed(quint16 speed)
{
    m_MaxSpeed = speed;
    SetWordParam(REG_MAX_SPEED, speed);
    //emit maxSpeedChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: MinSpeed
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint16 Stepper::MinSpeed()
{
    //m_MinSpeed = getMinSpeed();
    return m_MinSpeed;
}

void Stepper::setMinSpeed(quint16 speed)
{
    m_MinSpeed = speed;
    SetWordParam(REG_MIN_SPEED, speed);
    //emit minSpeedChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: Acceleration
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint16 Stepper::Acceleration()
{
    //m_Acceleration = getAcceleration();
    return m_Acceleration;
}

void Stepper::setAcceleration(quint16 acc)
{
    m_Acceleration = acc;
    SetWordParam(REG_ACC, acc);
    //emit accelerationChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: Deceleration
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint16 Stepper::Deceleration()
{
    //m_Deceleration = getDeceleration();
    return m_Deceleration;
}

void Stepper::setDeceleration(quint16 dec)
{
    m_Deceleration = dec;
    SetWordParam(REG_DEC, dec);
    //emit decelerationChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: KvalHold
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::KValHold()
{
    //m_KValHold = getKvalHold();
    return m_KValHold;
}

void Stepper::setKvalHold(quint8 val)
{
    m_KValHold = val;
    SetByteParam(REG_KVAL_HOLD, val);
    //emit kValHoldChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: KvalRun
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::KValRun()
{
    //m_KValRun = getKvalRun();
    return m_KValRun;
}

void Stepper::setKvalRun(quint8 val)
{
    m_KValRun = val;
    SetByteParam(REG_KVAL_RUN, val);
    //emit kValRunChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: KvalAcc
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::KValAcc()
{
    //m_KValAcc = getKvalAcc();
    return m_KValAcc;
}

void Stepper::setKvalAcc(quint8 val)
{
    m_KValAcc = val;
    SetByteParam(REG_KVAL_ACC, val);
    //emit kValAccChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: KvalDec
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::KValDec()
{
    //m_KValDec = getKvalDec();
    return m_KValDec;
}

void Stepper::setKvalDec(quint8 val)
{
    m_KValDec = val;
    SetByteParam(REG_KVAL_DEC, val);
    //emit kValDecChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: KTherm
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::KTherm()
{
    //m_KTherm = getKTherm();
    return m_KTherm;
}

void Stepper::setKTherm(quint8 value)
{
    m_KTherm = value;
    SetByteParam(REG_K_THERM, value);
    //emit kThermChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: OCDThreshold
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::OcdThreshold()
{
    //m_OcdThreshold = getOCDThreshold();
    return m_OcdThreshold;
}

void Stepper::setOCDThreshold(quint8 value)
{
    m_OcdThreshold = value;
    SetByteParam(REG_OCD_TH, value);
    //emit ocdThresholdChanged();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROPERTY: StallThreshold
/////////////////////////////////////////////////////////////////////////////////////////////////////
quint8 Stepper::StallThreshold()
{
    //m_StallThreshold = getStallThreshold();
    return m_StallThreshold;
}

void Stepper::setStallThreshold(quint8 value)
{
    m_StallThreshold = value;
    SetByteParam(REG_STALL_TH, value);
    //emit stallThresholdChanged();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Private Methods
/////////////////////////////////////////////////////////////////////////////////////////////////////
void Stepper::saveParameters()
{
    QFile jsonFile(m_jsonFileName);
    if(jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<< "Save Stepper Parameters";

        m_jsonObj[KEY_STEP_MODE] = m_StepMode;
        m_jsonObj[KEY_THRESHOLD_FULL_SPEED] = m_ThresholdSpeed;
        m_jsonObj[KEY_MAX_SPEED] = m_MaxSpeed;
        m_jsonObj[KEY_MIN_SPEED] = m_MinSpeed;
        m_jsonObj[KEY_ACCELERATION] = m_Acceleration;
        m_jsonObj[KEY_DECELERATION] = m_Deceleration;
        m_jsonObj[KEY_KVAL_HOLD] = m_KValHold;
        m_jsonObj[KEY_KVAL_RUN] = m_KValRun;
        m_jsonObj[KEY_KVAL_ACC] = m_KValAcc;
        m_jsonObj[KEY_KVAL_DEC] = m_KValDec;
        m_jsonObj[KEY_K_THERMIC] = m_KTherm;
        m_jsonObj[KEY_OCD_THRESHOLD] = m_OcdThreshold;
        m_jsonObj[KEY_STALL_THRESHOLD] = m_StallThreshold;

        jsonFile.write(QJsonDocument(m_jsonObj).toJson());
        jsonFile.close();
    } else
    {
        qDebug()<< "Can't open " + m_jsonFileName;
    }

}



void Stepper::SetByteParam(unsigned char reg, unsigned char value)
{
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &reg, 1);
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &value, 1);
    m_cs->pinHigh();
}

void Stepper::SetWordParam(quint8 reg, quint16 value)
{
    unsigned char data;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &reg, 1);
    m_cs->pinHigh();
    data = (value & 0xFF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    m_cs->pinHigh();
    data = (value & 0x00FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    m_cs->pinHigh();
}

quint8 Stepper::GetParamByte(unsigned char param)
{
    quint8 rv = 0x00;
    unsigned char dummy = NOP_COMMAND;
    unsigned char data = param | 0x20;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &dummy, 1);
    m_cs->pinHigh();
    rv = quint8(dummy);
    return rv;
}

quint16 Stepper::GetParamWord(quint8 param)
{
    quint16 rv = 0x0000;
    unsigned char data = param | 0x20;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    data = NOP_COMMAND;
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    rv = quint16(quint16(data) << 8);
    data = NOP_COMMAND;
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    rv |= quint16(data);
    m_cs->pinHigh();
    return rv;
}

qint32 Stepper::GetParamInt32S(quint8 param)
{
    quint32 rv = 0x00000000;
    unsigned char data = param | 0x20;

    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    data = NOP_COMMAND;
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    //qDebug()<< "Hi: " << data;
    rv = quint32(data) << 16;
    data = NOP_COMMAND;
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    //qDebug()<< "Me: " << data;
    rv |= quint32(data) << 8;
    data = NOP_COMMAND;
    m_cs->pinHigh();
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &data, 1);
    //qDebug()<< "Low: " << data;
    rv |= quint32(data);
    m_cs->pinHigh();
    return qint32(rv);
}

void Stepper::clock()
{
    m_stpClk->pinToggle();
}

void Stepper::run(quint8 dir, quint32 speed)
{
    quint32 data = speed & 0x000FFFFF;
    unsigned char byte = dir | 0x50;
    //m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();


    /*
    unsigned char spidata[4];

    spidata[0] = dir | 0x50;
    spidata[1] = (data & 0x00FF0000)>>16;
    spidata[2] = (data & 0x0000FF00)>>8;
    spidata[3] = (data & 0x000000FF);


    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, spidata, 4);
    m_cs->pinHigh();
    */

}

void Stepper::stepClock(quint8 dir)
{
    unsigned char byte = dir | 0x58;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::softStop()
{
    unsigned char byte = 0xB0;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::hardStop()
{
    unsigned char byte = 0xB8;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::softHiZ()
{
    unsigned char byte = 0xA0;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::hardHiZ()
{
    unsigned char byte = 0xA8;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::move(quint8 dir, quint32 step)
{
    quint32 data = step & 0x003FFFFF;
    unsigned char byte = dir | 0x40;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::goTo(qint32 position)
{
    quint32 data = position & 0x003FFFFF;
    unsigned char byte = 0x60;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::goToDir(quint8 dir, qint32 position)
{
    quint32 data = position & 0x003FFFFF;
    unsigned char byte = dir | 0x68;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::goUntil(quint8 act, quint8 dir, quint32 speed)
{
    quint32 data = speed & 0x000FFFFF;
    unsigned char byte = act | dir | 0x82;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::releaseSW(quint8 act, quint8 dir)
{
    unsigned char byte = act | dir | 0x92;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::goHome()
{
    unsigned char byte = 0x70;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::goMark()
{
    unsigned char byte = 0x78;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::resetPosition()
{
    unsigned char byte = 0xD8;
    m_StepperBusy = true;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::resetDevice()
{
    // Da implementare
}

void Stepper::setIntersectSpeed(quint16 speed)
{
    SetWordParam(REG_INT_SPD, speed);
}

void Stepper::setStartSlope(quint8 value)
{
    SetByteParam(REG_ST_SLP, value);
}

void Stepper::setAccFinalSlope(quint8 value)
{
    SetByteParam(REG_FN_SLP_ACC, value);
}

void Stepper::setDecFinalSlope(quint8 value)
{
    SetByteParam(REG_FN_SLP_DEC, value);
}

void Stepper::setConfiguration(quint16 value)
{
    SetWordParam(REG_CONFIG, value);
}

void Stepper::setMarkPosition(qint32 value)
{
    quint32 data = value & 0x003FFFFF;
    unsigned char byte = REG_MARK_POS;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

void Stepper::setAbsPosition(qint32 value)
{
    quint32 data = value & 0x003FFFFF;
    unsigned char byte = REG_ABS_POS;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x00FF0000)>>16;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x0000FF00)>>8;
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
    byte = (data & 0x000000FF);
    m_cs->pinLow();
    wiringPiSPIDataRW(m_channel, &byte, 1);
    m_cs->pinHigh();
}

qint32 Stepper::getAbsPosition()
{
    qint32 temp;
    temp = GetParamInt32S(REG_ABS_POS);
    //qDebug()<< "Position: " << temp;
    if ((temp & 0x200000) > 0)           // Si tratta di un numero negativo
    {
        temp = ~temp;
        temp = -1 * ((temp & 0x3FFFFF) + 1);
    }
    return temp;
}

quint16 Stepper::getFullStepSpeed()
{
    return GetParamWord(REG_FS_SPD);
}

quint16 Stepper::getMaxSpeed()
{
    return GetParamWord(REG_MAX_SPEED);
}

quint16 Stepper::getMinSpeed()
{
    return GetParamWord(REG_MIN_SPEED);
}

quint16 Stepper::getAcceleration()
{
    return GetParamWord(REG_ACC);
}

quint16 Stepper::getDeceleration()
{
    return GetParamWord(REG_DEC);
}

quint8 Stepper::getKvalHold()
{
    return GetParamByte(REG_KVAL_HOLD);
}

quint8 Stepper::getKvalRun()
{
    return GetParamByte(REG_KVAL_RUN);
}

quint8 Stepper::getKvalAcc()
{
    return GetParamByte(REG_KVAL_ACC);
}

quint8 Stepper::getKvalDec()
{
    return GetParamByte(REG_KVAL_DEC);
}

quint8 Stepper::getStepMode()
{
    return GetParamByte(REG_STEP_MODE);
}

quint16	Stepper::getStatus()
{
    return GetParamWord(REG_STATUS);
}

quint16 Stepper::getIntersectSpeed()
{
    return GetParamWord(REG_INT_SPD);
}

quint8 Stepper::getStartSlope()
{
    return GetParamByte(REG_ST_SLP);
}

quint8 Stepper::getAccFinalSlope()
{
    return GetParamByte(REG_FN_SLP_ACC);
}

quint8 Stepper::getDecFinalSlope()
{
    return GetParamByte(REG_FN_SLP_DEC);
}
quint8 Stepper::getKTherm()
{
    return GetParamByte(REG_K_THERM);
}

quint8 Stepper::getOCDThreshold()
{
    return GetParamByte(REG_OCD_TH);
}

quint8 Stepper::getStallThreshold()
{
    return GetParamByte(REG_STALL_TH);
}

quint16 Stepper::getConfiguration()
{
    return GetParamWord(REG_CONFIG);
}

qint32 Stepper::getMarkPosition()
{
    qint32 temp;
    temp = GetParamInt32S(REG_MARK_POS);
    if ((temp & 0x200000) > 0)           // Si tratta di un numero negativo
    {
        temp = ~temp;
        temp = -1 * ((temp & 0x3FFFFF) + 1);
    }
    return temp;
}

qint32 Stepper::getCurrentSpeed()
{
    return GetParamInt32S(REG_SPEED);
}
