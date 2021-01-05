import QtQuick 2.0
import QtMultimedia 5.12
import test.opencv.qt 1.0

Item
{
    property int resWidth: 640          // Viewfinder Width Resolution
    property int resHeight: 480         // Viewfinder Height Resolution
    property int resPhotoWidth: 1920    // Camera Capture Width Resolution
    property int resPhotoHeight: 1080   // Camera Capture Height Resolution


    property bool trackingEnabled: true
    property bool stopTracking: false

    readonly property int st_CAPT_IDLE: 0
    readonly property int st_CAPT_START: 1
    readonly property int st_CAPT_RES_CHANGED: 2
    readonly property int st_CAPT_RESTART: 3
    readonly property int st_CAPT_CAPTURE: 4
    readonly property int st_CAPT_CAPTURED: 5
    readonly property int st_CAPT_STANDARD_RES: 6
    readonly property int st_CAPT_WAIT_READY: 7

    property int statusCapture: st_CAPT_IDLE

    // Steppers properties
    property real oldxVal: 0.0
    property real oldyVal: 0.0
    property int speed: 0

    property int horDir_FW: 0
    property int horDir_BW: 1

    property int verDir_FW: 0
    property int verDir_BW: 1

    property int xSpeed: 1
    property int ySpeed: 1

    // PID properties
    property real errX: 0.0
    property real errY: 0.0
    property real old_errX: 0.0
    property real old_errY: 0.0

    property real xP: 0.0
    property real yP: 0.0

    property real xI: 0.0
    property real yI: 0.0

    property real xD: 0.0
    property real yD: 0.0

    property real outX: 0.0
    property real outY: 0.0

    Camera
    {
        id: camera

        viewfinder
        {
            resolution: Qt.size(resWidth, resHeight)
            maximumFrameRate: 15
        }

        exposure
        {
            exposureCompensation: -1.0
            exposureMode: Camera.ExposurePortrait
            manualIso: 100
        }

        imageCapture
        {
           id: cameracapture
           onImageCaptured:
           {
               photoPreview.visible = true;
               photoPreview.source = preview;  // Show the preview in an Image
               statusCapture = st_CAPT_CAPTURED;
               timerPreview.interval = 1000;
               timerPreview.restart();
           }

        }

        onCameraStateChanged:
        {
            console.log("camera state changed to: ", cameraState.toString());

            switch (camera.cameraState)
            {
                case Camera.UnloadedState:
                    console.log("Unloaded STATE");
                    break;
                case Camera.LoadedState:
                    console.log("Loaded STATE");
                    break;
                case Camera.ActiveState:
                    console.log("Active STATE");
                    break;
                default:
                    console.log("no maching STATE");
            }
        }


        onCameraStatusChanged:
        {
            switch(statusCapture)
            {
                case st_CAPT_IDLE:
                    if(camera.cameraStatus === Camera.ActiveStatus)
                    {
                        console.log("Camera STATUS active");
                    } else
                    {
                        console.log("Camera STATUS inactive");
                    }
                    break;

                case st_CAPT_START:
                    if(camera.cameraStatus !== Camera.ActiveStatus)
                    {
                        console.log("Change resolution");
                        camera.viewfinder.resolution = Qt.size(resPhotoWidth, resPhotoHeight)
                        statusCapture = st_CAPT_RES_CHANGED;
                    }
                    break;

                case st_CAPT_RES_CHANGED:
                    if(camera.cameraStatus !== Camera.ActiveStatus)
                    {
                        console.log("Restart Camera");
                        statusCapture = st_CAPT_RESTART;
                        camera.start();
                    }
                    break;

                case st_CAPT_RESTART:
                    if(camera.cameraStatus === Camera.ActiveStatus)
                    {
                        console.log("Start Timer Capture");
                        statusCapture = st_CAPT_CAPTURE;
                        timerPreview.restart();
                    }
                    break;

                case st_CAPT_STANDARD_RES:
                    if(camera.cameraStatus !== Camera.ActiveStatus)
                    {
                        console.log("Start Again");
                        camera.start();
                        statusCapture = st_CAPT_WAIT_READY;
                    }
                    break;

                case st_CAPT_WAIT_READY:
                    if(camera.cameraStatus === Camera.ActiveStatus)
                    {
                        console.log("Ready");
                        trackingEnabled = true;
                        photoPreview.visible = false;
                        statusCapture = st_CAPT_IDLE;
                    }
                    break;
            }
        }
    }

    Timer
    {
        id: timerPreview
        interval: 1
        onTriggered:
        {
            if(statusCapture === st_CAPT_CAPTURE)
            {
                console.log( "Capture" );
                camera.imageCapture.capture();
            } else
            {
                console.log( "Back Resolution" );
                camera.stop();
                camera.viewfinder.resolution = Qt.size(resWidth, resHeight)
                statusCapture = st_CAPT_STANDARD_RES;
            }

        }
    }


    function resetRects()
    {
        for (var i = 0; i < rectHolder.count; ++i)
            rectHolder.itemAt(i).visible = false;
    }

    VideoOutput
    {
        id: output
        source: camera
        filters: [ filter ]
        anchors.fill: parent
        focus: visible
        fillMode: VideoOutput.PreserveAspectCrop
    }

    CVFilter
    {
        id: filter
        active: true
        onActiveChanged: resetRects()
        property bool flip: false
        property string classifier: fileClassifier
        property real scaleFactor: 1.3// slScale.value
        property int minNeighbours: 5 //slMN.value

        onFinished:
        {
            if (!filter.active) // due to queued slot invocations it may happen that active has changed since the emit
                return;
            var r = e.rects();
            resetRects();
            //matchCount.v = r.length;
            for (var i = 0; i < r.length; ++i)
            {
                var xr = output.width / output.sourceRect.width;
                var yr = output.height / output.sourceRect.height;
                var rect = rectHolder.itemAt(i);
                rect.x = output.x + r[i].x * xr;
                rect.y = output.y + r[i].y * yr;
                rect.width = r[i].width * xr;
                rect.height = r[i].height * yr;
                rect.visible = true;
            }

            // PID Controller
            if(r.length === 1 && trackingEnabled === true)
            {

                stopTracking = true;
                var newXpos = r[0].x + (r[0].width / 2);
                var newYpos = r[0].y + (r[0].height / 2);

                errX = newXpos - (resWidth / 2);
                errY = newYpos - (resHeight / 2);

                xP = pid_Kp * errX;
                yP = pid_Kp * errY;

                xI = xI + (errX * pid_Ki);
                yI = yI + (errY * pid_Ki);

                xD = pid_Kd * (errX - old_errX);
                old_errX = errX;
                yD = pid_Kd * (errY - old_errY);
                old_errY = errY;

                outX = xP + xI + xD;
                outY = yP + yI + yD;

                if(outX > 0.0)
                {
                    stepperX.run(horDir_FW, outX);
                } else
                {
                    outX = -1 * outX;
                    stepperX.run(horDir_BW, outX);
                }

                if(outY > 0.0)
                {
                    stepperY.run(verDir_FW, outY);
                } else
                {
                    outY = -1 * outY;
                    stepperY.run(verDir_BW, outY);
                }

            } else
            {
                if(stopTracking === true)
                {
                    stopTracking = false;
                    stepperX.softStop();
                    stepperY.softStop();
                }
            }
        }
    }

    Repeater
    {
        id: rectHolder
        model: 20
        Rectangle
        {
            color: "transparent"
            border.width: 4
            border.color: "red"
            visible: false
        }
    }

    Image
    {
        id: photoPreview
        anchors.fill: parent
    }


    JoyPad
    {
        id:joyPad
        width:150
        height:150
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5

        onPressed:
        {
            trackingEnabled = false;
            stopTracking = true;
        }

        onReleased:
        {
            trackingEnabled = true;
            stepperX.softStop();
            stepperY.softStop();
        }

        onSendParameter:
        {
            if(Math.abs(xVal - oldxVal) > 0.1)
            {
                if(xVal > 0.0)
                {
                    speed = Math.round(xVal * 1000 * xSpeed);
                    stepperX.run(horDir_FW, speed);

                } else
                {
                    speed = Math.round(-1 * xVal * 1000 * xSpeed);
                    stepperX.run(horDir_BW, speed);
                }
                oldxVal = xVal;
            }

            if(Math.abs(yVal - oldyVal) > 0.1)
            {
                if(yVal > 0.0)
                {
                    speed = Math.round(yVal * 1000 * ySpeed);
                    stepperY.run(verDir_BW, speed);
                } else
                {
                    speed = Math.round(-1 * yVal * 1000 * ySpeed);
                    stepperY.run(verDir_FW, speed);
                }
                oldyVal = yVal;
            }
        }
    }

    Image
    {
        id: record
        source: "qrc:images/circle-64.png"
        anchors.bottom: settingsStepper.top
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        scale: 0.8

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                if(camera.cameraStatus === Camera.ActiveStatus)
                {
                    statusCapture = st_CAPT_START;
                    trackingEnabled = false;
                    stopTracking = true;
                    camera.stop();
                } else
                {
                    console.log("Please start Camera capture to take image");
                }
            }
        }
    }

    Image
    {
        id: settingsStepper
        source: "qrc:images/settings64.png"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
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
