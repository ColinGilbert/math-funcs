// This class is used for C++ convenience, and for providing an API for AngelScript
// TODO: Convert to Eigen and test
#pragma once

#include <cmath>
#include <array>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <Eigen/Geometry>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define NOOB_PI 3.1415926535
#define TAU 2.0 * NOOB_PI
#define ONE_DEG_IN_RAD (2.0 * NOOB_PI) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / (2.0 * NOOB_PI) //57.2957795

#define TWO_PI (2.0 * NOOB_PI)

#define NOOB_EPSILON 0.0001


#include "format.h"

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Versor.hpp"
#include "Mat3.hpp"
#include "Mat4.hpp"
#include "MathUtilTypes.hpp"


namespace noob
{
	typedef vec2_type<float> vec2;
	typedef vec3_type<float> vec3;
	typedef vec4_type<float> vec4;

	typedef versor_type<float> versor;
	typedef mat3_type<float> mat3;
	typedef mat4_type<float> mat4;



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UTILITY TYPES:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	enum class binary_op
	{
		ADD, SUBTRACT
	};

	enum class csg_op
	{
		UNION = 0, DIFFERENCE = 1, INTERSECTION = 2
	};

	enum class cardinal_axis
	{
		X, Y, Z
	};

	struct bbox
	{
		noob::vec3 get_dims() const noexcept(true)
		{
			return noob::vec3(std::fabs(min.v[0]) + std::fabs(max.v[0]), std::fabs(min.v[1]) + std::fabs(max.v[1]), std::fabs(max.v[2]) + std::fabs(max.v[2]));
		}

		noob::vec3 min, max;
	};

	struct cubic_region
	{
		vec3 lower_corner, upper_corner;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// INT-PACKING FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	uint64_t pack_32_to_64(uint32_t x, uint32_t y) noexcept(true)
	{
		return static_cast<uint64_t>(x) << 32 | y;
	}

