#include "StdAfx.h"
#include "Matrix4x4.h"

using namespace hc3d;
	

	Matrix4x4::Matrix4x4(void) {
		Matrix4x4::setToIdentity();
	}

	void Matrix4x4::set(float a0, float a1, float a2,float a3,float a4,float a5,float a6,float a7,float a8,float a9,float a10,float a11,float a12,float a13,float a14,float a15) {
		m[0] = a0; m[1] = a1; m[2] = a2; m[3] = a3; m[4] = a4; m[5] = a5;
		m[6] = a6; m[7] = a7; m[8] = a8; m[9] = a9; m[10] = a10; m[11] = a11;
		m[12] = a12; m[13] = a13; m[14] = a14; m[15] = a15;
	}
	void Matrix4x4::setToIdentity() {
		m[ 0] = 1; m[ 4] = 0; m[ 8] = 0; m[12] = 0;
		m[ 1] = 0; m[ 5] = 1; m[ 9] = 0; m[13] = 0;
		m[ 2] = 0; m[ 6] = 0; m[10] = 1; m[14] = 0;
		m[ 3] = 0; m[ 7] = 0; m[11] = 0; m[15] = 1;
	}

	void Matrix4x4::copy( Matrix4x4 M ) {
		m[ 0] = M.m[ 0]; m[ 4] = M.m[ 4]; m[ 8] = M.m[ 8]; m[12] = M.m[12];
		m[ 1] = M.m[ 1]; m[ 5] = M.m[ 5]; m[ 9] = M.m[ 9]; m[13] = M.m[13];
		m[ 2] = M.m[ 2]; m[ 6] = M.m[ 6]; m[10] = M.m[10]; m[14] = M.m[14];
		m[ 3] = M.m[ 3]; m[ 7] = M.m[ 7]; m[11] = M.m[11]; m[15] = M.m[15];
	}

	 void Matrix4x4::setToTranslation( Vector3D v ) {
		m[ 0] = 1; m[ 4] = 0; m[ 8] = 0; m[12] = v.x;
		m[ 1] = 0; m[ 5] = 1; m[ 9] = 0; m[13] = v.y;
		m[ 2] = 0; m[ 6] = 0; m[10] = 1; m[14] = v.z;
		m[ 3] = 0; m[ 7] = 0; m[11] = 0; m[15] = 1;
	}

	

	
	// return the product of the given matrices
	Matrix4x4 Matrix4x4::mult( Matrix4x4 b) {
		Matrix4x4 M;
		M.m[ 0] = m[ 0]*b.m[ 0] + m[ 4]*b.m[ 1] + m[ 8]*b.m[ 2] + m[12]*b.m[ 3];
		M.m[ 1] = m[ 1]*b.m[ 0] + m[ 5]*b.m[ 1] + m[ 9]*b.m[ 2] + m[13]*b.m[ 3];
		M.m[ 2] = m[ 2]*b.m[ 0] + m[ 6]*b.m[ 1] + m[10]*b.m[ 2] + m[14]*b.m[ 3];
		M.m[ 3] = m[ 3]*b.m[ 0] + m[ 7]*b.m[ 1] + m[11]*b.m[ 2] + m[15]*b.m[ 3];

		M.m[ 4] = m[ 0]*b.m[ 4] + m[ 4]*b.m[ 5] + m[ 8]*b.m[ 6] + m[12]*b.m[ 7];
		M.m[ 5] = m[ 1]*b.m[ 4] + m[ 5]*b.m[ 5] + m[ 9]*b.m[ 6] + m[13]*b.m[ 7];
		M.m[ 6] = m[ 2]*b.m[ 4] + m[ 6]*b.m[ 5] + m[10]*b.m[ 6] + m[14]*b.m[ 7];
		M.m[ 7] = m[ 3]*b.m[ 4] + m[ 7]*b.m[ 5] + m[11]*b.m[ 6] + m[15]*b.m[ 7];

		M.m[ 8] = m[ 0]*b.m[ 8] + m[ 4]*b.m[ 9] + m[ 8]*b.m[10] + m[12]*b.m[11];
		M.m[ 9] = m[ 1]*b.m[ 8] + m[ 5]*b.m[ 9] + m[ 9]*b.m[10] + m[13]*b.m[11];
		M.m[10] = m[ 2]*b.m[ 8] + m[ 6]*b.m[ 9] + m[10]*b.m[10] + m[14]*b.m[11];
		M.m[11] = m[ 3]*b.m[ 8] + m[ 7]*b.m[ 9] + m[11]*b.m[10] + m[15]*b.m[11];

		M.m[12] = m[ 0]*b.m[12] + m[ 4]*b.m[13] + m[ 8]*b.m[14] + m[12]*b.m[15];
		M.m[13] = m[ 1]*b.m[12] + m[ 5]*b.m[13] + m[ 9]*b.m[14] + m[13]*b.m[15];
		M.m[14] = m[ 2]*b.m[12] + m[ 6]*b.m[13] + m[10]*b.m[14] + m[14]*b.m[15];
		M.m[15] = m[ 3]*b.m[12] + m[ 7]*b.m[13] + m[11]*b.m[14] + m[15]*b.m[15];
		return M;
	}

	// return the product of the given matrix and vector
	Vector3D Matrix4x4::mult(Vector3D b ) {
		// We treat the vector as if
		// its (homogeneous) 4th component were zero.
		Vector3D c;
		c.Set(
			m[ 0]*b.x + m[ 4]*b.y + m[ 8]*b.z  + m[12],
			m[ 1]*b.x + m[ 5]*b.y + m[ 9]*b.z  + m[13],
			m[ 2]*b.x + m[ 6]*b.y + m[10]*b.z   + m[14]
			 //m[ 3]*b.x() + m[ 7]*b.y() + m[11]*b.z() + m[15]
		);
		return c;
	}

	// return the product of the given matrix and point
