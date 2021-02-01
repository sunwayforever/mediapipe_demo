import QtQuick
import QtQuick3D
Node {
    id: marisol_obj
    Model {
        id: font_015_Text_015
        source: "meshes/font_015_Text_015.mesh"

        DefaultMaterial {
            id: none_material
            diffuseColor: "#ff999999"
        }
        materials: [
            none_material
        ]
    }
    Model {
        id: font_006_Text_006
        source: "meshes/font_006_Text_006.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_007_Text_007
        source: "meshes/font_007_Text_007.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_Text
        source: "meshes/font_Text.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_001_Text_001
        source: "meshes/font_001_Text_001.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_002_Text_002
        source: "meshes/font_002_Text_002.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_003_Text_003
        source: "meshes/font_003_Text_003.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_004_Text_004
        source: "meshes/font_004_Text_004.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_005_Text_005
        source: "meshes/font_005_Text_005.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_014_Text_014
        source: "meshes/font_014_Text_014.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_013_Text_013
        source: "meshes/font_013_Text_013.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_012_Text_012
        source: "meshes/font_012_Text_012.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_011_Text_011
        source: "meshes/font_011_Text_011.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_010_Text_010
        source: "meshes/font_010_Text_010.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_009_Text_009
        source: "meshes/font_009_Text_009.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: font_008_Text_008
        source: "meshes/font_008_Text_008.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: tooth_up_Cube
        source: "meshes/tooth_up_Cube.mesh"

        DefaultMaterial {
            id: tooth_material
            diffuseColor: "#ff999999"
        }
        materials: [
            tooth_material
        ]
    }
    Model {
        id: eye_R_cl_head_LOW_001
        source: "meshes/eye_R_cl_head_LOW_001.mesh"

        DefaultMaterial {
            id: eye_material
            diffuseColor: "#ff999999"
        }

        DefaultMaterial {
            id: iris_material
            diffuseColor: "#ff999999"
        }

        DefaultMaterial {
            id: pupil_material
            diffuseColor: "#ff999999"
        }
        materials: [
            eye_material,
            iris_material,
            pupil_material
        ]
    }
    Model {
        id: eye_L_cl_head_LOW_001
        source: "meshes/eye_L_cl_head_LOW_001.mesh"
        materials: [
            eye_material,
            iris_material,
            pupil_material
        ]
    }
    Model {
        id: head_cl_head_LOW_002
        source: "meshes/head_cl_head_LOW_002.mesh"

        DefaultMaterial {
            id: skin_material
            diffuseColor: "#ff999999"
        }

        DefaultMaterial {
            id: mouth_material
            diffuseColor: "#ff999999"
        }
        materials: [
            skin_material,
            mouth_material
        ]
    }
    Model {
        id: tooth_down_Cube_001
        source: "meshes/tooth_down_Cube_001.mesh"
        materials: [
            tooth_material
        ]
    }
    Model {
        id: sphere_001
        source: "meshes/sphere_001.mesh"

        DefaultMaterial {
            id: cornea_material
            diffuseColor: "#ff999999"
        }
        materials: [
            cornea_material
        ]
    }
    Model {
        id: sphere
        source: "meshes/sphere.mesh"
        materials: [
            cornea_material
        ]
    }
    Model {
        id: path_Plane_002
        source: "meshes/path_Plane_002.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: path_002_Circle_001
        source: "meshes/path_002_Circle_001.mesh"
        materials: [
            none_material
        ]
    }
    Model {
        id: path_001_Plane
        source: "meshes/path_001_Plane.mesh"
        materials: [
            none_material
        ]
    }
}
