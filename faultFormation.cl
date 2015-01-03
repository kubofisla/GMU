float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927;
	float f = (1.0f - cos(ft)) * .5;

	return  a*(1.0f - f) + b*f;
}

__kernel void faultFormation(__global float *h, __global int *r1, __global float *r2, float iterations, float displacement, int width)
{
    int global_x = (int)get_global_id(0);
    int global_y = (int)get_global_id(1);
	
    int index = global_x + width*global_y;

	for (int i = 0; i < iterations; i++){
		float v = (r1[i]);
		float a = sin(v);
		float b = cos(v);
		float d = sqrt((float)width*width + width*width);
		// rand() / RAND_MAX gives a random number between 0 and 1.
		// therefore c will be a random number between -d/2 and d/2
		float c = (r2[i] * d - d / 2);

		if (a*global_x + b*global_y - c > 2.0f)
			h[index] += displacement;
		else if (a*global_x + b*global_y - c < -2.0f)
			h[index] -= displacement;
		else
			h[index] += Cosine_Interpolate(-displacement, displacement, (a*global_x + b*global_y - c + 2) / 4.0);
	}
}