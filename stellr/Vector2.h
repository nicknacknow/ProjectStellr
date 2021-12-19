#pragma once
#include <math.h>

namespace g3dimpl {
	class Vector2 {
	public:
		float x, y;

		inline Vector2(float _x, float _y) : x(_x), y(_y) {}
		inline Vector2() : x(0.0f), y(0.0f) {}

		inline bool operator== (const Vector2& rkVector) const {
			return (x == rkVector.x && y == rkVector.y);
		}

		inline bool operator!= (const Vector2& rkVector) const {
			return (x != rkVector.x || y != rkVector.y);
		}

		inline Vector2 operator+ (const Vector2& rkVector) const {
			return Vector2(x + rkVector.x, y + rkVector.y);
		}

		inline Vector2 operator- (const Vector2& rkVector) const {
			return Vector2(x - rkVector.x, y - rkVector.y);
		}

		inline Vector2 operator* (float fScalar) const {
			return Vector2(fScalar * x, fScalar * y);
		}

		inline Vector2 operator- () const {
			return Vector2(-x, -y);
		}

		inline Vector2 operator* (const Vector2& rkVector) const {
			return Vector2(x * rkVector.x, y * rkVector.y);
		}

		inline Vector2 operator/ (const Vector2& rkVector) const {
			return Vector2(x / rkVector.x, y / rkVector.y);
		}

		inline Vector2 operator/ (float k) const {
			return *this * (1.0f / k);
		}

		inline float squaredLength() const {
			return x * x + y * y;
		}

		inline float length() const {
			return sqrtf(x * x + y * y);
		}

		inline Vector2 direction() const {
			float lenSquared = x * x + y * y;

			if (lenSquared != 1.0f) {
				return *this / sqrtf(lenSquared);
			}
			else {
				return *this;
			}
		}

		inline float dot(const Vector2& rkVector) const {
			return x * rkVector.x + y * rkVector.y;
		}

		inline Vector2 min(const Vector2& v) const {
			return Vector2(fmin(v.x, x), fmin(v.y, y));
		}

		inline Vector2 max(const Vector2& v) const {
			return Vector2(fmax(v.x, x), fmax(v.y, y));
		}
	private:

	};
}