	std::tuple<uint32_t, uint32_t> pack_64_to_32(uint64_t arg) noexcept(true)
	{
		uint32_t x = static_cast<uint32_t>(arg >> 32);
		uint32_t y = static_cast<uint32_t>(x);
		return std::make_tuple(x,y);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ONE-FLOAT FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T> int sign(T val) noexcept(true)
	{
		return (T(0) < val) - (val < T(0));
	}

	bool approximately_zero(float a) noexcept(true)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a) <= NOOB_EPSILON) return false;
		else return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TWO-FLOAT FUNCTIONS(S):
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool compare_floats(float a, float b) noexcept(true)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a - b) <= NOOB_EPSILON * std::fabs(a)) return false;
		else return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VECTOR FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	noob::vec3 negate(const noob::vec3& arg) noexcept(true)
	{
		return arg * -1.0;
	}

	bool vec3_equality(const vec3& first, const vec3& second) noexcept(true)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if (!compare_floats(first.v[i], second.v[i])) return false;
		}
		return true;
	}

	float length_squared(const vec3& v) noexcept(true)
	{
		return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
	}


	float length(const vec3& v) noexcept(true)
	{
		return sqrt (v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
	}

	vec3 normalize(const vec3& v) noexcept(true)
	{
		vec3 vb;
		float l = length (v);
		if (0.0f == l) {
			return vec3 (0.0f, 0.0f, 0.0f);
		}
		vb.v[0] = v.v[0] / l;
		vb.v[1] = v.v[1] / l;
		vb.v[2] = v.v[2] / l;
		return vb;
	}

	float dot(const vec3& a, const vec3& b) noexcept(true)
	{
		return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
	}

	vec3 cross(const vec3& a, const vec3& b) noexcept(true)
	{
		float x = a.v[1] * b.v[2] - a.v[2] * b.v[1];
		float y = a.v[2] * b.v[0] - a.v[0] * b.v[2];
		float z = a.v[0] * b.v[1] - a.v[1] * b.v[0];
		return vec3 (x, y, z);
	}

	float get_squared_dist(const vec3& from, const vec3& to) noexcept(true)
	{
		float x = (to.v[0] - from.v[0]) * (to.v[0] - from.v[0]);
		float y = (to.v[1] - from.v[1]) * (to.v[1] - from.v[1]);
		float z = (to.v[2] - from.v[2]) * (to.v[2] - from.v[2]);
		return x + y + z;
	}

	/* converts an un-normalized direction into a heading in degrees
	   NB i suspect that the z is backwards here but i've used in in
	   several places like this. d'oh! */
	float direction_to_heading(const vec3& d) noexcept(true)
	{
		return atan2 (-d.v[0], -d.v[2]) * ONE_RAD_IN_DEG;
	}

	vec3 heading_to_direction(float degrees) noexcept(true)
	{
		float rad = degrees * ONE_DEG_IN_RAD;
		return vec3 (-sinf (rad), 0.0f, -cosf (rad));
	}

	bool linearly_dependent(const noob::vec3& a, const noob::vec3& b, const noob::vec3& c) noexcept(true)
	{
		// if (a cross b) dot c = 0
		if (dot(cross(a, b), c) == 0.0) return true;
		else return false;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// QUATERNION FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	versor quat_from_axis_rad(float radians, float x, float y, float z) noexcept(true)
	{
		versor result;
		result.q[0] = cos (radians / 2.0);
		result.q[1] = sin (radians / 2.0) * x;
		result.q[2] = sin (radians / 2.0) * y;
		result.q[3] = sin (radians / 2.0) * z;
		return result;
	}

	versor quat_from_axis_deg (float degrees, float x, float y, float z) noexcept(true)
	{
		return quat_from_axis_rad (ONE_DEG_IN_RAD * degrees, x, y, z);
	}


	versor normalize(const versor& q) noexcept(true)
	{
		// norm(q) = q / magnitude (q)
		// magnitude (q) = sqrt (w*w + x*x...)
		// only compute sqrt if interior sum != 1.0
		versor qq(q);
		float sum = qq.q[0] * qq.q[0] + qq.q[1] * qq.q[1] + qq.q[2] * qq.q[2] + qq.q[3] * qq.q[3];
		// NB: floats have min 6 digits of precision
		const float thresh = 0.0001f;
		if (fabs (1.0f - sum) < thresh)
		{
			return q;
		}
		float mag = sqrt(sum);
		return qq / mag;
	}

	float dot(const versor& q, const versor& r) noexcept(true)
	{
		return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
	}

	versor slerp(const versor& q, const versor& r, float t) noexcept(true)
	{
		versor temp_q(q);
		// angle between q0-q1
		float cos_half_theta = dot(temp_q, r);
		// as found here http://stackoverflow.com/questions/2886606/flipping-issue-when-interpolating-rotations-using-quaternions
		// if dot product is negative then one quaternion should be negated, to make
		// it take the short way around, rather than the long way
		// yeah! and furthermore Susan, I had to recalculate the d.p. after this

		if (cos_half_theta < 0.0f)
		{
			for (int i = 0; i < 4; i++)
			{
				temp_q.q[i] *= -1.0f;
			}
			cos_half_theta = dot (temp_q, r);
		}
		// if qa=qb or qa=-qb then theta = 0 and we can return qa
		if (fabs(cos_half_theta) >= 1.0f)
		{
			return temp_q;
		}
		// Calculate temporary values
		float sin_half_theta = sqrt(1.0f - cos_half_theta * cos_half_theta);
		// if theta = 180 degrees then result is not fully defined
		// we could rotate around any axis normal to qa or qb
		versor result;
		if (fabs(sin_half_theta) < 0.001f)
		{
			for (int i = 0; i < 4; i++)
			{
				result.q[i] = (1.0f - t) * temp_q.q[i] + t * r.q[i];
			}
			return result;
		}
		float half_theta = acos(cos_half_theta);
		float a = sin((1.0f - t) * half_theta) / sin_half_theta;
		float b = sin(t * half_theta) / sin_half_theta;
		for (int i = 0; i < 4; i++)
		{
			result.q[i] = temp_q.q[i] * a + r.q[i] * b;
		}
		return result;
	}

	versor quat_from_mat4(const mat4& m) noexcept(true)
	{
		glm::mat4 mm = glm::make_mat4(&m.m[0]);
		glm::quat q = glm::quat_cast(mm);
		noob::versor qq;
		qq.q[0] = q[0];
		qq.q[1] = q[1];
		qq.q[2] = q[2];
		qq.q[3] = q[3];
		return qq;
	}

	vec3 lerp(const noob::vec3& a, const noob::vec3& b, float t) noexcept(true)
	{ 
		return a + (b - a) * t;
	}


	versor versor_from_axis_rad(float radians, float x, float y, float z) noexcept(true)
	{
		versor result;
		result.q[0] = cos (radians / 2.0);
		result.q[1] = sin (radians / 2.0) * x;
		result.q[2] = sin (radians / 2.0) * y;
		result.q[3] = sin (radians / 2.0) * z;
		return result;
	}

	versor versor_from_axis_deg(float degrees, float x, float y, float z) noexcept(true)
	{
		return versor_from_axis_rad (ONE_DEG_IN_RAD * degrees, x, y, z);
	}

	mat4 versor_to_mat4(const noob::versor& q) noexcept(true)
	{
		const float w = q.q[0];
		const float x = q.q[1];
		const float y = q.q[2];
		const float z = q.q[3];
		return mat4 (1.0f - 2.0f * y * y - 2.0f * z * z,
				2.0f * x * y + 2.0f * w * z,
				2.0f * x * z - 2.0f * w * y,
				0.0f,
				2.0f * x * y - 2.0f * w * z,
				1.0f - 2.0f * x * x - 2.0f * z * z,
				2.0f * y * z + 2.0f * w * x,
				0.0f,
				2.0f * x * z + 2.0f * w * y,
				2.0f * y * z - 2.0f * w * x,
				1.0f - 2.0f * x * x - 2.0f * y * y,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f
			    );
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MATRIX FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mat3 zero_mat3() noexcept(true)
	{
		return mat3(	0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f);
	}

	mat3 identity_mat3() noexcept(true)
	{
		return mat3(	1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f);
	}

	mat4 zero_mat4() noexcept(true)
	{
		return mat4(	0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f);
	}

	mat4 identity_mat4() noexcept(true)
	{
		return mat4(	1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Returns a scalar value with the determinant for a 4x4 matrix
	// see http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
	float determinant(const mat4& mm) noexcept(true)
	{
		return
			mm.m[12] * mm.m[9] * mm.m[6] * mm.m[3] -
			mm.m[8] * mm.m[13] * mm.m[6] * mm.m[3] -
			mm.m[12] * mm.m[5] * mm.m[10] * mm.m[3] +
			mm.m[4] * mm.m[13] * mm.m[10] * mm.m[3] +
			mm.m[8] * mm.m[5] * mm.m[14] * mm.m[3] -
			mm.m[4] * mm.m[9] * mm.m[14] * mm.m[3] -
			mm.m[12] * mm.m[9] * mm.m[2] * mm.m[7] +
			mm.m[8] * mm.m[13] * mm.m[2] * mm.m[7] +
			mm.m[12] * mm.m[1] * mm.m[10] * mm.m[7] -
			mm.m[0] * mm.m[13] * mm.m[10] * mm.m[7] -
			mm.m[8] * mm.m[1] * mm.m[14] * mm.m[7] +
			mm.m[0] * mm.m[9] * mm.m[14] * mm.m[7] +
			mm.m[12] * mm.m[5] * mm.m[2] * mm.m[11] -
			mm.m[4] * mm.m[13] * mm.m[2] * mm.m[11] -
			mm.m[12] * mm.m[1] * mm.m[6] * mm.m[11] +
			mm.m[0] * mm.m[13] * mm.m[6] * mm.m[11] +
			mm.m[4] * mm.m[1] * mm.m[14] * mm.m[11] -
			mm.m[0] * mm.m[5] * mm.m[14] * mm.m[11] -
			mm.m[8] * mm.m[5] * mm.m[2] * mm.m[15] +
			mm.m[4] * mm.m[9] * mm.m[2] * mm.m[15] +
			mm.m[8] * mm.m[1] * mm.m[6] * mm.m[15] -
			mm.m[0] * mm.m[9] * mm.m[6] * mm.m[15] -
			mm.m[4] * mm.m[1] * mm.m[10] * mm.m[15] +
			mm.m[0] * mm.m[5] * mm.m[10] * mm.m[15];
	}


	/* returns a 16-element array that is the inverse of a 16-element array (4x4
	   matrix). see http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm */
	mat4 inverse(const mat4& mm) noexcept(true)
	{
		float det = determinant (mm);
		/* there is no inverse if determinant is zero (not likely unless scale is
		   broken) */
		if (0.0f == det)
		{
			// logger::log("WARNING. matrix has no determinant. can not invert");
			return mm;
		}
		float inv_det = 1.0f / det;

		return mat4 (
				inv_det * (
					mm.m[9] * mm.m[14] * mm.m[7] - mm.m[13] * mm.m[10] * mm.m[7] +
					mm.m[13] * mm.m[6] * mm.m[11] - mm.m[5] * mm.m[14] * mm.m[11] -
					mm.m[9] * mm.m[6] * mm.m[15] + mm.m[5] * mm.m[10] * mm.m[15]
					),
				inv_det * (
					mm.m[13] * mm.m[10] * mm.m[3] - mm.m[9] * mm.m[14] * mm.m[3] -
					mm.m[13] * mm.m[2] * mm.m[11] + mm.m[1] * mm.m[14] * mm.m[11] +
					mm.m[9] * mm.m[2] * mm.m[15] - mm.m[1] * mm.m[10] * mm.m[15]
					),
				inv_det * (
					mm.m[5] * mm.m[14] * mm.m[3] - mm.m[13] * mm.m[6] * mm.m[3] +
					mm.m[13] * mm.m[2] * mm.m[7] - mm.m[1] * mm.m[14] * mm.m[7] -
					mm.m[5] * mm.m[2] * mm.m[15] + mm.m[1] * mm.m[6] * mm.m[15]
					),
				inv_det * (
					mm.m[9] * mm.m[6] * mm.m[3] - mm.m[5] * mm.m[10] * mm.m[3] -
					mm.m[9] * mm.m[2] * mm.m[7] + mm.m[1] * mm.m[10] * mm.m[7] +
					mm.m[5] * mm.m[2] * mm.m[11] - mm.m[1] * mm.m[6] * mm.m[11]
					),
				inv_det * (
						mm.m[12] * mm.m[10] * mm.m[7] - mm.m[8] * mm.m[14] * mm.m[7] -
						mm.m[12] * mm.m[6] * mm.m[11] + mm.m[4] * mm.m[14] * mm.m[11] +
						mm.m[8] * mm.m[6] * mm.m[15] - mm.m[4] * mm.m[10] * mm.m[15]
					  ),
				inv_det * (
						mm.m[8] * mm.m[14] * mm.m[3] - mm.m[12] * mm.m[10] * mm.m[3] +
						mm.m[12] * mm.m[2] * mm.m[11] - mm.m[0] * mm.m[14] * mm.m[11] -
						mm.m[8] * mm.m[2] * mm.m[15] + mm.m[0] * mm.m[10] * mm.m[15]
					  ),
				inv_det * (
						mm.m[12] * mm.m[6] * mm.m[3] - mm.m[4] * mm.m[14] * mm.m[3] -
						mm.m[12] * mm.m[2] * mm.m[7] + mm.m[0] * mm.m[14] * mm.m[7] +
						mm.m[4] * mm.m[2] * mm.m[15] - mm.m[0] * mm.m[6] * mm.m[15]
					  ),
				inv_det * (
						mm.m[4] * mm.m[10] * mm.m[3] - mm.m[8] * mm.m[6] * mm.m[3] +
						mm.m[8] * mm.m[2] * mm.m[7] - mm.m[0] * mm.m[10] * mm.m[7] -
						mm.m[4] * mm.m[2] * mm.m[11] + mm.m[0] * mm.m[6] * mm.m[11]
					  ),
				inv_det * (
						mm.m[8] * mm.m[13] * mm.m[7] - mm.m[12] * mm.m[9] * mm.m[7] +
						mm.m[12] * mm.m[5] * mm.m[11] - mm.m[4] * mm.m[13] * mm.m[11] -
						mm.m[8] * mm.m[5] * mm.m[15] + mm.m[4] * mm.m[9] * mm.m[15]
					  ),
				inv_det * (
						mm.m[12] * mm.m[9] * mm.m[3] - mm.m[8] * mm.m[13] * mm.m[3] -
						mm.m[12] * mm.m[1] * mm.m[11] + mm.m[0] * mm.m[13] * mm.m[11] +
						mm.m[8] * mm.m[1] * mm.m[15] - mm.m[0] * mm.m[9] * mm.m[15]
					  ),
				inv_det * (
						mm.m[4] * mm.m[13] * mm.m[3] - mm.m[12] * mm.m[5] * mm.m[3] +
						mm.m[12] * mm.m[1] * mm.m[7] - mm.m[0] * mm.m[13] * mm.m[7] -
						mm.m[4] * mm.m[1] * mm.m[15] + mm.m[0] * mm.m[5] * mm.m[15]
					  ),
				inv_det * (
						mm.m[8] * mm.m[5] * mm.m[3] - mm.m[4] * mm.m[9] * mm.m[3] -
						mm.m[8] * mm.m[1] * mm.m[7] + mm.m[0] * mm.m[9] * mm.m[7] +
						mm.m[4] * mm.m[1] * mm.m[11] - mm.m[0] * mm.m[5] * mm.m[11]
					  ),
				inv_det * (
						mm.m[12] * mm.m[9] * mm.m[6] - mm.m[8] * mm.m[13] * mm.m[6] -
						mm.m[12] * mm.m[5] * mm.m[10] + mm.m[4] * mm.m[13] * mm.m[10] +
						mm.m[8] * mm.m[5] * mm.m[14] - mm.m[4] * mm.m[9] * mm.m[14]
					  ),
				inv_det * (
						mm.m[8] * mm.m[13] * mm.m[2] - mm.m[12] * mm.m[9] * mm.m[2] +
						mm.m[12] * mm.m[1] * mm.m[10] - mm.m[0] * mm.m[13] * mm.m[10] -
						mm.m[8] * mm.m[1] * mm.m[14] + mm.m[0] * mm.m[9] * mm.m[14]
					  ),
				inv_det * (
						mm.m[12] * mm.m[5] * mm.m[2] - mm.m[4] * mm.m[13] * mm.m[2] -
						mm.m[12] * mm.m[1] * mm.m[6] + mm.m[0] * mm.m[13] * mm.m[6] +
						mm.m[4] * mm.m[1] * mm.m[14] - mm.m[0] * mm.m[5] * mm.m[14]
					  ),
				inv_det * (
						mm.m[4] * mm.m[9] * mm.m[2] - mm.m[8] * mm.m[5] * mm.m[2] +
						mm.m[8] * mm.m[1] * mm.m[6] - mm.m[0] * mm.m[9] * mm.m[6] -
						mm.m[4] * mm.m[1] * mm.m[10] + mm.m[0] * mm.m[5] * mm.m[10]
					  )
					);
	}

	// Returns a 16-element array flipped on the main diagonal
	mat4 transpose(const mat4& mm) noexcept(true)
	{
		return mat4 (
				mm.m[0], mm.m[4], mm.m[8], mm.m[12],
				mm.m[1], mm.m[5], mm.m[9], mm.m[13],
				mm.m[2], mm.m[6], mm.m[10], mm.m[14],
				mm.m[3], mm.m[7], mm.m[11], mm.m[15]
			    );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AFFINE MATRIX FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: Find out if rotating a matrix by a quaternion can be done by converting the quat to a matrix and then multiplying the two.

	mat4 translate(const mat4& m, const vec3& v) noexcept(true)
	{
		mat4 m_t = identity_mat4();
		m_t.m[12] = v.v[0];
		m_t.m[13] = v.v[1];
		m_t.m[14] = v.v[2];
		return m_t * m;
	}

	mat4 rotate(const mat4& m, const versor& v) noexcept(true)
	{
		return versor_to_mat4(v) * m;
	}

	mat4 rotate_x_deg(const mat4& m, float deg) noexcept(true)
	{
		// convert to radians
		float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[5] = cos (rad);
		m_r.m[9] = -sin (rad);
		m_r.m[6] = sin (rad);
		m_r.m[10] = cos (rad);
		return m_r * m;
	}

	mat4 rotate_y_deg(const mat4& m, float deg) noexcept(true)
	{
		// convert to radians
		const float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[0] = cos (rad);
		m_r.m[8] = sin (rad);
		m_r.m[2] = -sin (rad);
		m_r.m[10] = cos (rad);
		return m_r * m;
	}

	mat4 rotate_z_deg(const mat4& m, float deg) noexcept(true)
	{
		// convert to radians
		const float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[0] = cos (rad);
		m_r.m[4] = -sin (rad);
		m_r.m[1] = sin (rad);
		m_r.m[5] = cos (rad);
		return m_r * m;
	}

	mat4 scale(const mat4& m, const vec3& v) noexcept(true)
	{
		mat4 a = identity_mat4();
		a.m[0] = v.v[0];
		a.m[5] = v.v[1];
		a.m[10] = v.v[2];
		return a * m;
	}

	vec3 get_normal(const std::array<noob::vec3, 3>& vertices) noexcept(true)
	{
		// Note: We onlt need the first three points on the face to calculate its normal.
		noob::vec3 u = vertices[1] - vertices[0];
		noob::vec3 v = vertices[2] - vertices[0];
		noob::vec3 results = noob::cross(u, v);
		return normalize(results);
	}

	// TODO: Implement. PITA (?)
	// vec4 rotation_from_mat4(const mat4& m);
	vec3 translation_from_mat4(const mat4& m) noexcept(true)
	{
		vec3 v;
		v.v[0] = m.m[12];
		v.v[1] = m.m[13];
		v.v[2] = m.m[14];
		return v;
	}


	vec3 scale_from_mat4(const mat4& m) noexcept(true)
	{
		vec3 v;
		v.v[0] = m.m[0];
		v.v[1] = m.m[5];
		v.v[2] = m.m[10];
		return v;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CAMERA FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Returns a view matrix using the opengl lookAt style. COLUMN ORDER:
	mat4 look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up) noexcept(true)
	{
		// inverse translation
		mat4 p = identity_mat4 ();
		p = translate (p, vec3 (-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2]));
		// distance vector
		vec3 d = targ_pos - cam_pos;
		// forward vector
		vec3 f = normalize (d);
		// right vector
		vec3 r = normalize (cross (f, up));
		// real up vector
		vec3 u = normalize (cross (r, f));
		mat4 ori = identity_mat4 ();
		ori.m[0] = r.v[0];
		ori.m[4] = r.v[1];
		ori.m[8] = r.v[2];
		ori.m[1] = u.v[0];
		ori.m[5] = u.v[1];
		ori.m[9] = u.v[2];
		ori.m[2] = -f.v[0];
		ori.m[6] = -f.v[1];
		ori.m[10] = -f.v[2];

		return ori * p;//p * ori;
	}

	// returns a perspective function mimicking the opengl projection style.
	mat4 perspective(float fovy, float aspect, float near, float far) noexcept(true)
	{
		float fov_rad = fovy * ONE_DEG_IN_RAD;
		float range = tan (fov_rad / 2.0f) * near;
		float sx = (2.0f * near) / (range * aspect + range * aspect);
		float sy = near / range;
		float sz = -(far + near) / (far - near);
		float pz = -(2.0f * far * near) / (far - near);
		mat4 m = zero_mat4 (); // make sure bottom-right corner is zero
		m.m[0] = sx;
		m.m[5] = sy;
		m.m[10] = sz;
		m.m[14] = pz;
		m.m[11] = -1.0f;
		return m;
	}

	/*
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15
	   */
	mat4 ortho(float left, float right, float bottom, float top, float near, float far) noexcept(true)
	{
		mat4 m = zero_mat4();
		m.m[0] = 2.0/(right-left);
		m.m[5] = 2.0/(top - bottom);
		m.m[10] = -2.0/(far - near);
		m.m[12] = (right + left)/(right - left);
		m.m[13] = (top + bottom)/(top - bottom);
		m.m[14] = (far + near)/(far - near);
		m.m[15] = 1.0;
		return m;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CONVERSION UTILITY FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	vec3 vec3_from_vec4(const vec4& vv) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = vv.v[0];
		v.v[1] = vv.v[1];
		v.v[2] = vv.v[2];
		return v;
	}

	vec3 vec3_from_array(const std::array<float, 3>& a) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = a[0];
		v.v[1] = a[1];
		v.v[2] = a[2];
		return v;
	}

	vec3 vec3_from_bullet(const btVector3& btVec) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = btVec[0];
		v.v[1] = btVec[1];
		v.v[2] = btVec[2];
		return v;
	}

	vec3 vec3_from_polymesh(const PolyMesh::Point& p) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	vec3 vec3_from_eigen_vec3(const Eigen::Vector3f& p) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	// Whatever the hell you gotta do to compile, man...
	vec3 vec3_from_eigen_block(const Eigen::Block<const Eigen::Matrix<float, 4, 1>, 3, 1, false> n) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = n[0];
		v.v[1] = n[1];
		v.v[2] = n[2];
		return v;
	}

	versor versor_from_mat4(const mat4& m) noexcept(true)
	{
		glm::mat4 mm = glm::make_mat4(&m.m[0]);
		glm::quat q = glm::quat_cast(mm);
		noob::versor qq;
		qq.q[0] = q[0];
		qq.q[1] = q[1];
		qq.q[2] = q[2];
		qq.q[3] = q[3];
		return qq;
	}

	versor versor_from_bullet(const btQuaternion& arg) noexcept(true)
	{
		noob::versor qq;
		qq.q[0] = arg[0];
		qq.q[1] = arg[1];
		qq.q[2] = arg[2];
		qq.q[3] = arg[3];
		return qq;
	}
}
