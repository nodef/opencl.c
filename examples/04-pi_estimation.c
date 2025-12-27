// Example 4: Estimating Pi using Monte Carlo Simulation in OpenCL
#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_POINTS 1000000

const char *kernelSource =
"__kernel void monte_carlo_pi(__global const float *x, __global const float *y, __global int *count, int numPoints) {\n" \
"    int id = get_global_id(0);\n" \
"    if (id < numPoints) {\n" \
"        float x_val = x[id];\n" \
"        float y_val = y[id];\n" \
"        if (x_val * x_val + y_val * y_val <= 1.0f) {\n" \
"            atomic_inc(count);\n" \
"        }\n" \
"    }\n" \
"}\n";

int main() {
    srand(time(NULL));

    float *x = (float *)malloc(sizeof(float) * NUM_POINTS);
    float *y = (float *)malloc(sizeof(float) * NUM_POINTS);
    int count = 0;

    for (int i = 0; i < NUM_POINTS; i++) {
        x[i] = (float)rand() / RAND_MAX;
        y[i] = (float)rand() / RAND_MAX;
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_mem bufferX = clCreateBuffer(context, CL_MEM_READ_ONLY, NUM_POINTS * sizeof(float), NULL, NULL);
    cl_mem bufferY = clCreateBuffer(context, CL_MEM_READ_ONLY, NUM_POINTS * sizeof(float), NULL, NULL);
    cl_mem bufferCount = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, NULL);

    clEnqueueWriteBuffer(queue, bufferX, CL_TRUE, 0, NUM_POINTS * sizeof(float), x, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferY, CL_TRUE, 0, NUM_POINTS * sizeof(float), y, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferCount, CL_TRUE, 0, sizeof(int), &count, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "monte_carlo_pi", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferX);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferY);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferCount);
    clSetKernelArg(kernel, 3, sizeof(int), &NUM_POINTS);

    size_t globalSize = NUM_POINTS;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufferCount, CL_TRUE, 0, sizeof(int), &count, 0, NULL, NULL);

    float pi = 4.0f * count / NUM_POINTS;
    printf("Estimated Pi: %f\n", pi);

    clReleaseMemObject(bufferX);
    clReleaseMemObject(bufferY);
    clReleaseMemObject(bufferCount);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(x);
    free(y);

    return 0;
}
