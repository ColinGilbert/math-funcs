#pragma once

#include <array>

namespace noob
{
	/* stored like this:
	   0 3 6
	   1 4 7
	   2 5 8  */
	template <typename T>
		struct mat3_type
		{
			mat3_type() noexcept(true) {}

			/* note: entered in COLUMNS */
			mat3_type(T a, T b, T c, T d, T e, T f, T g, T h, T i) noexcept(true)
			{
				m[0] = a;
				m[1] = b;
				m[2] = c;
				m[3] = d;
				m[4] = e;
				m[5] = f;
				m[6] = g;
				m[7] = h;
				m[8] = i;
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return m[x];
			}
/*
			std::string to_string() const noexcept(true)
			{	
				fmt::MemoryWriter w;
				w << m[0] << ", " << m[3] << ", " << m[6] << "\n" << m[1] << ", " << m[4] << ", " << m[7] << "\n" << m[2] << ", " << m[5] << ", " << m[8] << "\n";
				return w.str();
			}
*/

			std::array<T,9> m;
		};
}
