import QtQuick 2.0

Rectangle
{
    color: "black"
    property int leftmargin: 250

    Text
    {
        text: "Set Classifier"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30
        font.pixelSize: 25
        font.family: typeFont_Bold.name
        color: "white"
    }



    RadioButton
    {
        id: rbClassifierFace
        anchors.left: parent.left
        anchors.leftMargin: leftmargin
        anchors.top: parent.top
        anchors.topMargin: 90
        name: "Face"
        size: 60
        fontSize: 25
        active: true
        onSendParameter:
        {
            rbClassifierFace.active = true;
            rbClassifierEye.active = false;
            rbClassifierUpperBody.active = false;
            rbClassifierFullBody.active = false;
            fileClassifier = appDir + "/haarcascade_frontalface_default.xml"
            console.log("Classifier: ", fileClassifier)
        }

    }

    RadioButton
    {
        id: rbClassifierEye
        anchors.left: parent.left
        anchors.leftMargin: leftmargin
        anchors.top: rbClassifierFace.bottom
        anchors.topMargin: 25
        name: "Eye"
        size: 60
        fontSize: 25
        onSendParameter:
        {
            rbClassifierFace.active = false;
            rbClassifierEye.active = true;
            rbClassifierUpperBody.active = false;
            rbClassifierFullBody.active = false;
            fileClassifier = appDir + "/haarcascade_eye.xml"
            console.log("Classifier: ", fileClassifier)
        }
    }

    RadioButton
    {
        id: rbClassifierUpperBody
        anchors.left: parent.left
        anchors.leftMargin: leftmargin
        anchors.top: rbClassifierEye.bottom
        anchors.topMargin: 25
        name: "Upper Body"
        size: 60
        fontSize: 25
        onSendParameter:
        {
            rbClassifierFace.active = false;
            rbClassifierEye.active = false;
            rbClassifierUpperBody.active = true;
            rbClassifierFullBody.active = false;
            fileClassifier = appDir + "/haarcascade_upperbody.xml"
            console.log("Classifier: ", fileClassifier)
        }
    }

    RadioButton
    {
        id: rbClassifierFullBody
        anchors.left: parent.left
        anchors.leftMargin: leftmargin
        anchors.top: rbClassifierUpperBody.bottom
        anchors.topMargin: 25
        name: "Full Body"
        size: 60
        fontSize: 25
        onSendParameter:
        {
            rbClassifierFace.active = false;
            rbClassifierEye.active = false;
            rbClassifierUpperBody.active = false;
            rbClassifierFullBody.active = true;
            fileClassifier = appDir + "/haarcascade_fullbody.xml"
            console.log("Classifier: ", fileClassifier)
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
