#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory.h>
#include <ostream>
#include <algorithm>
#include <iostream>

namespace mathLib {
#define SQ(x) (x) * (x)
#define max(a,b) (a>b ? a:b)
#define min(a,b) (a<b ? a:b)

	template<typename T>
	static T lerp(const T a, const T b, float t)
	{
		return a * (1.0f - t) + (b * t);
	}

	template <typename T>
	const T& clamp(const T& v, const T& lo, const T& hi) {
		return max(lo, min(v, hi));
	}

	class Vec2
	{
	public:
		union
		{
			float v[2];
			struct { float x, y; };
		};

		Vec2(float _x = 0, float _y = 0) : x(_x), y(_y) {
			v[0] = _x;
			v[1] = _y;
		}

		Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
		Vec2 operator-(const Vec2& v) { return Vec2(x - v.x, y - v.y); }
		Vec2 operator*(const Vec2 v) { return Vec2(x * v.x, y * v.y); }
		Vec2 operator/(const Vec2 v) { return Vec2(x / v.x, y / v.y); }
		Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
		Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
		Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }
		Vec2 operator*(const float val) { return Vec2(x * val, y * val); }
		Vec2 operator/(const float val) { return Vec2(x / val, y / val); }
		Vec2& operator*=(const float val) { x *= val; y *= val; return *this; }
		Vec2& operator/=(const float val) { x /= val; y /= val; return *this; }
		Vec2 operator-() const { return Vec2(-v[0], -v[1]); }

		// get the length of vector
		float getLength() const {
			return sqrtf(x * x + y * y);
		}

		float getLengthSquare() {
			return (x * x + y * y);
		}

		// normalize into unit vector
		Vec2 normalize(void) {
			float len = 1.f / sqrtf(x * x + y * y);
			return Vec2(x * len, y * len);
		}

		float normalize_GetLength()
		{
			float length = sqrtf(x * x + y * y);
			float len = 1.0f / length;
			x *= len; y *= len;
			return length;
		}

		float dot(const Vec2& v) const { return x * v.x + y * v.y; }

		// 计算二维向量的叉积
		float cross(const Vec2& v) const { return x * v.y - y * v.x; }

		float Max() const {
			return max(x, y);
		}

		float Min() const {
			return min(x, y);
		}

