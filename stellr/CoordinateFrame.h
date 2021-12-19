#pragma once

#include "Matrix3.h"

namespace g3dimpl {
	class CoordinateFrame {
	public:
		Matrix3 rotation;
		Point3 translation;

		CoordinateFrame() : rotation(Matrix3::identity()), translation(Vector3::zero()) {}
		CoordinateFrame(const Point3& _translation) : rotation(Matrix3::identity()), translation(_translation) {}
		CoordinateFrame(const Matrix3& rotation, const Point3& translation) : rotation(rotation), translation(translation) {}
		CoordinateFrame(const Matrix3& rotation) : rotation(rotation), translation(Point3::zero()) {}
		CoordinateFrame(const CoordinateFrame& other) : rotation(other.rotation), translation(other.translation) {}
		inline ~CoordinateFrame() {}

		inline bool operator==(const CoordinateFrame& other) const {
			return (translation == other.translation) && (rotation == other.rotation);
		}

		inline bool operator!=(const CoordinateFrame& other) const {
			return !(*this == other);
		}

		CoordinateFrame operator*(const CoordinateFrame& other) const {
			return CoordinateFrame(rotation * other.rotation,
				pointToWorldSpace(other.translation));
		}

		CoordinateFrame operator+(const Vector3& v) const {
			return CoordinateFrame(rotation, translation + v);
		}

		CoordinateFrame operator-(const Vector3& v) const {
			return CoordinateFrame(rotation, translation - v);
		}

		inline Point3 pointToWorldSpace(const Point3& v) const {
			return Point3
			(rotation[0][0] * v[0] + rotation[0][1] * v[1] + rotation[0][2] * v[2] + translation[0],
				rotation[1][0] * v[0] + rotation[1][1] * v[1] + rotation[1][2] * v[2] + translation[1],
				rotation[2][0] * v[0] + rotation[2][1] * v[1] + rotation[2][2] * v[2] + translation[2]);
		}

		inline Point3 pointToObjectSpace(const Point3& v) const {
			float p[3];
			p[0] = v[0] - translation[0];
			p[1] = v[1] - translation[1];
			p[2] = v[2] - translation[2];
			return Point3(rotation[0][0] * p[0] + rotation[1][0] * p[1] + rotation[2][0] * p[2],
				rotation[0][1] * p[0] + rotation[1][1] * p[1] + rotation[2][1] * p[2],
				rotation[0][2] * p[0] + rotation[1][2] * p[1] + rotation[2][2] * p[2]);
		}

		inline CoordinateFrame inverse() const {
			CoordinateFrame out;
			out.rotation = rotation.transpose();
			out.translation = -out.rotation * translation;
			return out;
		}

		inline CoordinateFrame toObjectSpace(const CoordinateFrame& c) const {
			return this->inverse() * c;
		}

		inline Vector3 vectorToWorldSpace(const Vector3& v) const {
			return rotation * v;
		}

		inline Vector3 normalToWorldSpace(const Vector3& v) const {
			return rotation * v;
		}

		void lookAt(const Vector3& target) {
			lookAt(target, Vector3::unitY());
		}

		void lookAt(const Vector3& target, Vector3 up) {
			up = up.direction();

			const Vector3 look = (target - translation).direction();
			if (fabs(look.dot(up)) > 0.99f) {
				up = Vector3::unitX();
				if (fabs(look.dot(up)) > 0.99f) {
					up = Vector3::unitY();
				}
			}

			up -= look * look.dot(up);
			up = up.direction();

			const Vector3 z = -look;
			Vector3 x = -z.cross(up);
			x = x.direction();

			const Vector3 y = z.cross(x);

			rotation.setColumn(0, x);
			rotation.setColumn(1, y);
			rotation.setColumn(2, z);
		}
	};
}