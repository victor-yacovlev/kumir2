// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 40
    height: 40
    radius: 10
    signal clicked
    states: [
        State {
            name: "hovered"

            PropertyChanges {
                target: gradientstop1
                position: 0
                color: "#f9f8f8"
            }

            PropertyChanges {
                target: gradientstop2
                position: 1
                color: "#625757"
            }
        },
        State {
            name: "clicked"
            PropertyChanges {
                target: gradientstop1
                position: 0
                color: "#716363"
            }

            PropertyChanges {
                target: gradientstop2
                position: 1
                color: "#625757"
            }
        }
    ]
    gradient: Gradient {
        GradientStop {
            id: gradientstop1
            position: 0
            color: "#c2acac"
        }

        GradientStop {
            id: gradientstop2
            position: 1
            color: "#625757"
        }
    }

    MouseArea {
        id: mouse_area1
        x: 0
        y: 0
        width: 40
        height: 40
        hoverEnabled: true
        onPressed: parent.state = 'clicked'
        onEntered: parent.state = 'hovered'
        onReleased: parent.state = 'hovered'
        onExited: parent.state = ''
        onClicked: parent.clicked()
    }
}
