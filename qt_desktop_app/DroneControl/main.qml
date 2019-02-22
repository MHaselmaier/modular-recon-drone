import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Window 2.2
import Drone.ClientHandle 1.0
import QtGamepad 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Drone Control")
    ClientHandle{
        id: clienthandle
        onCameraDataChanged: {
            canvas.requestPaint()
        }
    }
    Gamepad{
        id: gamepad
        deviceId: GamepadManager.connectedGamepads[0]
        onAxisRightYChanged: {
            speed_slider.value = 100 * value * -1;
        }

        onAxisLeftXChanged: {
            balance_slider.value = 100 * value;
        }
    }

    Timer{
        interval: 20
        running: true
        repeat: true
        onTriggered:{
            sendData(speed_slider.value, balance_slider.value);
        }
    }

    function sendData(speedVal, balanceVal)
    {
        var direction = speedVal > 0 ? 0 : 1;
        var speed = Math.abs(speedVal);
        var motorA, motorB;

        if(balanceVal === 0)
            motorA = motorB = speed;
        else if(balanceVal < 0)
        {
            motorA = speed;
            motorB = speed - (-balanceVal);
            if(motorB < 50)
                motorB = 50;
        }
        else
        {
            motorA = speed - balanceVal;
            motorB = speed;
            if(motorA < 50)
                motorA = 50;
        }

        clienthandle.sendData(direction, motorB, direction, motorA)
    }

    Item{
        width: parent.width
        height: parent.height
        focus: true

        Keys.onPressed:
        {
            if(event.key === Qt.Key_Up){
                speed_slider.value += 20;
            }else if(event.key === Qt.Key_Down){
                speed_slider.value -= 20;
            }

            if(event.key === Qt.Key_Left){
                balance_slider.value -= 25;
            }else if(event.key === Qt.Key_Right){
                balance_slider.value += 25;
            }

        }


        Label{
            id: speed_label
            width: 100
            height:25
            text: "Speedcontrol: " + speed_slider.value + "%"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
        }

        Slider{
            id: speed_slider
            from: -100
            to: 100
            stepSize: 20
            value: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: speed_label.bottom
        }

        Label{
            id: balance_label
            width: 100
            height: 25
            text: "Steering Balance: " + balance_slider.value
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: speed_slider.bottom
        }

        Slider{
            id: balance_slider
            from: -100
            to: 100
            value: 0
            stepSize: 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: balance_label.bottom
        }

        Canvas{
            id: canvas
            width: clienthandle.cameraImageWidth
            height: clienthandle.cameraImageHeight
            anchors.top: balance_slider.bottom
            anchors.topMargin: 50
            anchors.horizontalCenterOffset: -width
            anchors.horizontalCenter: parent.horizontalCenter
            transform: Scale{xScale: 2.5; yScale: 2.5}
            onPaint:{
                var ctx = canvas.getContext("2d")
                var imageDataObj = ctx.createImageData(160, 120)
                var cameraData = new Uint8Array(clienthandle.cameraData)
                var cameraDataIndex = clienthandle.cameraImageWidth * clienthandle.cameraImageHeight
                for( var i = 0; i < imageDataObj.data.length; i+=4){
                    imageDataObj.data[i+2] = imageDataObj.data[i+1] = imageDataObj.data[i] = cameraData[cameraDataIndex]
                    imageDataObj.data[i+3] = 255
                    cameraDataIndex--
                }
                ctx.putImageData(imageDataObj, 0, 0, 0, 0, 160, 120)
            }
        }
    }
}
