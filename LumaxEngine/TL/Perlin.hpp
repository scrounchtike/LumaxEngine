
#ifndef PERLIN_HPP
#define PERLIN_HPP

# include <cstdint>
# include <numeric>
# include <algorithm>
# include <random>

// Taken from :
// https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
namespace Perlin {
	static std::int32_t p[512];

	static void reseed(std::uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
			{
				p[i] = i;
			}
		
		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));
		
		for (size_t i = 0; i < 256; ++i)
			{
				p[256 + i] = p[i];
			}
	}
	
	static double Fade(double t) noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	
	static double Lerp(double t, double a, double b) noexcept
	{
		return a + t * (b - a);
	}
	
	static double Grad(std::int32_t hash, double x, double y, double z) noexcept
	{
		const std::int32_t h = hash & 15;
		const double u = h < 8 ? x : y;
		const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	
	static float getNoise(float x, float y, float z){
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;
		
		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);
		
		const double u = Fade(x);
		const double v = Fade(y);
		const double w = Fade(z);
		
		const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
				Grad(p[BA], x - 1, y, z)),
				Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
				Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
				Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}
	static float getNoise(float x, float y){
		return getNoise(x, y, 0.0);
	}
}

#endif
