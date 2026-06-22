#include "terrain.hpp"

namespace terrain
{

	double alpha(double t, uint32 o) noexcept
	{
		if (o == 0.)
			return 1.;

		double freq = 1.;
		double h = 0.75;

		for (uint32 i{}; i < o; i++)
			freq *= 2.;

		return t * std::pow(freq, -h);
	}

	double multifractalElevation(double x, double z, double amp, double freq, uint32 octaves) noexcept
	{
		double ret = 0.;
		double f = freq;

		for (double i{}; i < octaves; i++)
		{
			ret += alpha(ret, i) * perlin.noise2D(x * f, z * f);
			f *= 2.;
		}

		return amp * ret;
	}



}