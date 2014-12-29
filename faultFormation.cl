__kernel void faultFormation(__global float *h, float a, float b, float c, float displacement, int width)
{
    int global_x = (int)get_global_id(0);
    int global_y = (int)get_global_id(1);
	
    int index = global_x + width*global_y;

    if(a*global_x + b*global_y - c > 0)
        h[index] +=  displacement;
    else
        h[index] -=  displacement;
}