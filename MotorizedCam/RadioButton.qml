import QtQuick 2.0

Item
{
    id: radioBtn
    height: size

    property int size: 40
    property color radioColor: "white"
    property bool active: false
    property string name: "Name"
    property int fontSize: 16

    signal sendParameter()

    Rectangle
    {
        id: extCircle
        anchors.centerIn: parent
        height: size
        width: size
        color: "transparent"
        radius: size / 2
        border.width: 2
        border.color: radioColor

        Rectangle
        {
            anchors.centerIn: parent
            height: size * 0.6
            width: height
            color: radioColor
            radius: height / 2
            visible: active
        }

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                sendParameter()
            }
        }
    }

    Text
    {
        text: name
        anchors.left: extCircle.right
        anchors.leftMargin: 10
        anchors.verticalCenter: extCircle.verticalCenter
        font.pixelSize: fontSize
        font.family: typeFont.name
        color: "white"
    }
}
