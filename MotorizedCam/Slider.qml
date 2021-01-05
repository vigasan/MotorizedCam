import QtQuick 2.2

Item
{
    id:slider
    height: heightSlider

    signal released(int value)
    signal pressed()

    // value is read/write.
    property real value: 1
    onValueChanged: updatePos();
    property real minimum: 1
    property real maximum: 1
    property real step: 1
    //property int xMax: width - handle.width - 4
    property real xMax: width - handle.width
    onXMaxChanged: updatePos();
    onMinimumChanged: updatePos();

    property color backgroundEmpty: "lightgrey";
    property color backgroundFull: "#FFA500"//"#39F724";
    property real  backgroundopacity: 1;
    property real  backgroundopacityFull: 1;
    property color pressCircle: "#FFA500"//"#39F724"
    property color handleGrad1: "lightgray"
    property color handleGrad2: "gray"
    property real  heightSlider: 10
    property real  fullCircle: 30
    property real  circleWidth: 70
    property real  circleHeight: 70

    /*
    function updatePos() {
        if (maximum > minimum) {
            var pos = 2 + (value - minimum) * slider.xMax / (maximum - minimum);
            pos = Math.min(pos, width - handle.width - 2);
            pos = Math.max(pos, 2);
            handle.x = pos;
        } else {
            handle.x = 2;
        }
    }
    */

    function updatePos()
    {
        if (maximum > minimum)
        {
            var pos = (value - minimum) * slider.xMax / (maximum - minimum);
            pos = Math.min(pos, width - handle.width / 2);
            pos = Math.max(pos, 0);
            handle.x = pos;
        } else
        {
            handle.x = 0;
        }
    }


    Rectangle
    {
        id:background
        x: fullCircle / 2
        height: heightSlider
        width: slider.width - fullCircle
        //anchors.fill: parent
        border.color: "white"; border.width: 0; radius: heightSlider * 2
        color: backgroundEmpty
        opacity: backgroundopacity
        /*
        gradient: Gradient {
            //GradientStop { position: 0.0; color: "#66343434" }
            //GradientStop { position: 1.0; color: "#66000000" }
            GradientStop { position: 0.0; color: "#41526F" }
            GradientStop { position: 1.0; color: "#1F2735" }
        }
        */
    }

    Rectangle
    {
        id:sliderFilled
        width: handle.x + (handle.width / 2)
        height: heightSlider
        border.color: "white"; border.width: 0; radius: heightSlider * 2
        //color: "#A111D"
        color: backgroundFull
        opacity: backgroundopacityFull
    }


    Rectangle
    {
        id: handle;
        smooth: true
        //y: 2; width: 30; height: slider.height-4; radius: 6
        width: fullCircle;
        height: fullCircle;
        radius: fullCircle/2
        anchors.verticalCenter: parent.verticalCenter
        gradient: Gradient {
            GradientStop { position: 0.0; color: handleGrad1 }
            GradientStop { position: 1.0; color: handleGrad1 }
        }

        Rectangle
        {
            id:cerchio;
            smooth: true
            visible:false
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width:circleWidth
            height: circleHeight; radius: circleWidth/2
            color:"transparent"
            border.width: 4
            border.color: pressCircle
        }

        MouseArea
        {
            id: mouse
            //anchors.fill: parent
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width:70
            height: 70
            drag.target: parent
            drag.axis: Drag.XAxis; drag.minimumX: 0; drag.maximumX: slider.xMax
            //onPositionChanged: { value = (maximum - minimum) * (handle.x-2) / slider.xMax + minimum; }

            onPositionChanged: {
                var stepPixel = ((slider.width - fullCircle) * slider.step) / (maximum - minimum);
                var numSteps = Math.round(handle.x / stepPixel);
                handle.x = numSteps * stepPixel;
                value = Math.round(numSteps * slider.step) + minimum;
            }

            onPressed:
            {
                cerchio.visible = true;
                slider.pressed();
            }

            onReleased:
            {
                cerchio.visible = false;
                var stepPixel = ((slider.width - fullCircle) * slider.step) / (maximum - minimum);
                var numSteps = Math.round(handle.x / stepPixel);
                handle.x = numSteps * stepPixel;
                value = Math.round(numSteps * slider.step) + minimum;
                slider.released(value);
            }
        }
    }
}
