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

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.bottomMargin: 10

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
            function onImageChanged() {
                webcam.reload()
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
