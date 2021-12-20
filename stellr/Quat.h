#pragma once
#include "CoordinateFrame.h"

namespace g3dimpl {
	class Quat {
	public:
		float x, y, z, w;
		Quat() : x(0), y(0), z(0), w(1) {}
		Quat(const Matrix3& rot) {
			// Implementation from Util/Quaternion.h
			float tr = rot[0][0] + rot[1][1] + rot[2][2];
			if (tr > 0.0) {
				float s = sqrt(tr + 1.0f);
				w = s * 0.5f;
				s = 0.5f / s;
				x = (rot[2][1] - rot[1][2]) * s;
				y = (rot[0][2] - rot[2][0]) * s;
				z = (rot[1][0] - rot[0][1]) * s;
			}
			else if ((rot[0][0] > rot[1][1]) && (rot[0][0] > rot[2][2])) {
				float s = -sqrt(1.0f + rot[0][0] - rot[1][1] - rot[2][2]);
				x = s * 0.5f;
				s = 0.5f / s;
				y = (rot[0][1] + rot[1][0]) * s;		// i == 0, j == 1, k == 2
				z = (rot[0][2] + rot[2][0]) * s;
				w = (rot[2][1] - rot[1][2]) * s;		// kj - jk == 21 - 12
			}
			else if (rot[1][1] > rot[2][2]) {
				float s = -sqrt(1.0f + rot[1][1] - rot[0][0] - rot[2][2]);
				y = s * 0.5f;
				s = 0.5f / s;
				x = (rot[0][1] + rot[1][0]) * s;		// i == 1, j == 2, k == 0
				z = (rot[1][2] + rot[2][1]) * s;
				w = (rot[0][2] - rot[2][0]) * s;		// kj - jk == 02 - 20
			}
			else {
				float s = -sqrt(1.0f + rot[2][2] - rot[0][0] - rot[1][1]);
				z = s * 0.5f;
				s = 0.5f / s;
				x = (rot[0][2] + rot[2][0]) * s;		// i == 2, j == 0, k == 1
				y = (rot[1][2] + rot[2][1]) * s;
				w = (rot[1][0] - rot[0][1]) * s;		// kj - jk == 10 - 01
			}
		}
		Quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		Quat(const Vector3& v, float _w = 0) : x(v.x), y(v.y), z(v.z), w(_w) {}
		explicit Quat(const float values[4]) : x(values[0]), y(values[1]), z(values[2]), w(values[3]) {}

		const float& real() const {
			return w;
		}

		float& real() {
			return w;
		}

		Quat operator-() const {
			return Quat(-x, -y, -z, -w);
		}

		Quat operator-(const Quat& other) const {
			return Quat(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		Quat& operator-=(const Quat& q) {
			x -= q.x;
			y -= q.y;
			z -= q.z;
			w -= q.w;
			return *this;
		}

		Quat operator+(const Quat& q) const {
			return Quat(x + q.x, y + q.y, z + q.z, w + q.w);
		}

		Quat& operator+=(const Quat& q) {
			x += q.x;
			y += q.y;
			z += q.z;
			w += q.w;
			return *this;
		}

		Quat conj() const {
			return Quat(-x, -y, -z, w);
		}

		float sum() const {
			return x + y + z + w;
		}

		float average() const {
			return sum() / 4.0f;
		}

		Quat operator*(float s) const {
			return Quat(x * s, y * s, z * s, w * s);
		}

		Quat& operator*=(float s) {
			x *= s;
			y *= s;
			z *= s;
			w *= s;
			return *this;
		}

		friend Quat operator*(float s, const Quat& q) {
			return q * s;
		}

		inline Quat operator/(float s) const {
			return Quat(x / s, y / s, z / s, w / s);
		}

		float dot(const Quat& other) const {
			return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
		}

		inline float rsq(float x) {
			return 1.0f / sqrtf(x);
		}

		void unitize() {
			*this *= rsq(dot(*this));
		}

		float magnitude() const {
			return sqrtf(dot(*this));
		}

		Quat toUnit() const {
			Quat x = *this;
			x.unitize();
			return x;
		}

		const Vector3& imag() const {
			return *(reinterpret_cast<const Vector3*>(this));
		}

		Vector3& imag() {
			return *(reinterpret_cast<Vector3*>(this));
		}

		Quat fromAxisAngleRotation(const Vector3& axis, float angle) {
			Quat q;

			q.w = cos(angle / 2.0f);
			q.imag() = axis.direction() * sin(angle / 2.0f);

			return q;
		}

		Quat fromRotation(const Vector3& rotation)
		{
			float angle = rotation.magnitude();

			if (angle <= 1e-6)
				return Quat();

			return Quat::fromAxisAngleRotation(rotation / angle, angle);
		}

		Quat fromVectors(const Vector3& from, const Vector3& to)
		{
			Vector3 u = from.unit();
			Vector3 v = to.unit();
			Vector3 w = u.cross(v);
			Quat q(w, 1 + u.dot(v));
			return q.toUnit();
		}

		Matrix3 toRotationMatrix() const {
			Matrix3 out = Matrix3::zero();

			toRotationMatrix(out);

			return out;
		}

		void toRotationMatrix(Matrix3& rot) const {
			rot.setMatrix3<Quat>(*this);
		}

		Quat slerp(const Quat& other, float alpha, float threshold = 0.05f) const {
			const Quat& quat0 = *this;
			Quat quat1 = other;

			float phi;
			float cosphi = quat0.dot(quat1);

			if (cosphi < 0) {
				quat1 = -quat1;
				cosphi = -cosphi;
			}

			phi = acosf(cosphi);

			if (phi >= threshold) {
				float scale0 = sin((1.f - alpha) * phi), scale1 = sin(alpha * phi);

				return ((quat0 * scale0) + (quat1 * scale1)) / sin(phi);
			}
			else return quat0.nlerp(quat1, alpha);
		}

		Quat nlerp(const Quat& other, float alpha) const {
			Quat result = (*this) * (1.f - alpha) + other * alpha;
			return result / result.magnitude();
		}
	};
}