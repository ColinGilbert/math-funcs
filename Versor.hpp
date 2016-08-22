#pragma once

#include <array>

#include "Vec4.hpp"

namespace noob
{
	struct versor
	{
		versor() noexcept(true) {}

		versor(float x, float y, float z, float w) noexcept(true)
		{
			q[0] = x;
			q[1] = y;
			q[2] = z;
			q[3] = w;
		}

		versor(const noob::vec4& v) noexcept(true)
		{
			q[0] = v[0];
			q[1] = v[1];
			q[2] = v[2];
			q[3] = v[3];
		}

		versor operator/(float rhs) const noexcept(true)
		{
			versor result;
			result.q[0] = q[0] / rhs;
			result.q[1] = q[1] / rhs;
			result.q[2] = q[2] / rhs;
			result.q[3] = q[3] / rhs;
			return result;
		}

		versor operator*(float rhs) const noexcept(true)
		{
			versor result;
			result.q[0] = q[0] * rhs;
			result.q[1] = q[1] * rhs;
			result.q[2] = q[2] * rhs;
			result.q[3] = q[3] * rhs;
			return result;
		}

		std::string to_string() const noexcept(true)
		{
			fmt::MemoryWriter w;
			w << "(" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << ")";
			return w.str();
		}

		versor operator*(const versor& rhs) const noexcept(true)
		{
			versor result;
			result.q[0] = rhs.q[0] * q[0] - rhs.q[1] * q[1] -
				rhs.q[2] * q[2] - rhs.q[3] * q[3];
			result.q[1] = rhs.q[0] * q[1] + rhs.q[1] * q[0] -
				rhs.q[2] * q[3] + rhs.q[3] * q[2];
			result.q[2] = rhs.q[0] * q[2] + rhs.q[1] * q[3] +
				rhs.q[2] * q[0] - rhs.q[3] * q[1];
			result.q[3] = rhs.q[0] * q[3] - rhs.q[1] * q[2] +
				rhs.q[2] * q[1] + rhs.q[3] * q[0];
			// re-normalize in case of mangling
			return normalize (result);
		}

		versor operator+(const versor& rhs) const noexcept(true)
		{
			versor result;
			result.q[0] = rhs.q[0] + q[0];
			result.q[1] = rhs.q[1] + q[1];
			result.q[2] = rhs.q[2] + q[2];
			result.q[3] = rhs.q[3] + q[3];
			// re-normalize in case of mangling
			return normalize (result);
		}

		versor& operator=(const versor& rhs) noexcept(true)
		{
			q[0] = rhs.q[0];
			q[1] = rhs.q[1];
			q[2] = rhs.q[2];
			q[3] = rhs.q[3];
			return *this;
		}

		float& operator[](int x) noexcept(true)
		{
			return q[x];
		}

		std::array<float,4> q;
	};
}
