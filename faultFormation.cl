float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927;
	float f = (1.0f - cos(ft)) * .5;

	return  a*(1.0f - f) + b*f;
}

__kernel void faultFormation(__global float *h, float a, float b, float c, float displacement, int width)
{
    int global_x = (int)get_global_id(0);
    int global_y = (int)get_global_id(1);
	
    int index = global_x + width*global_y;

	if (a*global_x + b*global_y - c > 3.0f)
        h[index] +=  displacement;
	else if (a*global_x + b*global_y - c < -3.0f)
		h[index] -= displacement;
	else
		h[index] += Cosine_Interpolate(-displacement, displacement, (a*global_x + b*global_y - c + 3)/6.0);
}