import QtQuick 2.0

Item
{
    height: 55

    property string paramName: "Name"
    property string paramUnit: " "
    property real paramValue: 1
    property real minValue: 1
    property real maxValue: 100
    property real delta: 1

    signal sendParameter()

    Rectangle
    {
        width: parent.width
        height: parent.height
        color: "transparent"

        Text
        {
            id: strParamUnit
            text: paramName + ": " + paramValue;
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin:  10
            font.pixelSize: 18
            font.family: typeFont_Bold.name
            color: "white"
        }

        Slider
        {
            id: sld
            width: parent.width * 0.90
            heightSlider: 8
            fullCircle: 20
            circleHeight: 40
            circleWidth: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: strParamUnit.bottom
            anchors.topMargin:  10
            minimum: minValue
            maximum: maxValue
            step: delta
            value: paramValue
            onReleased:
            {
                sendParameter();
            }
            onValueChanged:
            {
                paramValue = value;
            }
        }

    }

}
