#pragma once
#include <Windows.h>
#include <math.h>

namespace g3dimpl {
	class Vector3 {
	public:
		float x, y, z;

		inline Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		inline Vector3(float fX, float fY, float fZ) : x(fX), y(fY), z(fZ) {}

		inline float& operator[] (int i) {
			return ((float*)this)[i];
		}

		inline const float& operator[] (int i) const {
			return ((float*)this)[i];
		}

		inline Vector3& operator= (const Vector3& rkVector) {
			x = rkVector.x;
			y = rkVector.y;
			z = rkVector.z;
			return *this;
		}

		inline bool operator==(const Vector3& rkVector) const {
			return (x == rkVector.x && y == rkVector.y && z == rkVector.z);
		}

		inline bool operator!= (const Vector3& rkVector) const {
			return (x != rkVector.x || y != rkVector.y || z != rkVector.z);
		}

		inline Vector3 operator+ (const Vector3& rkVector) const {
			return Vector3(x + rkVector.x, y + rkVector.y, z + rkVector.z);
		}

		inline Vector3 operator- (const Vector3& rkVector) const {
			return Vector3(x - rkVector.x, y - rkVector.y, z - rkVector.z);
		}

		inline Vector3 operator* (const Vector3& rkVector) const {
			return Vector3(x * rkVector.x, y * rkVector.y, z * rkVector.z);
		}

		inline Vector3 operator*(float f) const {
			return Vector3(x * f, y * f, z * f);
		}

		inline Vector3 operator/ (const Vector3& rkVector) const {
			return Vector3(x / rkVector.x, y / rkVector.y, z / rkVector.z);
		}

		inline Vector3 operator- () const {
			return Vector3(-x, -y, -z);
		}

		inline Vector3& operator+= (const Vector3& rkVector) {
			x += rkVector.x;
			y += rkVector.y;
			z += rkVector.z;
			return *this;
		}

		inline Vector3& operator-= (const Vector3& rkVector) {
			x -= rkVector.x;
			y -= rkVector.y;
			z -= rkVector.z;
			return *this;
		}

		inline float squaredMagnitude() const {
			return x * x + y * y + z * z;
		}

		inline float magnitude() const {
			return sqrtf(squaredMagnitude());
		}

		inline Vector3 direction() const {
			const float lenSquared = squaredMagnitude();
			const float invSqrt = 1.0f / sqrtf(lenSquared);
			return Vector3(x * invSqrt, y * invSqrt, z * invSqrt);
		}

		inline float dot(const Vector3& rkVector) const {
			return x * rkVector.x + y * rkVector.y + z * rkVector.z;
		}

		inline Vector3 cross(const Vector3& rkVector) const {
			return Vector3(y * rkVector.z - z * rkVector.y, z * rkVector.x - x * rkVector.z,
				x * rkVector.y - y * rkVector.x);
		}

		inline Vector3 unitCross(const Vector3& rkVector) const {
			Vector3 kCross(y * rkVector.z - z * rkVector.y, z * rkVector.x - x * rkVector.z,
				x * rkVector.y - y * rkVector.x);
			return kCross.direction();
		}

		inline Vector3 unit() const {
			return direction();
		}

		static const Vector3& zero() { static const Vector3 v(0, 0, 0); return v; }
		static const Vector3& one() { static const Vector3 v(1, 1, 1); return v; }
		static const Vector3& unitX() { static const Vector3 v(1, 0, 0); return v; }
		static const Vector3& unitY() { static const Vector3 v(0, 1, 0); return v; }
		static const Vector3& unitZ() { static const Vector3 v(0, 0, 1); return v; }

		const char* tostring() {
			char* buff = new char;
			sprintf(buff, "{ %f %f %f }", x, y, z);
			return buff;
		}

#undef min
		inline Vector3 min(const Vector3& v) const {
			return Vector3(fmin(v.x, x), fmin(v.y, y), fmin(v.z, z));
		}
#undef max
		inline Vector3 max(const Vector3& v) const {
			return Vector3(fmax(v.x, x), fmax(v.y, y), fmax(v.z, z));
		}
	};

	typedef Vector3 Point3;
}