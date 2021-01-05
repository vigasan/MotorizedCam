import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle
{
    id: settings
    color: "black"

    GridLayout
    {
        id: grid
        rows: 2
        columns: 3
        anchors.fill: parent


        /////////////////////////////////////////////////////////////////////
        // ROW 1
        /////////////////////////////////////////////////////////////////////
        Rectangle
        {
            id: view_1
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 0

            Image
            {
                id: settingStepperX
                source: "qrc:images/stepper128.png"
                anchors.centerIn: parent

                MouseArea
                {
                    anchors.fill: parent
                    onPressed:
                    {
                        stepperX.updateParameters();
                        rootPage.reqSettingsXpage();
                    }
                }
            }

            Text
            {
                text: "X Parameters"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: settingStepperX.bottom
                anchors.topMargin:10
                font.pixelSize: 18
                font.family: typeFont_Bold.name
                color: "white"
            }
         }

        Rectangle
        {
            id: view_2
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 1

            Image
            {
                id: settingStepperY
                source: "qrc:images/stepper128.png"
                anchors.centerIn: parent

                MouseArea
                {
                    anchors.fill: parent
                    onPressed:
                    {
                        stepperY.updateParameters();
                        rootPage.reqSettingsYpage();
                    }
                }
            }

            Text
            {
                text: "Y Parameters"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: settingStepperY.bottom
                anchors.topMargin:10
                font.pixelSize: 18
                font.family: typeFont_Bold.name
                color: "white"
            }

         }

        Rectangle
        {
            id: view_3
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 0
            Layout.column: 2

            Image
            {
                id: settingClassifier
                source: "qrc:images/happy128.png"
                anchors.centerIn: parent

                MouseArea
                {
                    anchors.fill: parent
                    onPressed:
                    {
                        rootPage.reqClassifiersPage();
                    }
                }
            }

            Text
            {
                text: "Classifier"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: settingClassifier.bottom
                anchors.topMargin:10
                font.pixelSize: 18
                font.family: typeFont_Bold.name
                color: "white"
            }

        }

        /////////////////////////////////////////////////////////////////////
        // ROW 2
        /////////////////////////////////////////////////////////////////////
        Rectangle
        {
            id: view_4
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 1
            Layout.column: 0


         }

        Rectangle
        {
            id: view_5
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 1
            Layout.column: 1

            Image
            {
                id: settingPid
                source: "qrc:images/length128.png"
                anchors.centerIn: parent

                MouseArea
                {
                    anchors.fill: parent
                    onPressed:
                    {
                        rootPage.reqSettingPid();
                    }
                }
            }

            Text
            {
                text: "PID Parameters"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: settingPid.bottom
                anchors.topMargin:10
                font.pixelSize: 18
                font.family: typeFont_Bold.name
                color: "white"
            }


         }

        Rectangle
        {
            id: view_6
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.row: 1
            Layout.column: 2

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
                rootPage.reqHomePage();
            }
        }
    }

}
