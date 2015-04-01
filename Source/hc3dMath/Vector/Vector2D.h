#pragma once
#include <math.h>
#include "dllmain.h"

namespace hc3d {

	class HC3D_API Vector2D
	{
	public:

		float   x;
		float   y;

		Vector2D() {}

		Vector2D(float r, float s)
		{
			x = r;
			y = s;
		}

		Vector2D& Set(float r, float s)
		{
			x = r;
			y = s;
			return (*this);
		}

		float& operator [](long k)
		{
			return ((&x)[k]);
		}

		const float& operator [](long k) const
		{
			return ((&x)[k]);
		}

		Vector2D& operator +=(const Vector2D& v)
		{
			x += v.x;
			y += v.y;
			return (*this);
		}

		Vector2D& operator -=(const Vector2D& v)
		{
			x -= v.x;
			y -= v.y;
			return (*this);
		}

		Vector2D& operator *=(float t)
		{
			x *= t;
			y *= t;
			return (*this);
		}

		Vector2D& operator /=(float t)
		{
			float f = 1.0F / t;
			x *= f;
			y *= f;
			return (*this);
		}

		Vector2D& operator &=(const Vector2D& v)
		{
			x *= v.x;
			y *= v.y;
			return (*this);
		}

		Vector2D operator -(void) const
		{
			return (Vector2D(-x, -y));
		}

		Vector2D operator +(const Vector2D& v) const
		{
			return (Vector2D(x + v.x, y + v.y));
		}

		Vector2D operator -(const Vector2D& v) const
		{
			return (Vector2D(x - v.x, y - v.y));
		}

		Vector2D operator *(float t) const
		{
			return (Vector2D(x * t, y * t));
		}

		Vector2D operator /(float t) const
		{
			float f = 1.0F / t;
			return (Vector2D(x * f, y * f));
		}

		float operator *(const Vector2D& v) const
		{
			return (x * v.x + y * v.y);
		}

		Vector2D operator &(const Vector2D& v) const
		{
			return (Vector2D(x * v.x, y * v.y));
		}

		bool operator ==(const Vector2D& v) const
		{
			return ((x == v.x) && (y == v.y));
		}

		bool operator !=(const Vector2D& v) const
		{
			return ((x != v.x) || (y != v.y));
		}

		Vector2D& Normalize(void)
		{
			return (*this /= sqrtf(x * x + y * y));
		}

		Vector2D& Rotate(float angle);
	};


	class HC3D_API Point2D : public Vector2D
	{
	public:

		Point2D() {}

		Point2D(float r, float s) : Vector2D(r, s) {}

		Point2D& operator =(const Vector2D& v)
		{
			x = v.x;
			y = v.y;
			return (*this);
		}

		Point2D& operator *=(float t)
		{
			x *= t;
			y *= t;
			return (*this);
		}

		Point2D& operator /=(float t)
		{
			float f = 1.0F / t;
			x *= f;
			y *= f;
			return (*this);
		}

		Point2D operator -(void) const
		{
			return (Point2D(-x, -y));
		}

		Point2D operator +(const Vector2D& v) const
		{
			return (Point2D(x + v.x, y + v.y));
		}

		Point2D operator -(const Vector2D& v) const
		{
			return (Point2D(x - v.x, y - v.y));
		}

		Vector2D operator -(const Point2D& p) const
		{
			return (Vector2D(x - p.x, y - p.y));
		}

		Point2D operator *(float t) const
		{
			return (Point2D(x * t, y * t));
		}

		Point2D operator /(float t) const
		{
			float f = 1.0F / t;
			return (Point2D(x * f, y * f));
		}
	};


	inline HC3D_API Vector2D operator *(float t, const Vector2D& v)
	{
		return (Vector2D(t * v.x, t * v.y));
	}

	inline HC3D_API Point2D operator *(float t, const Point2D& p)
	{
		return (Point2D(t * p.x, t * p.y));
	}

	inline HC3D_API float Dot(const Vector2D& v1, const Vector2D& v2)
	{
		return (v1 * v2);
	}

	inline HC3D_API float Magnitude(const Vector2D& v)
	{
		return (sqrtf(v.x * v.x + v.y * v.y));
	}

	inline HC3D_API float InverseMag(const Vector2D& v)
	{
		return (1.0F / sqrtf(v.x * v.x + v.y * v.y));
	}

	inline HC3D_API float SquaredMag(const Vector2D& v)
	{
		return (v.x * v.x + v.y * v.y);
	}

}