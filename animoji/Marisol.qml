import QtQuick
import QtQuick3D
import QtQuick.Timeline
Node {
    id: rOOT
    Node {
        id: rig
        x: 0.0442899
        y: -2.87973
        z: 0.269505
        Node {
            id: head
            rotation: Qt.quaternion(1, -3.7749e-08, 3.15289e-08, 1.19018e-15)
            Node {
                id: bone_001
                x: -1.19112
                y: 2.51095
                z: 1.90262
                rotation: Qt.quaternion(0.707787, 0.706426, 6.43012e-08, 1.26631e-07)
                Model {
                    id: eye_R
                    y: 1.19209e-07
                    z: 1.52737e-07
                    rotation: Qt.quaternion(0.759355, -0.635338, -0.11335, 0.0829266)
                    scale.y: 1
                    scale.z: 1
                    source: "meshes/eye_R.mesh"

                    PrincipledMaterial {
                        id: eye_material
                        roughness: 0.5
                        cullMode: Material.NoCulling
                        alphaMode: PrincipledMaterial.Opaque
                    }

                    PrincipledMaterial {
                        id: iris_material
                        baseColor: "#ff545454"
                        roughness: 0.5
                        cullMode: Material.NoCulling
                        alphaMode: PrincipledMaterial.Opaque
                    }

                    PrincipledMaterial {
                        id: pupil_material
                        baseColor: "#ff000000"
                        roughness: 0.5
                        cullMode: Material.NoCulling
                        alphaMode: PrincipledMaterial.Opaque
                    }
                    materials: [
                        eye_material,
                        iris_material,
                        pupil_material
                    ]
                }
                Model {
                    id: sphere
                    x: -0.00357258
                    y: -0.0949966
                    z: 0.00858893
                    rotation: Qt.quaternion(-0.00122017, 0.999999, -1.50648e-08, 1.34678e-07)
                    scale.x: 1.08882
                    scale.y: 1.08882
                    scale.z: 1.08882
                    source: "meshes/sphere.mesh"

                    PrincipledMaterial {
                        id: cornea_material
                        baseColor: "#ff000046"
                        roughness: 0.5
                        cullMode: Material.NoCulling
                        alphaMode: PrincipledMaterial.Opaque
                    }
                    materials: [
                        cornea_material
                    ]
                }
            }
            Node {
                id: bone_002
                x: 1.34093
                y: 2.51095
                z: 1.90262
                rotation: Qt.quaternion(0.707787, 0.706426, 5.24305e-08, 1.46012e-07)
                Model {
                    id: eye_L
                    x: -1.19209e-07
                    y: 2.38419e-07
                    z: 1.45286e-07
                    rotation: Qt.quaternion(0.759355, -0.635338, -0.11335, 0.0829266)
                    scale.y: 1
                    scale.z: 1
                    source: "meshes/eye_L.mesh"
                    materials: [
                        eye_material,
                        iris_material,
                        pupil_material
                    ]
                }
                Model {
                    id: sphere_001
                    x: -0.00379777
                    y: -0.0974116
                    z: 0.00602129
                    rotation: Qt.quaternion(-0.00459086, 0.999989, -1.45914e-08, 1.37354e-07)
                    scale.x: 1.08882
                    scale.y: 1.08882
                    scale.z: 1.08882
                    source: "meshes/sphere_001.mesh"
                    materials: [
                        cornea_material
                    ]
                }
            }
            Model {
                id: head_1
                x: 0.0710701
                y: 2.95486
                z: 0.178771
                rotation: Qt.quaternion(1, 3.56383e-08, -3.1529e-08, -1.19018e-15)
                source: "meshes/head.mesh"

                PrincipledMaterial {
                    id: skin_material
                    baseColor: "#ffffdfc9"
                    roughness: 0.5
                    cullMode: Material.NoCulling
                    alphaMode: PrincipledMaterial.Opaque
                }

                PrincipledMaterial {
                    id: mouth_material
                    baseColor: "#ff460009"
                    roughness: 0.5
                    cullMode: Material.NoCulling
                    alphaMode: PrincipledMaterial.Opaque
                }
                materials: [
                    skin_material,
                    mouth_material
                ]
            }
            Model {
                id: tooth_down
                x: 0.0781692
                y: 0.700354
                z: 2.45684
                rotation: Qt.quaternion(3.15257e-08, 4.54128e-10, 0.999896, -0.0144034)
                scale.x: -0.617358
                scale.y: -0.617358
                scale.z: -0.617358
                source: "meshes/tooth_down.mesh"

                PrincipledMaterial {
                    id: tooth_material
                    roughness: 0.5
                    cullMode: Material.NoCulling
                    alphaMode: PrincipledMaterial.Opaque
                }
                materials: [
                    tooth_material
                ]
            }
            Model {
                id: tooth_up
                x: 0.0781692
                y: 1.19862
                z: 2.48855
                rotation: Qt.quaternion(0.999896, -0.0144034, -3.15257e-08, -4.54128e-10)
                scale.x: 0.617358
                scale.y: 0.617358
                scale.z: 0.617358
                source: "meshes/tooth_up.mesh"
                materials: [
                    tooth_material
                ]
            }
        }
        Node {
            id: p
            x: 4.46172
            y: 3.1738
            rotation: Qt.quaternion(1, -3.7749e-08, 1.54252e-07, -5.82287e-15)
            Model {
                id: font_
                x: -0.46215
                y: 1.55272
                z: -0.00500935
                rotation: Qt.quaternion(0.707107, 0.707107, -1.09318e-07, 1.09073e-07)
                scale.x: 0.494222
                scale.y: 0.494221
                scale.z: 0.494221
                source: "meshes/font_.mesh"

                PrincipledMaterial {
                    id: _material
                    metalness: 1
                    roughness: 1
                    alphaMode: PrincipledMaterial.Opaque
                }
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: s
            x: 6.48481
            y: 3.1738
            rotation: Qt.quaternion(1, -3.7749e-08, 1.95053e-07, -7.36305e-15)
            Model {
                id: font_001
                x: -0.843583
                y: 1.55272
                z: -0.0050095
                rotation: Qt.quaternion(0.707107, 0.707107, -1.39611e-07, 1.37923e-07)
                scale.x: 0.900322
                scale.y: 0.900322
                scale.z: 0.900322
                source: "meshes/font_001.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: ey_R
            x: 8.50952
            y: 3.1738
            rotation: Qt.quaternion(-7.54979e-08, 1.54252e-07, 3.77489e-08, 1)
            Model {
                id: font_002
                x: 0.486419
                y: -1.91538
                z: -0.00500932
                rotation: Qt.quaternion(-5.59333e-08, 1.61756e-07, 0.707107, 0.707107)
                scale.x: 0.419
                scale.y: 0.419
                scale.z: 0.419
                source: "meshes/font_002.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: ey_L
            x: 9.50952
            y: 3.1738
            rotation: Qt.quaternion(-7.54979e-08, 1.58804e-07, 3.77489e-08, 1)
            Model {
                id: font_003
                x: 0.487409
                y: -1.91538
                z: -0.00500929
                rotation: Qt.quaternion(-5.98846e-08, 1.64975e-07, 0.707107, 0.707107)
                scale.x: 0.419
                scale.y: 0.419
                scale.z: 0.419
                source: "meshes/font_003.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: j
            x: 10.5205
            y: 3.1738
            rotation: Qt.quaternion(-7.54979e-08, 1.58804e-07, 3.77489e-08, 1)
            Model {
                id: font_004
                x: 0.388054
                y: -1.69789
                z: -0.00500932
                rotation: Qt.quaternion(-5.92261e-08, 1.64975e-07, 0.707107, 0.707107)
                scale.x: 0.590046
                scale.y: 0.590046
                scale.z: 0.590046
                source: "meshes/font_004.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: st
            x: 12.5113
            y: 3.1738
            rotation: Qt.quaternion(1, -3.7749e-08, 2.02227e-07, -7.63385e-15)
            Model {
                id: font_005
                x: -1.25775
                y: 1.53773
                z: -0.00500968
                rotation: Qt.quaternion(0.707107, 0.707107, -1.42246e-07, 1.42996e-07)
                scale.x: 0.878972
                scale.y: 0.878972
                scale.z: 0.878972
                source: "meshes/font_005.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: ey2_R
            x: 14.5195
            y: 3.1738
            rotation: Qt.quaternion(1, -3.7749e-08, 1.71737e-07, -6.48291e-15)
            Model {
                id: font_006
                x: -0.397053
                y: 1.77689
                z: -0.00500932
                rotation: Qt.quaternion(0.707107, 0.707107, -1.21172e-07, 1.21437e-07)
                scale.x: 0.300007
                scale.y: 0.300007
                scale.z: 0.300007
                source: "meshes/font_006.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: ey2_L
            x: 15.5195
            y: 3.1738
            z: -1.08312e-07
            rotation: Qt.quaternion(1, -3.7749e-08, 1.58804e-07, -5.99469e-15)
            Model {
                id: font_007
                x: -0.435114
                y: 1.77689
                z: -0.0050092
                rotation: Qt.quaternion(0.707107, 0.707107, -1.12611e-07, 1.12292e-07)
                scale.x: 0.309609
                scale.y: 0.309609
                scale.z: 0.309609
                source: "meshes/font_007.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: sn_R
            x: 4.46021
            y: 6.88481
            rotation: Qt.quaternion(1, -3.7749e-08, 1.71737e-07, -6.48291e-15)
            Model {
                id: font_008
                x: -0.378239
                y: 1.90567
                z: -0.00500953
                rotation: Qt.quaternion(0.707107, 0.707107, -1.42246e-07, 1.21437e-07)
                scale.x: 0.427322
                scale.y: 0.427322
                scale.z: 0.427322
                source: "meshes/font_008.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: sn_L
            x: 5.45101
            y: 6.88481
            rotation: Qt.quaternion(1, -3.7749e-08, 1.83763e-07, -6.93684e-15)
            Model {
                id: font_009
                x: -0.390984
                y: 1.90567
                z: -0.00500956
                rotation: Qt.quaternion(0.707107, 0.707107, -1.29075e-07, 1.2994e-07)
                scale.x: 0.427322
                scale.y: 0.427322
                scale.z: 0.427322
                source: "meshes/font_009.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: c
            x: 7.48583
            y: 6.88613
            rotation: Qt.quaternion(1, -3.7749e-08, 1.95053e-07, -7.36305e-15)
            Model {
                id: font_010
                x: -1.18447
                y: 1.55617
                z: -0.00500995
                rotation: Qt.quaternion(0.707107, 0.707107, -1.36977e-07, 1.37923e-07)
                scale.x: 0.908873
                scale.y: 0.908873
                scale.z: 0.908873
                source: "meshes/font_010.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: e_R
            x: 9.46269
            y: 6.90703
            z: -1.27783e-07
            rotation: Qt.quaternion(-7.54979e-08, 1.54252e-07, 3.77489e-08, 1)
            Model {
                id: font_011
                x: 0.46334
                y: -1.75361
                z: -0.00500944
                rotation: Qt.quaternion(-5.59333e-08, 1.61756e-07, 0.707107, 0.707107)
                scale.x: 0.263579
                scale.y: 0.263579
                scale.z: 0.263579
                source: "meshes/font_011.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: e_L
            x: 10.4627
            y: 6.90703
            rotation: Qt.quaternion(-7.54979e-08, 1.58804e-07, 3.77489e-08, 1)
            Model {
                id: font_012
                x: 0.416044
                y: -1.76227
                z: -0.00500956
                rotation: Qt.quaternion(-5.9226e-08, 1.64975e-07, 0.707107, 0.707107)
                scale.x: 0.240975
                scale.y: 0.240975
                scale.z: 0.240975
                source: "meshes/font_012.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: o
            x: 11.432
            y: 6.88481
            rotation: Qt.quaternion(1, -3.7749e-08, 1.83763e-07, -6.93684e-15)
            Model {
                id: font_013
                x: -0.389462
                y: 1.49832
                z: -0.00500959
                rotation: Qt.quaternion(0.707107, 0.707107, -1.29733e-07, 1.40476e-07)
                scale.x: 1.01965
                scale.y: 1.01965
                scale.z: 1.01965
                source: "meshes/font_013.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: f
            x: 12.4336
            y: 6.88481
            rotation: Qt.quaternion(1, -3.7749e-08, 1.83763e-07, -6.93684e-15)
            Model {
                id: font_014
                x: -0.257831
                y: 1.49832
                z: -0.00500953
                rotation: Qt.quaternion(0.707107, 0.707107, -1.29733e-07, 1.2994e-07)
                scale.x: 1.01965
                scale.y: 1.01965
                scale.z: 1.01965
                source: "meshes/font_014.mesh"
                materials: [
                    _material
                ]
            }
        }
        Node {
            id: c_001
            x: -6.40078
            y: 7.18282
            rotation: Qt.quaternion(1, -3.7749e-08, 2.28682e-07, -8.63252e-15)
            Model {
                id: font_015
                x: -2.50779
                y: 2.68188
                z: -0.00501049
                rotation: Qt.quaternion(0.707107, 0.707107, -1.43563e-07, 1.42996e-07)
                scale.x: 1.21576
                scale.y: 1.21576
                scale.z: 1.21576
                source: "meshes/font_015.mesh"
                materials: [
                    _material
                ]
            }
        }
    }
    Model {
        id: path
        source: "meshes/path.mesh"
        materials: [
            _material
        ]
    }
    Model {
        id: path_002
        source: "meshes/path_002.mesh"
        materials: [
            _material
        ]
    }
    Model {
        id: path_001
        source: "meshes/path_001.mesh"
        materials: [
            _material
        ]
    }

    Timeline {
        id: timeline0
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: true
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
                loops: Animation.Infinite
            }
        ]

        KeyframeGroup {
            target: eye_R
            property: "position"
            keyframeSource: "animations/eye_R_position.qad"
        }

        KeyframeGroup {
            target: eye_R
            property: "rotation"
            keyframeSource: "animations/eye_R_rotation.qad"
        }
    }

    Timeline {
        id: timeline1
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
                loops: Animation.Infinite
            }
        ]

        KeyframeGroup {
            target: eye_L
            property: "position"
            keyframeSource: "animations/eye_L_position.qad"
        }

        KeyframeGroup {
            target: eye_L
            property: "rotation"
            keyframeSource: "animations/eye_L_rotation.qad"
        }
    }

    Timeline {
        id: timeline2
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
                loops: Animation.Infinite
            }
        ]

        KeyframeGroup {
            target: head_1
            property: "position"
            keyframeSource: "animations/head_1_position.qad"
        }

        KeyframeGroup {
            target: head_1
            property: "rotation"
            keyframeSource: "animations/head_1_rotation.qad"
        }
    }
}
