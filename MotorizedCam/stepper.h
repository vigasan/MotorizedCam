#ifndef STEPPER_H
#define STEPPER_H

#include <QObject>
#include "gpio.h"
#include <QJsonDocument>
#include <QJsonObject>

#define REG_ABS_POS			0x01		// Current Position				22
#define REG_EL_POS			0x02		// Electrical Position			9
#define REG_MARK_POS		0x03		// Mark Position				22
#define REG_SPEED			0x04		// Current Speed				20
#define REG_ACC				0x05		// Acceleration					12
#define REG_DEC				0x06		// Deceleration					12
#define REG_MAX_SPEED		0x07		// Maximum Speed				10
#define REG_MIN_SPEED		0x08		// Minimim Speed				13
#define REG_FS_SPD			0x15		// Full Step Speed				10
#define REG_KVAL_HOLD 		0x09		// Holding KVAL 				8
#define REG_KVAL_RUN 		0x0A		// Constant speed KVAL 			8
#define REG_KVAL_ACC 		0x0B		// Acceleration starting KVAL 	8
#define REG_KVAL_DEC 		0x0C		// Deceleration starting KVAL 	8
#define REG_INT_SPD 		0x0D		// Intersect speed 				14
#define REG_ST_SLP 			0x0E		// Start slope 					8
#define REG_FN_SLP_ACC 		0x0F		// Acceleration final slope 	8
#define REG_FN_SLP_DEC 		0x10		// Deceleration final slope 	8
#define REG_K_THERM 		0x11		// Thermal compensationfactor	4
#define REG_ADC_OUT 		0x12		// ADC output 					5
#define REG_OCD_TH			0x13		// OCD threshold 				4
#define REG_STALL_TH		0x14		// STALL threshold 				7
#define REG_STEP_MODE 		0x16		// Step mode 					8
#define REG_ALARM_EN 		0x17		// Alarms enables 				8
#define REG_CONFIG			0x18		// IC Configuration				16
#define REG_STATUS			0x19		// Status						16

#define NOP_COMMAND			0x00


// DEFAULT PARAMETERS
#define DEFLT_STEP_MODE		7
#define DEFLT_THSPEED		98
#define DEFLT_MAX_SPEED		100
#define DEFLT_MIN_SPEED		4097
#define DEFLT_ACC			10
#define DEFLT_DEC			30
#define DEFLT_KV_HOLD		5
#define DEFLT_KV_RUN		50
#define DEFLT_KV_ACC		50
#define DEFLT_KV_DEC		50
#define DEFLT_K_THERM		0
#define DEFLT_OCD_TH		15
#define DEFLT_STALL_TH		127



