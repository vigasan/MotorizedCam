import QtQuick 2.0

Rectangle
{
    id: stepperParams
    color: "black"
    property int sliderWidth: 700

    Text
    {
        text: "PID Parameters"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30
        font.pixelSize: 25
        font.family: typeFont_Bold.name
        color: "white"
    }

    SliderParameter
    {
        id: param_Pid_Kp
        anchors.top: parent.top
        anchors.topMargin: 90
        anchors.horizontalCenter: parent.horizontalCenter
        width: sliderWidth
        paramName: "Kp"
        paramUnit: " "
        minValue: 0
        maxValue: 100
        delta: 1
        paramValue: 10
        onSendParameter:
        {
            pid_Kp = paramValue;
        }
    }

    SliderParameter
    {
        id: param_Pid_Ki
        anchors.top: param_Pid_Kp.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        width: sliderWidth
        paramName: "Ki"
        paramUnit: " "
        minValue: 0
        maxValue: 100
        delta: 1
        paramValue: 0
        onSendParameter:
        {
            pid_Ki = paramValue;
        }
    }

    SliderParameter
    {
        id: param_Pid_Kd
        anchors.top: param_Pid_Ki.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        width: sliderWidth
        paramName: "Kd"
        paramUnit: " "
        minValue: 0
        maxValue: 100
        delta: 1
        paramValue: 0
        onSendParameter:
        {
            pid_Kd = paramValue;
        }
    }

    Image
    {
        id: back
        source: "qrc:images/back64.png"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        scale: 0.8

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                rootPage.reqSettings();
            }
        }
    }

}
