import QtQuick 2.0

Item
{
    id:joyStick
    property int offset:30

    property real xVal: 0.0
    property real yVal: 0.0
    property bool horizontalEnabled: true
    property bool verticalEnabled: true
    property bool resetOnRelease: true
    property color handleGrad1: "lightgray"
    property color handleGrad2: "gray"

    signal pressed()
    signal released()
    signal sendParameter()

    function reset()
    {
        xVal = 0.0;
        yVal = 0.0;
        stick.x = totalArea.width /2 - stick.radius;
        stick.y = totalArea.height/2 - stick.radius;
    }

    Rectangle
    {
        id:totalArea
        color:"transparent"
        radius: parent.width/2
        opacity: 0.5
        width:parent.width;
        height:parent.height

        Image
        {
            source: "qrc:images/joybgr.png"
            anchors.fill: parent
            antialiasing: true
        }
    }

    Rectangle
    {
        id:stick
        width:totalArea.width / 4;
        height: width
        radius: width / 2
        x: totalArea.width / 2 - radius;
        y: totalArea.height / 2 - radius;
        gradient: Gradient
            {
                GradientStop { position: 0.0; color: handleGrad1 }
                GradientStop { position: 1.0; color: handleGrad1 }
            }

        Behavior on x
        {
            NumberAnimation
            {
                duration: 200
                easing.type: Easing.OutQuad
            }
        }

        Behavior on y
        {
            NumberAnimation
            {
                duration: 200
                easing.type: Easing.OutQuad
            }
        }
    }

    MouseArea
    {
        id:mouseArea
        anchors.fill: parent

        onPressed:
        {
            joyStick.pressed();
        }

        onPositionChanged:
        {
            var rad = totalArea.radius;
            rad =  rad * rad;

            // calculate distance in x direction
            var xDist = mouseX - (totalArea.x + totalArea.radius);
            xDist = xDist * xDist;

            // calculate distance in y direction
            var yDist = mouseY - (totalArea.y + totalArea.radius);

            yDist = yDist * yDist;

            //total distance for inner circle
            var dist = xDist + yDist;

            //if distance if less then radius then inner circle is inside larger circle

            if(dist > rad)
            {
                // return;
                var angle = Math.atan2(mouseY - totalArea.radius, mouseX - totalArea.radius);

                var xCorr = Math.cos(angle) * totalArea.radius + totalArea.radius;
                var yCorr = Math.sin(angle) * totalArea.radius + totalArea.radius;

                if(horizontalEnabled)
                {
                    xVal = (xCorr - (totalArea.x + totalArea.radius)) / totalArea.radius;
                    stick.x = xCorr - stick.radius;
                } else
                    xVal = 0.0;


                if(verticalEnabled)
                {
                    yVal = -1 * (yCorr - (totalArea.y + totalArea.radius)) / totalArea.radius;
                    stick.y = yCorr - stick.radius;
                } else
                    yVal = 0.0;

            } else
            {
                var oldX = stick.x; var oldY = stick.y;



                if(horizontalEnabled)
                {
                    xVal = (mouseX - (totalArea.x + totalArea.radius)) / totalArea.radius;
                    stick.x = mouseX - stick.radius;
                } else
                    xVal = 0.0;

                if(verticalEnabled)
                {
                    yVal = -1 * (mouseY - (totalArea.y + totalArea.radius)) / totalArea.radius;
                    stick.y = mouseY - stick.radius;
                } else
                    yVal = 0.0;
            }

            joyStick.sendParameter();

        }

        onReleased:
        {
            //snap to center
            if(resetOnRelease)
                reset();

            joyStick.released();
        }
    }
}