class Stepper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 Position READ Position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(quint8 StepMode READ StepMode WRITE setStepMode NOTIFY stepModeChanged)
    Q_PROPERTY(quint16 ThresholdSpeed READ ThresholdSpeed WRITE setFullStepSpeed NOTIFY thresholdSpeedChanged)
    Q_PROPERTY(quint16 MaxSpeed READ MaxSpeed WRITE setMaxSpeed NOTIFY maxSpeedChanged)
    Q_PROPERTY(quint16 MinSpeed READ MinSpeed WRITE setMinSpeed NOTIFY minSpeedChanged)
    Q_PROPERTY(quint16 Acceleration READ Acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(quint16 Deceleration READ Deceleration WRITE setDeceleration NOTIFY decelerationChanged)

    Q_PROPERTY(quint8 KValHold READ KValHold WRITE setKvalHold NOTIFY kValHoldChanged)
    Q_PROPERTY(quint8 KValRun READ KValRun WRITE setKvalRun NOTIFY kValRunChanged)
    Q_PROPERTY(quint8 KValAcc READ KValAcc WRITE setKvalAcc NOTIFY kValAccChanged)
    Q_PROPERTY(quint8 KValDec READ KValDec WRITE setKvalDec NOTIFY kValDecChanged)
    Q_PROPERTY(quint8 KTherm READ KTherm WRITE setKTherm NOTIFY kThermChanged)
    Q_PROPERTY(quint8 OcdThreshold READ OcdThreshold WRITE setOCDThreshold NOTIFY ocdThresholdChanged)
    Q_PROPERTY(quint8 StallThreshold READ StallThreshold WRITE setStallThreshold NOTIFY stallThresholdChanged)


public:
    explicit Stepper(GPIO *cs, GPIO *stpclk, int spiChannel, QString fileParams, GPIO *busyPin, QObject *parent = nullptr);


private:
    GPIO *m_cs;
    GPIO *m_stpClk;
    GPIO *m_busy;
    int m_channel;
    bool m_StepperBusy;
    QString m_jsonFileName;
    QJsonObject m_jsonObj;

    qint32  m_Position;
    qint32  Position();
    void    setPosition(qint32 position);

    quint8  m_StepMode;
    quint8  StepMode();
    void	setStepMode(quint8 val);

    quint16 m_ThresholdSpeed;
    quint16 ThresholdSpeed();
    void 	setFullStepSpeed(quint16 thSpeed);

    quint16 m_MaxSpeed;
    quint16 MaxSpeed();
    void 	setMaxSpeed(quint16 speed);

    quint16 m_MinSpeed;
    quint16 MinSpeed();
    void 	setMinSpeed(quint16 speed);

    quint16 m_Acceleration;
    quint16 Acceleration();
    void 	setAcceleration(quint16 acc);

    quint16 m_Deceleration;
    quint16 Deceleration();
    void 	setDeceleration(quint16 dec);

    quint8  m_KValHold;
    quint8  KValHold();
    void 	setKvalHold(quint8 val);

    quint8  m_KValRun;
    quint8  KValRun();
    void 	setKvalRun(quint8 val);

    quint8  m_KValAcc;
    quint8  KValAcc();
    void 	setKvalAcc(quint8 val);

    quint8  m_KValDec;
    quint8  KValDec();
    void 	setKvalDec(quint8 val);

    quint8  m_KTherm;
    quint8  KTherm();
    void 	setKTherm(quint8 value);

    quint8  m_OcdThreshold;
    quint8  OcdThreshold();
    void 	setOCDThreshold(quint8 value);

    quint8  m_StallThreshold;
    quint8  StallThreshold();
    void 	setStallThreshold(quint8 value);


    // Json name keys
    const QString KEY_STEP_MODE = "stepMode";
    const QString KEY_THRESHOLD_FULL_SPEED= "thresholdFullSpeed";
    const QString KEY_MAX_SPEED= "maxSpeed";
    const QString KEY_MIN_SPEED= "minSpeed";
    const QString KEY_ACCELERATION= "acceleration";
    const QString KEY_DECELERATION= "deceleration";
    const QString KEY_KVAL_HOLD= "kv_hold";
    const QString KEY_KVAL_RUN= "kv_run";
    const QString KEY_KVAL_ACC= "kv_acc";
    const QString KEY_KVAL_DEC= "kv_dec";
    const QString KEY_K_THERMIC= "k_thermic";
    const QString KEY_OCD_THRESHOLD= "ocdThreshold";
    const QString KEY_STALL_THRESHOLD= "stallThreshold";


    void 	SetByteParam(unsigned char reg, unsigned char value);
    void 	SetWordParam(quint8 reg, quint16 value);
    quint8 	GetParamByte(unsigned char param);
    quint16 GetParamWord(quint8 param);
    qint32 	GetParamInt32S(quint8 param);


signals:
    void positionChanged();
    void stepModeChanged();
    void thresholdSpeedChanged();
    void maxSpeedChanged();
    void minSpeedChanged();
    void accelerationChanged();
    void decelerationChanged();
    void kValHoldChanged();
    void kValRunChanged();
    void kValAccChanged();
    void kValDecChanged();
    void kThermChanged();
    void ocdThresholdChanged();
    void stallThresholdChanged();

public slots:
    void    cmdCompleted();
    void    setDefaultParameters();
    void    sendParameters();
    void    updateParameters();
    void    saveParameters();

    void    clock();

    void 	run(quint8 dir, quint32 speed);
    void 	stepClock(quint8 dir);
    void 	softStop();
    void 	hardStop();
    void 	softHiZ();
    void	hardHiZ();
    void 	move(quint8 dir, quint32 step);
    void 	goTo(qint32 position);
    void 	goToDir(quint8 dir, qint32 position);
    void 	goUntil(quint8 act, quint8 dir, quint32 speed);
    void 	releaseSW(quint8 act, quint8 dir);
    void 	goHome();
    void 	goMark();
    void 	resetPosition();
    void 	resetDevice();


    void 	setIntersectSpeed(quint16 speed);
    void	setStartSlope(quint8 value);
    void 	setAccFinalSlope(quint8 value);
    void 	setDecFinalSlope(quint8 value);
    void 	setConfiguration(quint16 value);
    void	setMarkPosition(qint32 value);
    void 	setAbsPosition(qint32 value);
    qint32	getAbsPosition();
    quint16 getFullStepSpeed();
    quint16 getMaxSpeed();
    quint16 getMinSpeed();
    quint16 getAcceleration();
    quint16 getDeceleration();
    quint8 	getKvalHold();
    quint8 	getKvalRun();
    quint8 	getKvalAcc();
    quint8 	getKvalDec();
    quint8 	getStepMode();
    quint16	getStatus();
    quint16 getIntersectSpeed();
    quint8 	getStartSlope();
    quint8 	getAccFinalSlope();
    quint8 	getDecFinalSlope();
    quint8 	getKTherm();
    quint8 	getOCDThreshold();
    quint8 	getStallThreshold();
    quint16 getConfiguration();
    qint32	getMarkPosition();
    qint32	getCurrentSpeed();
};

#endif // STEPPER_H
