import QtQuick 1.1
import QtQuick 1.0

Rectangle {
    id: pult
    width: 260
    height: 450
    color: "#bababa"
    signal goUp
    signal goDown
    signal goLeft
    signal goRight
    signal doPaint
    signal checkWallLeft
    signal checkWallRight
    signal checkWallTop
    signal checkWallBottom
    signal checkFreeLeft
    signal checkFreeRight
    signal checkFreeTop
    signal checkFreeBottom
    signal checkRadiation
    signal checkTemp
    signal checkColored
    signal checkClear
    signal copyTextToKumir(string hjghjgjh)
    property bool buttonsLocked: false

    function addToResultLog(s) {
        resultLog.text+=s+"\n";
        buttonsLocked = false;
    }

    Button {
        id: btnPaint
        x: 110
        y: 257
        z: 3

        Image {
            id: image5
            x: 4
            y: 4
            width: 32
            height: 32
            z: 2
            source: "btn_paint.png"
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "закрасить\n";
            scroll.updateScroll();
            parent.doPaint();
        }
    }

    Button {
        id: btnUp
        x: 110
        y: 204
        z: 3

        Image {
            id: image1
            x: 4
            y: 4
            z: 2
            source: "go-up.png"
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "вверх\n";
            scroll.updateScroll();
            parent.goUp();
        }
    }

    Button {
        id: btnDown
        x: 110
        y: 311
        z: 3

        Image {
            id: image2
            x: 4
            y: 4
            z: 2
            source: "go-down.png"
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "вниз\n";
            scroll.updateScroll();
            parent.goDown();
        }
    }

    Button {
        id: btnLeft
        x: 56
        y: 257
        z: 3

        Image {
            id: image3
            x: 4
            y: 4
            z: 2
            source: "go-previous.png"
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "влево\n";
            scroll.updateScroll();
            parent.goLeft();
        }
    }

    Button {
        id: btnRight
        x: 164
        y: 257
        z: 3

        Image {
            id: image4
            x: 4
            y: 4
            z: 2
            source: "go-next.png"
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "вправо\n";
            scroll.updateScroll();
            parent.goRight();
        }
    }

    Flickable {
        id: scroll
        x: 8
        y: 25
        width: 212
        height: 172
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.DragOverBounds
        z: 1


        Text {
            id: commandLog
            x: 4
            y: 5
            color: "#ffffff"
            text: qsTr("")
            z: 1
            font.bold: true
            font.pixelSize: 14
        }

        Text {
            id: resultLog
            x: 140
            y: 5
            color: "#ffffff"
            text: qsTr("")
            z: 1
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 14
            font.bold: true
        }

        function updateScroll() {
            contentHeight = commandLog.height;
            if (contentHeight>height)
                contentY = contentHeight-height;
        }
    }

    Rectangle {
        id: blackrect
        x: 8
        y: 1
        width: 212
        height: 200
        color: "#000000"
        z: 0
    }

    Rectangle {
        id: toprect
        x: 0
        y: 0
        width: 260
        height: 25
        color: "#bababa"
        z: 2
    }

    Rectangle {
        id: bottomrect
        x: 0
        y: 204
        width: 260
        height: 183
        color: "#bababa"
        z: 2
    }

    Rectangle {
        id: btnGroupCell
        x: 0
        y: 358
        width: 86.600
        height: 30
        gradient: Gradient {
            GradientStop {
                id: gradientstop1
                position: 0
                color: "#ffffff"
            }

            GradientStop {
                id: gradientstop2
                position: 1
                color: "#000000"
            }
        }
        z: 4

        MouseArea {
            width: 86.6
            height: 30
            hoverEnabled: true
            x: 0
            y: 0
            onEntered: text1.color = "white"
            onExited: text1.color = "black"
            onClicked: {
                if (pult.state=='cellProperties')
                    pult.state = '';
                else
                    pult.state = 'cellProperties'
            }
        }

        Text {
            id: text1
            x: 19
            y: 7
            text: qsTr("Клетка")
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }
    }

    Rectangle {
        id: btnGroupWall
        x: 87
        y: 358
        width: 86.600
        height: 30
        radius: 0
        gradient: Gradient {
            GradientStop {
                id: gradientstop3
                position: 0
                color: "#ffffff"
            }

            GradientStop {
                id: gradientstop4
                position: 1
                color: "#000000"
            }
        }
        z: 4

        Text {
            id: text2
            x: 23
            y: 7
            text: qsTr("Стена")
            font.pixelSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            width: 86.6
            height: 30
            hoverEnabled: true
            x: 0
            y: 0
            onEntered: text2.color = "white"
            onExited: text2.color = "black"
            onClicked: {
                if (pult.state=='wallProperties')
                    pult.state = '';
                else
                    pult.state = 'wallProperties'
            }
        }
    }

    Rectangle {
        id: btnGroupFree
        x: 174
        y: 358
        width: 86.600
        height: 30
        gradient: Gradient {
            GradientStop {
                id: gradientstop5
                position: 0
                color: "#ffffff"
            }

            GradientStop {
                id: gradientstop6
                position: 1
                color: "#000000"
            }
        }
        z: 4

        Text {
            id: text3
            x: 7
            y: 7
            text: qsTr("Свободно")
            font.pixelSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            width: 86.6
            height: 30
            hoverEnabled: true
            x: 0
            y: 0
            onEntered: text3.color = "white"
            onExited: text3.color = "black"
            onClicked: {
                if (pult.state=='freeProperties')
                    pult.state = '';
                else
                    pult.state = 'freeProperties'
            }
        }
    }


    Button {
        id: btnTemperature
        x: 8
        y: 515
        opacity: 0
        z: 10

        Image {
            id: image6
            source: "btn_temperature.png"
            opacity: 0
        }
                 onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "температура\n";
            scroll.updateScroll();
            parent.checkTemp();
        }
    }

    Button {
        id: btnRadiation
        x: 87
        y: 515
        opacity: 0
        z: 10

        Image {
            id: image7
            x: 6
            y: 6
            source: "btn_radiation.png"
            opacity: 0
        }
         onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "радиация\n";
            scroll.updateScroll();
            parent.checkRadiation();
        }
    }

    Button {
        id: btnPainted
        x: 160
        y: 515
        opacity: 0
        z: 10

        Image {
            id: image8
            x: 6
            y: 6
            source: "btn_painted.png"
            opacity: 0
        }
                 onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "закрашена\n";
            scroll.updateScroll();
            parent.checkColored();
        }
    }

    Button {
        id: btnClean
        x: 243
        y: 580
        opacity: 0
        z: 10

        Image {
            id: image9
            x: 13
            y: 12
            source: "btn_clean.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "клетка чистая\n";
            scroll.updateScroll();
            parent.checkClear();
        }
    }

    Button {
        id: btnWallAtLeft
        x: 8
        y: 530
        z: 10
        opacity: 0

        Image {
            id: image10
            x: 14
            y: 12
            source: "btn_at_left.png"
            opacity: 0
        }

        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "слева стена\n";
            scroll.updateScroll();
            parent.checkWallLeft();
        }

    }

    Button {
        id: btnWallAtTop
        x: 2
        y: 525
        z: 10
        opacity: 0

        Image {
            id: image11
            x: 6
            y: 6
            source: "btn_at_top.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "сверху стена\n";
            scroll.updateScroll();
            parent.checkWallTop();
        }
    }

    Button {
        id: btnWallAtBottom
        x: 9
        y: 534
        z: 10
        opacity: 0

        Image {
            id: image12
            x: 27
            y: 27
            source: "btn_at_bottom.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "снизу стена\n";
            scroll.updateScroll();
            parent.checkWallBottom();
        }
    }

    Button {
        id: btnWallAtRight
        x: 12
        y: 532
        z: 10
        opacity: 0

        Image {
            id: image13
            x: 20
            y: 17
            source: "btn_at_left.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "справа стена\n";
            scroll.updateScroll();
            parent.checkWallRight();
        }
    }

    Button {
        id: btnFreeAtLeft
        x: 12
        y: 525
        opacity: 0

        Image {
            id: image14
            x: 14
            y: 12
            source: "btn_free_at_left.png"
            opacity: 0
        }

        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "слева свободно\n";
            scroll.updateScroll();
            parent.checkFreeLeft();
        }
    }

    Button {
        id: btnFreeAtTop
        x: 19
        y: 534
        opacity: 0

        Image {
            id: image15
            x: 24
            y: 22
            source: "btn_free_at_top.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "сверху свободно\n";
            scroll.updateScroll();
            parent.checkFreeTop();
        }
    }

    Button {
        id: btnFreeAtBottom
        x: 2
        y: 529
        opacity: 0

        Image {
            id: image16
            x: 19
            y: 27
            source: "btn_free_at_bottom.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "снизу свободно\n";
            scroll.updateScroll();
            parent.checkFreeBottom();
        }
    }

    Button {
        id: btnFreeAtRight
        x: 21
        y: 516
        opacity: 0

        Image {
            id: image17
            x: 13
            y: 27
            source: "btn_free_at_right.png"
            opacity: 0
        }
        onClicked: {
            if (buttonsLocked)
                return;
            buttonsLocked = true;
            commandLog.text += "справа свободно\n";
            scroll.updateScroll();
            parent.checkFreeRight();
        }
    }

    Rectangle {
        id: rectangle1
        x: 0
        y: 500
        width: 200
        height: 200
        color: "#ffffff"
        opacity: 0
    }

    Rectangle {
        id: rectangle2
        x: 0
        y: 500
        width: 200
        height: 200
        color: "#ffffff"
        opacity: 0
    }

    Rectangle {
        id: rectangle3
        x: 0
        y: 500
        width: 200
        height: 200
        color: "#ffffff"
        opacity: 0
    }

    Rectangle {
        id: rectangle4
        x: 0
        y: 387
        width: 260
        height: 69
        color: "#dadada"
    }

    Rectangle {
        id: rectangle6
        x: 10
        y: 402
        width: 40
        height: 40
        color: "#00000000"
        radius: 16
        border.color: "#ffffff"
    }

    Rectangle {
        id: rectangle7
        x: 77
        y: 402
        width: 40
        height: 40
        color: "#00000000"
        radius: 16
        border.color: "#ffffff"
    }

    Rectangle {
        id: rectangle8
        x: 143
        y: 402
        width: 40
        height: 40
        color: "#00000000"
        radius: 16
        border.color: "#ffffff"
    }

    Rectangle {
        id: rectangle9
        x: 210
        y: 402
        width: 40
        height: 40
        color: "#00000000"
        radius: 16
        border.color: "#ffffff"
    }

    Rectangle {
        id: btnScrollUp
        x: 228
        y: 25
        width: 24
        height: 28
        color: "#808080"
        radius: 4
        border.color: "#000000"
        states: [
            State {
                name: "hovered"
                PropertyChanges {
                    target: btnScrollUp
                    color: "white"
                }
            },
            State {
                name: "clicked"
                PropertyChanges {
                    target: btnScrollUp
                    color: "black"
                }
            }
        ]
        MouseArea {
            id: mouse_area1
            hoverEnabled: true
            onEntered: parent.state = 'hovered'
            onReleased: parent.state = 'hovered'
            onPressed: parent.state = 'clicked'
            onExited: parent.state = ''
            onClicked: {
                if (scroll.contentY >= 20)
                    scroll.contentY -= 20;
                else
                    scroll.contentY = 0;
            }
            x: 0
            y: 0
            width: 24
            height: 28
        }

        Image {
            id: image18
            x: 1
            y: 3
            source: "arrow-up.png"
        }
    }

    Rectangle {
        id: btnScrollDown
        x: 228
        y: 169
        width: 24
        height: 28
        color: "#808080"
        radius: 4
        border.color: "#000000"
        Image {
            id: image19
            x: 2
            y: 3
            source: "arrow-down.png"
        }
        states: [
            State {
                name: "hovered"
                PropertyChanges {
                    target: btnScrollDown
                    color: "white"
                }
            },
            State {
                name: "clicked"
                PropertyChanges {
                    target: btnScrollDown
                    color: "black"
                }
            }
        ]
        MouseArea {
            id: mouse_area2
            hoverEnabled: true
            onEntered: parent.state = 'hovered'
            onReleased: parent.state = 'hovered'
            onPressed: parent.state = 'clicked'
            onExited: parent.state = ''
            onClicked: {
                if (scroll.contentY <= scroll.contentHeight - scroll.height - 20)
                    scroll.contentY += 20;
                else
                    scroll.contentY = scroll.contentHeight - scroll.height;
            }
            x: 0
            y: 0
            width: 24
            height: 28
        }

    }

    Rectangle {
        id: btnCopy
        x: 228
        y: 73
        width: 24
        height: 28
        color: "#808080"
        radius: 4
        border.color: "#000000"
        states: [
            State {
                name: "hovered"
                PropertyChanges {
                    target: btnCopy
                    color: "white"
                }
            },
            State {
                name: "clicked"
                PropertyChanges {
                    target: btnCopy
                    color: "black"
                }
            }
        ]
        MouseArea {
            id: mouse_area3
            hoverEnabled: true
            onEntered: parent.state = 'hovered'
            onReleased: parent.state = 'hovered'
            onPressed: parent.state = 'clicked'
            onExited: parent.state = ''
            onClicked: {
                var programText = commandLog.text;
                pult.copyTextToKumir(programText);
            }

            x: 0
            y: 0
            width: 24
            height: 28
        }

        Image {
            id: image20
            x: 2
            y: 3
            source: "edit-copy.png"
        }
    }

    Rectangle {
        id: btnClear
        x: 228
        y: 122
        width: 24
        height: 28
        color: "#808080"
        radius: 4
        border.color: "#000000"
        states: [
            State {
                name: "hovered"
                PropertyChanges {
                    target: btnClear
                    color: "white"
                }
            },
            State {
                name: "clicked"
                PropertyChanges {
                    target: btnClear
                    color: "black"
                }
            }
        ]
        MouseArea {
            id: mouse_area4
            hoverEnabled: true
            onEntered: parent.state = 'hovered'
            onReleased: parent.state = 'hovered'
            onPressed: parent.state = 'clicked'
            onExited: parent.state = ''
            onClicked: {
                commandLog.text = ""
                resultLog.text = ""
                pult.buttonsLocked = false;
                scroll.updateScroll()
            }

            x: 0
            y: 0
            width: 24
            height: 28
        }

        Image {
            id: image21
            x: 2
            y: 3
            source: "edit-clear-list.png"
        }
    }






    states: [
        State {
            name: "cellProperties"



            PropertyChanges {
                target: gradientstop1
                position: 0
                color: "#ffffff"
            }

            PropertyChanges {
                target: gradientstop2
                position: 1
                color: "#404040"
            }

            PropertyChanges {
                target: image6
                x: 4
                y: 4
                width: 32
                height: 32
                z: 4
                opacity: 1
            }

            PropertyChanges {
                target: btnTemperature
                x: 12
                y: 402
                opacity: 1
            }

            PropertyChanges {
                target: btnRadiation
                x: 78
                y: 402
                opacity: 1
            }

            PropertyChanges {
                target: btnPainted
                x: 145
                y: 402
                opacity: 1
            }

            PropertyChanges {
                target: btnClean
                x: 212
                y: 402
                z: 5
                opacity: 1
            }

            PropertyChanges {
                target: image7
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image8
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image9
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: rectangle1
                x: 0
                y: 387
                width: 260
                height: 69
                color: "#404040"
                opacity: 1
            }

            PropertyChanges {
                target: rectangle6
                x: 0
                y: 0
                visible: false
            }

            PropertyChanges {
                target: rectangle4
                visible: false
            }

            PropertyChanges {
                target: rectangle7
                visible: false
            }

            PropertyChanges {
                target: rectangle8
                visible: false
            }

            PropertyChanges {
                target: rectangle9
                visible: false
            }
        },
        State {
            name: "wallProperties"


            PropertyChanges {
                target: btnWallAtLeft
                x: 10
                y: 402
                opacity: 1
            }

            PropertyChanges {
                target: btnWallAtTop
                x: 76
                y: 402
                opacity: 1
                z: 7
            }

            PropertyChanges {
                target: btnWallAtBottom
                x: 143
                y: 402
                opacity: 1
                z: 2
            }

            PropertyChanges {
                target: btnWallAtRight
                x: 210
                y: 402
                opacity: 1
                z: 1
            }

            PropertyChanges {
                target: gradientstop3
                position: 0
                color: "#ffffff"
            }

            PropertyChanges {
                target: gradientstop4
                position: 1
                color: "#404040"
            }

            PropertyChanges {
                target: image10
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image11
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image12
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image13
                x: 4
                y: 4
                width: 32
                height: 32
                source: "btn_at_right.png"
                opacity: 1
            }

            PropertyChanges {
                target: rectangle2
                x: 0
                y: 388
                width: 260
                height: 68
                color: "#404040"
                opacity: 1
            }

            PropertyChanges {
                target: rectangle4
                visible: false
            }

            PropertyChanges {
                target: rectangle9
                visible: false
            }

            PropertyChanges {
                target: rectangle8
                visible: false
            }

            PropertyChanges {
                target: rectangle7
                visible: false
            }

            PropertyChanges {
                target: rectangle6
                visible: false
            }
        },
        State {
            name: "freeProperties"

            PropertyChanges {
                target: gradientstop5
                position: 0
                color: "#ffffff"
            }

            PropertyChanges {
                target: gradientstop6
                position: 1
                color: "#404040"
            }

            PropertyChanges {
                target: btnFreeAtLeft
                x: 10
                y: 402
                z: 14
                opacity: 1
            }

            PropertyChanges {
                target: btnFreeAtTop
                x: 76
                y: 402
                z: 13
                opacity: 1
            }

            PropertyChanges {
                target: btnFreeAtBottom
                x: 143
                y: 402
                z: 12
                opacity: 1
            }

            PropertyChanges {
                target: btnFreeAtRight
                x: 210
                y: 402
                z: 11
                opacity: 1
            }

            PropertyChanges {
                target: image14
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image15
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image16
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: image17
                x: 4
                y: 4
                width: 32
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: rectangle3
                x: 0
                y: 388
                width: 260
                height: 68
                color: "#404040"
                z: 0
                opacity: 1
            }

            PropertyChanges {
                target: rectangle4
                visible: false
            }

            PropertyChanges {
                target: rectangle6
                visible: false
            }

            PropertyChanges {
                target: rectangle7
                visible: false
            }

            PropertyChanges {
                target: rectangle8
                visible: false
            }

            PropertyChanges {
                target: rectangle9
                visible: false
            }
        }
    ]
}
