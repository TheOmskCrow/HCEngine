#pragma once

/// Реализация некоторых классов работы с векторами и матрицами
#include "Vector\Vector3D.h"
#include "Matrix\Matrix4x4.h"

namespace hc3d {
	class Math
	{
	public:
		template<typename T, typename U>
		static T hc3dMax(T a, U b) {
			return a > b ? a : (T) b;
		}

		template<typename T, typename U>
		static T hc3dMin(T a, U b) {
			return a < b ? a : (T) b;
		}

		template<typename T>
		static T hc3dAbs(T a) {
			return a > 0 ? a : -a;
		}
	};
}