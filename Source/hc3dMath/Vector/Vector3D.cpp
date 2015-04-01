#pragma once
#include "stdafx.h"
#include "Vector3D.h"

using namespace hc3d;

Vector3D& Vector3D::normal(Vector3D A, Vector3D B, Vector3D C) {
    x = A.y*(B.z - C.z) + B.y*(C.z - A.z) + C.y*(A.z - B.z);
    y = A.z*(B.x - C.x) + B.z*(C.x - A.x) + C.z*(A.x - B.x);
    z = A.x*(B.y - C.y) + B.x*(C.y - A.y) + C.x*(A.y - B.y);
    return (*this);
}
Vector3D& Vector3D::RotateAboutX(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    
    float ny = c * y - s * z;
    float nz = c * z + s * y;
    
    y = ny;
    z = nz;
    
    return (*this);
}

Vector3D& Vector3D::RotateAboutY(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    
    float nx = c * x + s * z;
    float nz = c * z - s * x;
    
    x = nx;
    z = nz;
    
    return (*this);
}

Vector3D& Vector3D::RotateAboutZ(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    
    float nx = c * x - s * y;
    float ny = c * y + s * x;
    
    x = nx;
    y = ny;
    
    return (*this);
}

Vector3D& Vector3D::RotateAboutAxis(float angle, const Vector3D& axis)
{
    float s = sinf(angle);
    float c = cosf(angle);
    float k = 1.0F - c;
    
    float nx = x * (c + k * axis.x * axis.x) + y * (k * axis.x * axis.y - s * axis.z)
            + z * (k * axis.x * axis.z + s * axis.y);
    float ny = x * (k * axis.x * axis.y + s * axis.z) + y * (c + k * axis.y * axis.y)
            + z * (k * axis.y * axis.z - s * axis.x);
    float nz = x * (k * axis.x * axis.z - s * axis.y) + y * (k * axis.y * axis.z + s * axis.x)
            + z * (c + k * axis.z * axis.z);
    
    x = nx;
    y = ny;
    z = nz;
    
    return (*this);
}