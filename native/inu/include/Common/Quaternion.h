//Quaternion.hpp: Class for representing and manipulating rotation Quaternions
#pragma once
#ifndef NO_ALGO
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "CommonAlgo.h"
#include "OSUtilities.h"


class COMMONALGO_API Quaternion
{
public:
        
    float x;
    float y;
    float z;
    float w;
    
    Quaternion():
        x(0.0), y(0.0), z(0.0), w(1.0)
    {
    }
    
    enum Quat_Type {xRot, yRot, zRot};
    
    Quaternion(float x, float y, float z, float w);

    Quaternion(cv::Point3f axis, float angle);
    
    Quaternion(cv::Point3f eulerAngles);
    
    Quaternion(Quat_Type type, float angle);

    Quaternion operator-(const Quaternion &other) const;

    Quaternion operator*(const Quaternion &other) const;

	inline cv::Point3f operator*(const cv::Point3f & vect) const
    {
        // nVidia SDK implementation
        cv::Point3f uv, uuv;
        cv::Point3f qvec(x, y, z);

        uv = qvec.cross(vect);
        uuv = qvec.cross(uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;

        return vect + uv + uuv;
    }

    std::string ToString();

    Quaternion Conjugate() const;
    
    void Normalize();
};
#endif
