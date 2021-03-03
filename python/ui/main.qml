import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick3D 1.15

ApplicationWindow {
    id: applicationWindow
    visible: true

    RowLayout {
        id: rowLayout
        anchors.fill: parent
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

            GroupBox {
                id: groupBox
                width: 200
                height: 200
                font.bold: true
                padding: 10
                Layout.fillWidth: true
                title: qsTr("Expression")

                ColumnLayout {
                    id: columnLayout2
                    anchors.fill: parent

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
            }

            GroupBox {
                id: groupBox1
                width: 200
                height: 200
                Layout.fillWidth: true
                font.bold: true
                title: qsTr("Hand Gesture")

                ColumnLayout {
                    id: columnLayout3
                    anchors.fill: parent

                    Image {
                        id: hand
                        cache: false
                        width: 100
                        height: 100
                        source: "image://hand"
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.fillWidth: true
                        fillMode: Image.PreserveAspectFit
                        function reload() {
                            source=""
                            source="image://hand"
                        }
                    }
                }
            }
        }


        GroupBox {
            id: groupBox3
            width: 200
            height: 200
            font.bold: true
            Layout.margins: 10
            Layout.preferredWidth: 100
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: qsTr("Video")

            ColumnLayout {
                id: columnLayout5
                anchors.fill: parent

                Image {
                    id: webcam
                    cache: false
                    width: 100
                    height: 100
                    source: "image://webcam"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.margins: 0
                    Layout.preferredWidth: -1
                    Layout.fillHeight: true
                    Layout.preferredHeight: -1
                    Layout.fillWidth: true
                    fillMode: Image.PreserveAspectFit
                    function reload() {
                        source=""
                        source="image://webcam"
                    }
                }
            }
        }

        ColumnLayout {
            id: columnLayout1
            width: 100
            height: 100
            Layout.margins: 10
            Layout.fillHeight: true
            Layout.preferredWidth: 20
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignTop

            GroupBox {
                id: groupBox2
                width: 200
                height: 200
                Layout.margins: 0
                font.bold: true
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                title: qsTr("Face Recognizer")

                ColumnLayout {
                    id: columnLayout4
                    anchors.fill: parent

                    Image {
                        id: facenet
                        width: 100
                        height: 100
                        cache: false
                        source: "image://facenet"
                        Layout.preferredHeight: 160
                        Layout.margins: 0
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
            }


            Button {
                id: enroll
                text: qsTr("ENROLL")
                font.underline: false
                font.bold: true
                Layout.topMargin: 5
                Layout.margins: 0
                enabled: false
                padding: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                onClicked: {
                    backend.enroll()
                }
            }

            Item {
                id: item1
                width: 200
                height: 200
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            View3D {
                id: view3D
                width: 300
                height: 300
                scale: 1
                clip: false
                Layout.preferredWidth: -1
                Layout.fillHeight: true
                transformOrigin: Item.Center
                Layout.margins: 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.preferredHeight: 160
                Layout.fillWidth: true
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



        }

        Connections {
            target: backend
            function onWebcamImageChanged() {
                webcam.reload()
            }

            function onFacenetImageChanged(enrolled) {
                facenet.reload()
                enroll.enabled=!enrolled
            }

            function onHandImageChanged() {
                hand.reload()
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
    D{i:0;autoSize:true;height:480;width:640}D{i:12}D{i:16}
}
##^##*/
