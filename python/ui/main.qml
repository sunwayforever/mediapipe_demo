import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0

ApplicationWindow {
    id: applicationWindow
    visible: true

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Image {
            id: webcam
            cache: false
            width: 100
            height: 100
            source: "image://webcam"
            Layout.fillHeight: true
            Layout.preferredHeight: 200
            Layout.fillWidth: true
            fillMode: Image.PreserveAspectFit
            function reload() {
                source=""
                source="image://webcam"
            }
        }

        Connections {
            target: backend
            onImageChanged: webcam.reload()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1}
}
##^##*/
