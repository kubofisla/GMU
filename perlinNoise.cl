float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927;
	float f = (1.0f - cos(ft)) * .5;

	return  a*(1.0f - f) + b*f;
}

float PseudoRandom_NoiseI(int x, int y)
{
	int n = (int)x + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n*(n*n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((float)nn / 1073741824.0);
}

float Smooth_NoiseI(float x, float y)
{
	float corners = (PseudoRandom_NoiseI(x - 1, y - 1) + PseudoRandom_NoiseI(x + 1, y - 1) + PseudoRandom_NoiseI(x - 1, y + 1) + PseudoRandom_NoiseI(x + 1, y + 1)) / 16;
	float sides = (PseudoRandom_NoiseI(x - 1, y) + PseudoRandom_NoiseI(x + 1, y) + PseudoRandom_NoiseI(x, y - 1) + PseudoRandom_NoiseI(x, y + 1)) / 8;
	float center = PseudoRandom_NoiseI(x, y) / 4;
	return corners + sides + center;
}

float Interpolated_NoiseI(float x, float y, int freq){
	int integer_X = (int)x;
	float fractional_X = x - integer_X;

	int integer_Y = (int)y;
	float fractional_Y = y - integer_Y;

	float v1 = Smooth_NoiseI(integer_X, integer_Y);
	float v2 = Smooth_NoiseI(integer_X + 1, integer_Y);
	float v3 = Smooth_NoiseI(integer_X, integer_Y + 1);
	float v4 = Smooth_NoiseI(integer_X + 1, integer_Y + 1);

	float i1 = Cosine_Interpolate(v1, v2, fractional_X);
	float i2 = Cosine_Interpolate(v3, v4, fractional_X);

	return Cosine_Interpolate(i1, i2, fractional_Y);
}

__kernel void perlinNoise(__global float *h, float persistence, int octaves, int width, int length)
{
	int global_x = (int)get_global_id(0);
	int global_y = (int)get_global_id(1);

	int index = global_x + width*global_y;

	float frequency;
	float amplitude = 1.0f;

	for (int i = 0; i < octaves; i++)
	{
		frequency = pow(2.0f, i);
		amplitude *= persistence;

		h[index] += Interpolated_NoiseI(global_y / frequency, global_x / frequency, frequency) * amplitude;

	}
}