		// Overload the << operator
		friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
			os << "Vec2(" << vec.x << ", " << vec.y << ")";
			return os;
		}
	};

	class Vec3
	{
	public:
		union
		{
			float v[3];
			struct { float x, y, z; };
		};

		Vec3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {
			v[0] = _x;
			v[1] = _y;
			v[2] = _z;
		}

		Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
		Vec3 operator-(const Vec3& v) { return Vec3(x - v.x, y - v.y, z - v.z); }
		Vec3 operator*(const Vec3 v) { return Vec3(x * v.x, y * v.y, z * v.z); }
		Vec3 operator/(const Vec3 v) { return Vec3(x / v.x, y / v.y, z / v.z); }
		Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
		Vec3& operator*=(const Vec3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		Vec3& operator/=(const Vec3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		Vec3 operator*(const float val) { return Vec3(x * val, y * val, z * val); }
		Vec3 operator/(const float val) { return Vec3(x / val, y / val, z / val); }
		Vec3& operator*=(const float val) { x *= val; y *= val; z *= val; return *this; }
		Vec3& operator/=(const float val) { x /= val; y /= val; z /= val; return *this; }
		Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }

		// get the length of vector
		float getLength() const {
			return sqrtf(SQ(x) + SQ(y) + SQ(z));
		}

		float getLengthSquare() {
			return (SQ(x) + SQ(y) + SQ(z));
		}

		// normalize into unit vector
		Vec3 normalize(void) {
			float len = 1.f / sqrtf(SQ(x) + SQ(y) + SQ(z));
			return Vec3(x * len, y * len, z * len);
		}

		float normalize_GetLength()
		{
			float length = sqrtf(SQ(x) + SQ(y) + SQ(z));
			float len = 1.0f / length;
			x *= len; y *= len; z *= len;
			return length;
		}

		float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

		Vec3 cross(const Vec3& v) { return Vec3(v.y * z - v.z * y, v.z * x - v.x * z, v.x * y - v.y * x); }

		float Max() const {
			return max(x, max(y, z));
		}

		float Min() const {
			return min(x, min(y, z));
		}

		// Overload the << operator
		friend std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
			os << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
			return os;
		}
	};

	static float dot(const Vec3& v1, const Vec3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
	static Vec3 cross(const Vec3& v1, const Vec3& v2) { return Vec3(v2.y * v1.z - v2.z * v1.y, v2.z * v1.x - v2.x * v1.z, v2.x * v1.y - v2.y * v1.x); }
	static Vec3 Max(const Vec3& v1, const Vec3& v2)
	{
		return Vec3(max(v1.x, v2.x),
			max(v1.y, v2.y),
			max(v1.z, v2.z));
	}
	static Vec3 Min(const Vec3& v1, const Vec3& v2) {
		return Vec3(min(v1.x, v2.x),
			min(v1.y, v2.y),
			min(v1.z, v2.z));
	}

	class Vec4 {
	public:
		union
		{
			float v[4];
			struct { float x, y, z, w; };
		};

		// Constructor
		Vec4(float _x = 0, float _y = 0, float _z = 0, float _w = 0)
			: x(_x), y(_y), z(_z), w(_w) {
			v[0] = x;
			v[1] = y;
			v[2] = z;
			v[3] = w;
		}

		void divideByW() {
			if (w == 0) {
				std::cout << "Cannot divide by zero" << std::endl;
			}
			x /= w;
			y /= w;
			z /= w;
			w = 1;  // After division, w is typically set to 1
		}

		Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vec4 operator*(float scalar) const { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
		Vec4 operator/(float scalar) const { return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); }

		// Dot product
		float dot(const Vec4& v) const {
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		// Length (magnitude)
		float length() const {
			return std::sqrt(SQ(x) + SQ(y) + SQ(z) + SQ(w));
		}

		// Normalize
		Vec4 normalize() const {
			float len = length();
			return Vec4(x / len, y / len, z / len, w / len);
		}

		// Print vector
		friend std::ostream& operator<<(std::ostream& os, const Vec4& v) {
			os << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
			return os;
		}
	};

	static Vec4 Homo(Vec3& v) {
		return Vec4(v.x, v.y, v.z, 1.0f);
	}

	class Matrix {
	public:
		union {
			float a[4][4];
			float m[16];
		};

		Matrix() { identity(); }
		Matrix(float* otherMatrix) {
			memcpy(&m, otherMatrix, 16 * sizeof(float));
		}
		Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
			a[0][0] = m00;
			a[0][1] = m01;
			a[0][2] = m02;
			a[0][3] = m03;
			a[1][0] = m10;
			a[1][1] = m11;
			a[1][2] = m12;
			a[1][3] = m13;
			a[2][0] = m20;
			a[2][1] = m21;
			a[2][2] = m22;
			a[2][3] = m23;
			a[3][0] = m30;
			a[3][1] = m31;
			a[3][2] = m32;
			a[3][3] = m33;
		}

		void identity() {
			memset(m, 0, 16 * sizeof(float));
			m[0] = 1;
			m[5] = 1;
			m[10] = 1;
			m[15] = 1;
		}

		//// 使用齐次坐标
		//Vec3 mulPointP(Vec3& v) {
		//	Vec3 v1 = Vec3(
		//		(v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
		//		(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
		//		(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
		//	float w;
		//	w = (m[12] * v.x) + (m[13] * v.y) + (m[14] * v.z) + m[15];
		//	w = 1.0f / w;
		//	return (v1 * w);
		//}

		// 不使用齐次坐标,带平移的变换
		Vec3 mulPoint(Vec3& v)
		{
			return Vec3(
				(v.x * m[0] + v.y * m[1] + v.z * m[2]) + m[3],
				(v.x * m[4] + v.y * m[5] + v.z * m[6]) + m[7],
				(v.x * m[8] + v.y * m[9] + v.z * m[10]) + m[11]);
		}

		// 齐次坐标
		Vec4 mulPointP(Vec4& v) {
			return Vec4(
				v.x * m[0] + v.y * m[1] + v.z * m[2] + v.w * m[3],
				v.x * m[4] + v.y * m[5] + v.z * m[6] + v.w * m[7],
				v.x * m[8] + v.y * m[9] + v.z * m[10] + v.w * m[11],
				v.x * m[12] + v.y * m[13] + v.z * m[14] + v.w * m[15]
			);
		}

		// 没有平移的线性变换
		Vec3 mulVec(Vec3& v)
		{
			return Vec3(
				(v.x * m[0] + v.y * m[1] + v.z * m[2]),
				(v.x * m[4] + v.y * m[5] + v.z * m[6]),
				(v.x * m[8] + v.y * m[9] + v.z * m[10]));
		}

		// 平移矩阵
		static Matrix translation(const Vec3& t) {
			Matrix mat;
			mat.a[0][3] = t.x;
			mat.a[1][3] = t.y;
			mat.a[2][3] = t.z;
			return mat;
		}

		// 缩放矩阵
		static Matrix scaling(const Vec3& s) {
			Matrix mat;
			mat.m[0] = s.x;
			mat.m[5] = s.y;
			mat.m[10] = s.z;
			return mat;
		}

		// 绕X轴旋转theta角度
		static Matrix rotateX(float theta) {
			Matrix mat;
			float ct = cosf(theta);
			float st = sinf(theta);
			mat.m[5] = ct;
			mat.m[6] = -st;
			mat.m[9] = st;
			mat.m[10] = ct;
			return mat;
		}

		// 绕Y轴旋转theta角度
		static Matrix rotateY(float theta) {
			Matrix mat;
			float ct = cosf(theta);
			float st = sinf(theta);
			mat.m[0] = ct;
			mat.m[2] = st;
			mat.m[8] = -st;
			mat.m[10] = ct;
			return mat;
		}

		// 绕Z轴旋转theta角度
		static Matrix rotateZ(float theta) {
			Matrix mat;
			float ct = cosf(theta);
			float st = sinf(theta);
			mat.m[0] = ct;
			mat.m[1] = -st;
			mat.m[4] = st;
			mat.m[5] = ct;
			return mat;
		}

		// 混合矩阵
		Matrix mul(const Matrix& matrix) const
		{
			Matrix ret;
			ret.m[0] = m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3];
			ret.m[1] = m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3];
			ret.m[2] = m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3];
			ret.m[3] = m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3];
			ret.m[4] = m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7];
			ret.m[5] = m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7];
			ret.m[6] = m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7];
			ret.m[7] = m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7];
			ret.m[8] = m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11];
			ret.m[9] = m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11];
			ret.m[10] = m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11];
			ret.m[11] = m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11];
			ret.m[12] = m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15];
			ret.m[13] = m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15];
			ret.m[14] = m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15];
			ret.m[15] = m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15];
			return ret;
		}

		// 矩阵的乘积
		Matrix operator*(const Matrix& matrix)
		{
			return mul(matrix);
		}

		// 矩阵转置
		Matrix transpose() const {
			Matrix ret;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ret.m[i * 4 + j] = m[j * 4 + i];
				}
			}
			return ret;
		}

		// 计算矩阵的余子式
		float cofactor(int row, int col) const {
			float submatrix[9];
			int index = 0;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (i != row && j != col) {
						submatrix[index++] = m[i * 4 + j];
					}
				}
			}
			return submatrix[0] * (submatrix[4] * submatrix[8] - submatrix[5] * submatrix[7])
				- submatrix[1] * (submatrix[3] * submatrix[8] - submatrix[5] * submatrix[6])
				+ submatrix[2] * (submatrix[3] * submatrix[7] - submatrix[4] * submatrix[6]);
		}

		// 计算矩阵的行列式
		float determinant() const {
			float det = 0.0f;
			for (int col = 0; col < 4; ++col) {
				det += (col % 2 == 0 ? 1 : -1) * m[col] * cofactor(0, col);
			}
			return det;
		}

		// 计算矩阵的逆矩阵
		Matrix invert() const
		{
			Matrix inv;
			inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
			inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
			inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
			inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
			inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
			inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
			inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
			inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
			inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
			inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
			inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
			inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
			inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
			inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
			inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
			inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
			float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
			if (det == 0) {
				// Handle this case
			}
			det = 1.0 / det;
			for (int i = 0; i < 16; i++) {
				inv[i] = inv[i] * det;
			}
			return inv;
		}

		float& operator[](int index) {
			return m[index];
		}

		// Print matrix
		friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
			os << mat.a[0][0] << " " << mat.a[0][1] << " " << mat.a[0][2] << " " << mat.a[0][3] << std::endl;
			os << mat.a[1][0] << " " << mat.a[1][1] << " " << mat.a[1][2] << " " << mat.a[1][3] << std::endl;
			os << mat.a[2][0] << " " << mat.a[2][1] << " " << mat.a[2][2] << " " << mat.a[2][3] << std::endl;
			os << mat.a[3][0] << " " << mat.a[3][1] << " " << mat.a[3][2] << " " << mat.a[3][3] << std::endl;
			return os;
		}
	};

	class ShadingFrame {
	public:
		Vec3 tangent;
		Vec3 binormal;
		Vec3 normal;

		ShadingFrame(Vec3& v) {
			gramSchmidt(v);
		}

		// 用新创建的正交基坐标系来表示vec
		Vec3 forwardTransform(const Vec3& vec) const {
			return Vec3(
				normal.dot(vec),
				tangent.dot(vec),
				binormal.dot(vec)
			);
		}

		// 将vec从正交基坐标系转换回原始坐标系
		Vec3 inverseTransform(const Vec3& vec) const {
			return Vec3(
				normal.x * vec.x + tangent.x * vec.y + binormal.x * vec.z,
				normal.y * vec.x + tangent.y * vec.y + binormal.y * vec.z,
				normal.z * vec.x + tangent.z * vec.y + binormal.z * vec.z
			);
		}

		// Gram-Schmidt Orthonormalization
		void gramSchmidt(Vec3& v) {
			normal = v.normalize();

			// 找到一个向量temp不与normal平行
			Vec3 temp;
			if (fabs(normal.x) > 0.99f) {
				temp = Vec3(0.0f, 1.0f, 0.0f);
			}
			else {
				temp = Vec3(1.0f, 0.0f, 0.0f);
			}
			// 获得temp和normal叉乘后的向量tangent并将它归一化,该向量与normal正交
			tangent = normal.cross(temp).normalize();

			// 找到与normal和tangent正交的向量binormal
			binormal = normal.cross(tangent);
		}
	};

	class SphericalCoordinates {
	public:
		float r, theta, phi;

		SphericalCoordinates(float _r = 1, float _theta = 0, float _phi = 0)
			: r(_r), theta(_theta), phi(_phi) {}

		// 从笛卡尔坐标转换为球坐标
		static SphericalCoordinates fromCartesian(float x, float y, float z) {
			float r = sqrtf(SQ(x) + SQ(y) + SQ(z));
			float theta = std::acosf(z / r);
			float phi = std::atan2f(y, x);
			return SphericalCoordinates(r, theta, phi);
		}

		// 从球坐标转换为笛卡尔坐标
		void toCartesian(float& x, float& y, float& z) const {
			x = r * std::sinf(theta) * std::cosf(phi);
			y = r * std::sinf(theta) * std::sinf(phi);
			z = r * std::cosf(theta);
		}

		// 打印球坐标
		friend std::ostream& operator<<(std::ostream& os, const SphericalCoordinates& sc) {
			os << "SphericalCoordinates(r: " << sc.r << ", theta: " << sc.theta << ", phi: " << sc.phi << ")";
			return os;
		}
	};

	class Quaternion {
	public:
		float w, x, y, z;

		// Constructor
		Quaternion(float _w = 1, float _x = 0, float _y = 0, float _z = 0)
			: w(_w), x(_x), y(_y), z(_z) {}

		// Normalize the quaternion
		void normalize() {
			float norm = std::sqrtf(SQ(w) + SQ(x) + SQ(y) + SQ(z));
			w /= norm;
			x /= norm;
			y /= norm;
			z /= norm;
		}

		// Quaternion multiplication
		Quaternion operator*(const Quaternion& q) const {
			return Quaternion(
				w * q.w - x * q.x - y * q.y - z * q.z,
				w * q.x + x * q.w + y * q.z - z * q.y,
				w * q.y - x * q.z + y * q.w + z * q.x,
				w * q.z + x * q.y - y * q.x + z * q.w
			);
		}

		// Quaternion addition
		Quaternion operator+(const Quaternion& q) const {
			return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
		}

		// Quaternion subtraction
		Quaternion operator-(const Quaternion& q) const {
			return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
		}

		// Scalar multiplication
		Quaternion operator*(float scalar) const {
			return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
		}

		// Dot product
		float dot(const Quaternion& q) const {
			return w * q.w + x * q.x + y * q.y + z * q.z;
		}

		// Slerp (Spherical Linear Interpolation)
		static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
			float dot = q1.dot(q2);
			const float threshold = 0.9995f;

			if (dot > threshold) {
				// If the quaternions are nearly identical, use linear interpolation
				Quaternion result = q1 + (q2 - q1) * t;
				result.normalize();
				return result;
			}

			dot = clamp(dot, -1.0f, 1.0f);
			float theta_0 = std::acosf(dot);
			float theta = theta_0 * t;

			Quaternion q3 = q2 - q1 * dot;
			q3.normalize();

			return q1 * std::cosf(theta) + q3 * std::sinf(theta);
		}

		// Convert to rotation matrix
		Matrix toMatrix() const {
			Matrix mat;
			mat.a[0][0] = 1 - 2 * (SQ(y) + SQ(z));
			mat.a[0][1] = 2 * (x * y - z * w);
			mat.a[0][2] = 2 * (x * z + y * w);
			mat.a[1][0] = 2 * (x * y + z * w);
			mat.a[1][1] = 1 - 2 * (SQ(x) + SQ(z));
			mat.a[1][2] = 2 * (y * z - x * w);
			mat.a[2][0] = 2 * (x * z - y * w);
			mat.a[2][1] = 2 * (y * z + x * w);
			mat.a[2][2] = 1 - 2 * (SQ(x) + SQ(y));
			return mat;
		}

		// Print quaternion
		friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
			os << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
			return os;
		}
	};

	class Color {
	public:
		Vec4 color;

		// Constructor
		Color(float r = 0, float g = 0, float b = 0, float a = 1)
			: color(r, g, b, a) {}

		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
			: color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) {}

		// Addition
		Color operator+(const Color& c) const {
			return Color(
				max(color.x + c.color.x, 0),
				max(color.y + c.color.y, 0),
				max(color.z + c.color.z, 0),
				max(color.w + c.color.w, 0)
			);
		}

		// Multiplication with another color
		Color operator*(const Color& c) const {
			return Color(
				color.x * c.color.x,
				color.y * c.color.y,
				color.z * c.color.z,
				color.w * c.color.w
			);
		}

		// Scalar multiplication
		Color operator*(const float a) const {
			return Color(
				max(color.x * a, 0),
				max(color.y * a, 0),
				max(color.z * a, 0),
				max(color.w * a, 0)
			);
		}

		// Scalar division
		Color operator/(const float a) const {
			return Color(
				max(color.x / a, 0.0f),
				max(color.y / a, 0.0f),
				max(color.z / a, 0.0f),
				max(color.w / a, 0.0f)
			);
		}

		// Print color
		friend std::ostream& operator<<(std::ostream& os, const Color& c) {
			os << "Color(" << c.color.x << ", " << c.color.y << ", " << c.color.z << ", " << c.color.w << ")";
			return os;
		}
	};

	class Barycentric {
	public:
		float alpha, beta, gamma;
		Barycentric(float _alpha = 0, float _beta = 0, float _gamma = 0) :
			alpha(_alpha), beta(_beta), gamma(_gamma) {}

		bool isInTriangle() const {
			if (alpha > 0 && beta > 0 && gamma > 0 && alpha + beta + gamma <= 1) return true;
			return false;
		}
	};

	static float edgeFunction(Vec2& a, Vec2& b, Vec2& p) {
		Vec2 e = b - a;
		Vec2 q = p - a;
		return q.cross(e);
	}

	static float calArea(Vec2& a, Vec2& b, Vec2& p) {
		Vec2 e = b - a;
		Vec2 q = p - a;
		return abs(q.cross(e)) / 2;
	}

	static Barycentric calBarycentric(Vec2& a, Vec2& b, Vec2& c, Vec2& p) {
		float A = calArea(a, b, c);
		float alpha = calArea(a, b, p) / A;
		float beta = calArea(a, c, p) / A;
		float gamma = 1 - beta - alpha;
		return Barycentric(alpha, beta, gamma);
	}

	static Vec4 transformToScreenSpace(Vec4& vertex, Matrix& model, Matrix& view, Matrix& projection) {
		Matrix mv = view * model;
		Matrix mvp = projection * mv;
		Vec4 transformed = mvp.mulPointP(vertex);
		transformed.divideByW();
		return transformed;
	}

	static Vec2 MapToScreenSpace(Vec4& vertex, Matrix& model, Matrix& view, Matrix& projection, int screenWidth, int screenHeight) {
		Vec4 transformed = transformToScreenSpace(vertex, model, view, projection);
		// Convert normalized device coordinates to screen coordinates
		float x = (transformed.x + 1.0f) * 0.5f * screenWidth;
		float y = (1.0f - transformed.y) * 0.5f * screenHeight;  // Flip y for screen space
		return Vec2(x, y);
	}

	static Matrix lookAt(Vec3& from, Vec3& to, Vec3& up) {
		Vec3 dir = (from - to).normalize();
		Vec3 right = up.cross(dir).normalize();
		Vec3 u = dir.cross(right);

		Matrix mat;
		mat.a[0][0] = right.x;
		mat.a[0][1] = right.y;
		mat.a[0][2] = right.z;
		mat.a[0][3] = -from.dot(right);
		mat.a[1][0] = u.x;
		mat.a[1][1] = u.y;
		mat.a[1][2] = u.z;
		mat.a[1][3] = -from.dot(u);
		mat.a[2][0] = dir.x;
		mat.a[2][1] = dir.y;
		mat.a[2][2] = dir.z;
		mat.a[2][3] = -from.dot(dir);
		mat.a[3][0] = 0.0f;
		mat.a[3][1] = 0.0f;
		mat.a[3][2] = 0.0f;
		mat.a[3][3] = 1.0f;
		return mat;
	}

	static Matrix view(Vec3& from, Vec3& to, Vec3& up) {
		return lookAt(from, to, up).invert();
	}

	template<typename t>
	t perspectiveCorrectInterpolateAttribute(t a0, t a1, t a2, float v0_w, float v1_w, float v2_w, float alpha, float beta, float gamma)
	{
		float frag_w = ((alpha * v0_w) + (beta * v1_w) + (gamma * v2_w));
		t attrib[3];
		attrib[0] = a0 * v0_w * alpha;
		attrib[1] = a1 * v1_w * beta;
		attrib[2] = a2 * v2_w * gamma;
		return ((attrib[0] + attrib[1] + attrib[2]) / frag_w);
	}

}