import QtQuick
import QtQuick3D
import QtQuick.Controls

Window {
    id: window
    width: 1280
    height: 720
    visible: true
    title: "View3Ds with Different Cameras"
    color: "#848895"

    // The root scene
    //! [rootnode]
    QtObject {
        id: control
        property int y:0
    }
    Node {
        id: standAloneScene
        //! [rootnode]

        DirectionalLight {
            ambientColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            brightness: 1.0
            eulerRotation.x: -25
        }

        Model {
            source: "#Cube"
            y: -104
            scale: Qt.vector3d(3, 3, 0.1)
            eulerRotation.x: -90
            materials: [
                DefaultMaterial {
                    diffuseColor: Qt.rgba(0.8, 0.8, 0.8, 1.0)
                }
            ]
        }

        Marisol {
            id: boy
            scale: Qt.vector3d(100, 100, 100)
            /*
             * control:control
             */
        }

        // Stationary orthographic camera viewing from the front
        OrthographicCamera {
            id: cameraOrthographicFront
            z: 600
        }

    }
    //! [cameras end]

    //! [views]
    // The views
    Rectangle {
        id: topLeft
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: parent.height
        color: "#848895"
        border.color: "black"
        focus: true

        Keys.onPressed: {
            control.y+=10
            event.accepted = true;
        }

        View3D {
            id: topLeftView
            anchors.fill: parent
            importScene: standAloneScene
            camera: cameraOrthographicFront
        }

    }
}
