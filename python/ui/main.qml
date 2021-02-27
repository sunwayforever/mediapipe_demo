import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick3D 1.15

ApplicationWindow {
    id: applicationWindow
    visible: true

    View3D {
        id: view3D
        width: 300
        height: 300
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
        z: 1

        SceneEnvironment {
            id: sceneEnvironment
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }

        Node {
            id: scene
            DirectionalLight {
                id: directionalLight
            }

            PerspectiveCamera {
                id: camera
                z: 350
            }

            Model {
                id: cubeModel
                source: "meshes/group1.mesh"
                scale.z: 100
                scale.y: 100
                scale.x: 100
                DefaultMaterial {
                    id: cubeMaterial
                    diffuseColor: "#ff999999"
                }
                materials: cubeMaterial
                eulerRotation.x: -180
                eulerRotation.y: 0
                eulerRotation.z: 0
            }
        }

        Connections {
            target: backend
            function onRotationChanged(vector) {
                /* yaw */
                cubeModel.eulerRotation.y=vector[0]
                /* pitch */
                cubeModel.eulerRotation.x=vector[1]-180
                /* row */
                cubeModel.eulerRotation.z=vector[2]
            }
        }

        environment: sceneEnvironment
    }

    RowLayout {
        id: rowLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.fill: parent
        anchors.rightMargin: 388
        anchors.leftMargin: 0
        anchors.bottomMargin: 20
        anchors.topMargin: 0
        Layout.preferredHeight: 50
        Layout.fillHeight: true
        Layout.fillWidth: true

        ColumnLayout {
            id: columnLayout
            width: 100
            height: 100
            Layout.margins: 10
            Layout.maximumWidth: 65535
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.preferredWidth: 20

            Switch {
                id: mouth
                text: "mouth"
                Layout.margins: 5
                Layout.rowSpan: 1
                font.underline: false
                display: AbstractButton.TextBesideIcon
                checkable: false
                Layout.preferredHeight: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Switch {
                id: left_eye
                text: "left eye"
                Layout.margins: 5
                Layout.rowSpan: 2
                checkable: false
                Layout.preferredHeight: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Switch {
                id: right_eye
                text: "right eye"
                Layout.margins: 5
                Layout.rowSpan: 2
                checkable: false
                Layout.preferredHeight: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }


        Image {
            id: webcam
            cache: false
            width: 100
            height: 100
            source: "image://webcam"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.margins: 10
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            Layout.preferredHeight: -1
            Layout.fillWidth: true
            fillMode: Image.PreserveAspectFit
            function reload() {
                source=""
                source="image://webcam"
            }
        }

        ColumnLayout {
            id: columnLayout1
            width: 100
            height: 100
            Layout.preferredWidth: 40
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignTop

            Image {
                id: facenet
                width: 100
                height: 100
                cache: false
                source: "image://facenet"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: true
                Layout.fillWidth: true
                fillMode: Image.PreserveAspectFit
                function reload() {
                    source=""
                    source="image://facenet"
                }
            }
        }

        Connections {
            target: backend
            function onWebcamImageChanged() {
                webcam.reload()
            }

            function onFacenetImageChanged() {
                facenet.reload()
            }

            function onMouthChanged(is_open) {
                mouth.checked=is_open
            }

            function onEyeChanged(left,right) {
                left_eye.checked=left
                right_eye.checked=right
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:1.25;height:480;width:640}
}
##^##*/
