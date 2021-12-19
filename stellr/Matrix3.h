#pragma once
#include "Vector3.h"

namespace g3dimpl {
    class Matrix3 {
    private:
        float elt[3][3]; // Row, column
    public:
        Matrix3() {}

        Matrix3(const float aafEntry[3][3]) {
            memcpy(elt, aafEntry, 9 * sizeof(float));
        }

        Matrix3(const Matrix3& rkMatrix) {
            memcpy(elt, rkMatrix.elt, 9 * sizeof(float));
        }

        Matrix3(
            float fEntry00, float fEntry01, float fEntry02,
            float fEntry10, float fEntry11, float fEntry12,
            float fEntry20, float fEntry21, float fEntry22) {
            set(fEntry00, fEntry01, fEntry02,
                fEntry10, fEntry11, fEntry12,
                fEntry20, fEntry21, fEntry22);
        }

        void set(
            float fEntry00, float fEntry01, float fEntry02,
            float fEntry10, float fEntry11, float fEntry12,
            float fEntry20, float fEntry21, float fEntry22) {

            elt[0][0] = fEntry00;
            elt[0][1] = fEntry01;
            elt[0][2] = fEntry02;
            elt[1][0] = fEntry10;
            elt[1][1] = fEntry11;
            elt[1][2] = fEntry12;
            elt[2][0] = fEntry20;
            elt[2][1] = fEntry21;
            elt[2][2] = fEntry22;
        }

        static Matrix3 fromColumns(const Vector3& c0, const Vector3& c1, const Vector3& c2) {
            Matrix3 m;
            for (int r = 0; r < 3; ++r) {
                m.elt[r][0] = c0[r];
                m.elt[r][1] = c1[r];
                m.elt[r][2] = c2[r];
            }
            return m;
        }

        static Matrix3 fromRows(const Vector3& r0, const Vector3& r1, const Vector3& r2) {
            Matrix3 m;
            for (int c = 0; c < 3; ++c) {
                m.elt[0][c] = r0[c];
                m.elt[1][c] = r1[c];
                m.elt[2][c] = r2[c];
            }
            return m;
        }

        static const Matrix3& zero() {
            static Matrix3 m(0, 0, 0, 0, 0, 0, 0, 0, 0);
            return m;
        }

        static const Matrix3& identity() {
            static Matrix3 m(1, 0, 0, 0, 1, 0, 0, 0, 1);
            return m;
        }

        inline float* operator[] (int iRow) {
            return (float*)&elt[iRow][0];
        }

        inline const float* operator[] (int iRow) const {
            return (const float*)&elt[iRow][0];
        }

        inline operator float* () {
            return (float*)&elt[0][0];
        }

        inline operator const float* () const {
            return (const float*)&elt[0][0];
        }

        Vector3 column(int iCol) const {
            return Vector3(elt[0][iCol], elt[1][iCol],
                elt[2][iCol]);
        }

        const Vector3& row(int iRow) const {
            return *reinterpret_cast<const Vector3*>(elt[iRow]);
        }

        void setColumn(int iCol, const Vector3& vector) {
            elt[0][iCol] = vector.x;
            elt[1][iCol] = vector.y;
            elt[2][iCol] = vector.z;
        }


        void setRow(int iRow, const Vector3& vector) {
            elt[iRow][0] = vector.x;
            elt[iRow][1] = vector.y;
            elt[iRow][2] = vector.z;
        }

        Matrix3 transpose() const {
            Matrix3 kTranspose;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kTranspose[iRow][iCol] = elt[iCol][iRow];
                }
            }

            return kTranspose;
        }

        bool operator== (const Matrix3& rkMatrix) const {
            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    if (elt[iRow][iCol] != rkMatrix.elt[iRow][iCol])
                        return false;
                }
            }

            return true;
        }

        bool operator!= (const Matrix3& rkMatrix) const {
            return !operator==(rkMatrix);
        }

        Matrix3 operator+ (const Matrix3& rkMatrix) const {
            Matrix3 kSum;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kSum.elt[iRow][iCol] = elt[iRow][iCol] +
                        rkMatrix.elt[iRow][iCol];
                }
            }

            return kSum;
        }

        Matrix3 operator- (const Matrix3& rkMatrix) const {
            Matrix3 kDiff;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kDiff.elt[iRow][iCol] = elt[iRow][iCol] -
                        rkMatrix.elt[iRow][iCol];
                }
            }

            return kDiff;
        }

        Matrix3 operator* (const Matrix3& rkMatrix) const {
            Matrix3 kProd;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kProd.elt[iRow][iCol] =
                        elt[iRow][0] * rkMatrix.elt[0][iCol] +
                        elt[iRow][1] * rkMatrix.elt[1][iCol] +
                        elt[iRow][2] * rkMatrix.elt[2][iCol];
                }
            }

            return kProd;
        }

        inline Vector3 operator* (const Vector3& v) const {
            Vector3 kProd;

            for (int r = 0; r < 3; ++r) {
                kProd[r] =
                    elt[r][0] * v[0] +
                    elt[r][1] * v[1] +
                    elt[r][2] * v[2];
            }

            return kProd;
        }

        Matrix3 operator- () const {
            Matrix3 kNeg;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kNeg[iRow][iCol] = -elt[iRow][iCol];
                }
            }

            return kNeg;
        }

        Matrix3 operator* (float fScalar) const {
            Matrix3 kProd;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol) {
                    kProd[iRow][iCol] = fScalar * elt[iRow][iCol];
                }
            }

            return kProd;
        }

        bool inverse(Matrix3& rkInverse, float fTolerance) const {
            // Invert a 3x3 using cofactors.  This is about 8 times faster than
            // the Numerical Recipes code which uses Gaussian elimination.

            rkInverse[0][0] = elt[1][1] * elt[2][2] -
                elt[1][2] * elt[2][1];
            rkInverse[0][1] = elt[0][2] * elt[2][1] -
                elt[0][1] * elt[2][2];
            rkInverse[0][2] = elt[0][1] * elt[1][2] -
                elt[0][2] * elt[1][1];
            rkInverse[1][0] = elt[1][2] * elt[2][0] -
                elt[1][0] * elt[2][2];
            rkInverse[1][1] = elt[0][0] * elt[2][2] -
                elt[0][2] * elt[2][0];
            rkInverse[1][2] = elt[0][2] * elt[1][0] -
                elt[0][0] * elt[1][2];
            rkInverse[2][0] = elt[1][0] * elt[2][1] -
                elt[1][1] * elt[2][0];
            rkInverse[2][1] = elt[0][1] * elt[2][0] -
                elt[0][0] * elt[2][1];
            rkInverse[2][2] = elt[0][0] * elt[1][1] -
                elt[0][1] * elt[1][0];

            float fDet =
                elt[0][0] * rkInverse[0][0] +
                elt[0][1] * rkInverse[1][0] +
                elt[0][2] * rkInverse[2][0];

            if (fabs(fDet) <= fTolerance)
                return false;

            float fInvDet = 1.0f / fDet;

            for (int iRow = 0; iRow < 3; ++iRow) {
                for (int iCol = 0; iCol < 3; ++iCol)
                    rkInverse[iRow][iCol] *= fInvDet;
            }

            return true;
        }

        Matrix3 inverse(float fTolerance) {
            Matrix3 kInverse = zero();
            inverse(kInverse, fTolerance);
            return kInverse;
        }
    };
}