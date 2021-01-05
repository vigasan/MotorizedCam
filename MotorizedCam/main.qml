import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.12


Window//Rectangle
{
    id: rootPage
    visible: true
    width : Screen.height > Screen.width ? Screen.height : Screen.width
    height : Screen.height > Screen.width ? Screen.width : Screen.height

    readonly property int stepMODE_FULL_STEP:   0
    readonly property int stepMODE_HALF_STEP:   1
    readonly property int stepMODE_uSTEP_4:     2
    readonly property int stepMODE_uSTEP_8:     3
    readonly property int stepMODE_uSTEP_16:    4
    readonly property int stepMODE_uSTEP_32:    5
    readonly property int stepMODE_uSTEP_64:    6
    readonly property int stepMODE_uSTEP_128:   7

    property int displayWidth: width
    property int displayHeight: height
    property string fileClassifier: appDir + "/haarcascade_frontalface_default.xml"

    // PID vars
    property real pid_Kp: 10
    property real pid_Ki: 0
    property real pid_Kd: 0

    property int sliderWidt: width * 0.8

    FontLoader
    {
        id: typeFont
        source:"qrc:/images/Helvetica.ttf"
    }

    FontLoader
    {
        id: typeFont_Bold
        source:"qrc:/images/HelveticaSemiBold.ttf"
    }

    signal reqHomePage()
    signal reqSettings()
    signal reqSettingsXpage()
    signal reqSettingsYpage()
    signal reqClassifiersPage()
    signal reqSettingPid()

    onReqHomePage:
    {
        homePage.visible = true;
        settingsPage.visible = false;
        settingsXStepper.visible = false;
        settingsYStepper.visible = false;
        classifiersPage.visible = false;
        pidPage.visible = false;
    }

    onReqSettings:
    {
        homePage.visible = false;
        settingsPage.visible = true;
        settingsXStepper.visible = false;
        settingsYStepper.visible = false;
        classifiersPage.visible = false;
        pidPage.visible = false;
    }

    onReqSettingsXpage:
    {
        homePage.visible = false;
        settingsPage.visible = false;
        settingsXStepper.visible = true;
        settingsYStepper.visible = false;
        classifiersPage.visible = false;
        pidPage.visible = false;
    }

    onReqSettingsYpage:
    {
        homePage.visible = false;
        settingsPage.visible = false;
        settingsXStepper.visible = false;
        settingsYStepper.visible = true;
        classifiersPage.visible = false;
        pidPage.visible = false;
    }

    onReqClassifiersPage:
    {
        homePage.visible = false;
        settingsPage.visible = false;
        settingsXStepper.visible = false;
        settingsYStepper.visible = false;
        classifiersPage.visible = true;
        pidPage.visible = false;
    }

    onReqSettingPid:
    {
        homePage.visible = false;
        settingsPage.visible = false;
        settingsXStepper.visible = false;
        settingsYStepper.visible = false;
        classifiersPage.visible = false;
        pidPage.visible = true;
    }

    //////////////////////////////////////////////////////////////
    // Pages
    //////////////////////////////////////////////////////////////
    HomePage
    {
       id: homePage
       width: parent.width
       height: parent.height
       visible: true
    }

    Settings
    {
        id: settingsPage
        width: parent.width
        height: parent.height
        visible: false
    }

    SettingXPage
    {
        id: settingsXStepper
        width: parent.width
        height: parent.height
        visible: false
    }

    SettingYPage
    {
        id: settingsYStepper
        width: parent.width
        height: parent.height
        visible: false
    }

    ClassifiersPage
    {
        id: classifiersPage
        width: parent.width
        height: parent.height
        visible: false
    }

    SettingPidPage
    {
        id: pidPage
        width: parent.width
        height: parent.height
        visible: false
    }

}
