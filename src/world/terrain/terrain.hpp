#pragma once // terrain.hpp
// MIT
// Allosker ------------------------------
// =========+
// Utility functions to create accurate terrain shapes
// ---------------------------------------

#include "utilities/types.hpp"
#include "PerlinNoiseHO/PerlinNoise.hpp"
#include <cmath>

namespace terrain
{
	using SeedType = siv::PerlinNoise::seed_type;

	static inline const SeedType seed{ 1232356u };
	static inline siv::PerlinNoise perlin{ seed };
	static inline siv::PerlinNoise perlin2{ seed };
	
	double alpha(double t, uint32 o) noexcept;

	double multifractalElevation(double x, double z, double amp, double freq, uint32 octaves) noexcept;

	inline double skeleton(double x, double z, double radius) noexcept
	{
		double d{ x * x + z * z }; 

		if (d > radius)
			return 0.;

		return 1. - (d / radius);
	}

	inline float height_factor{ 100.f };

	struct
	{
		double freq{ 0.001 };
		float amp{ 1.f };
	} inline continentalness_;

	struct
	{
		double freq{ 0.01 };
		float amp{ 1.f };
	} inline valleys;

	struct
	{
		float water{ 0.5f };
		float sand { 0.53f };
		float grass{ 0.724f };
		float stone{ 1.f };

	} inline thresholds;
	
	inline double generate_height(double x, double z) noexcept
	{
		double continentalness = perlin.octave2D_01(x * continentalness_.freq, z * continentalness_.freq, 6) * continentalness_.amp;

		//double valleys = perlin2.octave2D_01()


		return continentalness;
	}

}