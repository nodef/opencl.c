// Example 5: N-Body Simulation using OpenCL
#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BODIES 1024
#define TIME_STEP 0.01f
#define GRAVITY 6.67430e-11f

const char *kernelSource =
"__kernel void nbody_simulation(__global float4 *positions, __global float4 *velocities, __global float4 *newPositions, __global float4 *newVelocities, int numBodies, float timeStep) {\n" \
"    int id = get_global_id(0);\n" \
"    if (id < numBodies) {\n" \
"        float4 pos = positions[id];\n" \
"        float4 vel = velocities[id];\n" \
"        float4 force = (float4)(0.0f, 0.0f, 0.0f, 0.0f);\n" \
"        for (int i = 0; i < numBodies; i++) {\n" \
"            if (i != id) {\n" \
"                float4 otherPos = positions[i];\n" \
"                float3 diff = otherPos.xyz - pos.xyz;\n" \
"                float dist = length(diff);\n" \
"                float f = GRAVITY * otherPos.w * pos.w / (dist * dist + 1e-10f);\n" \
"                force.xyz += f * normalize(diff);\n" \
"            }\n" \
"        }\n" \
"        vel.xyz += force.xyz * timeStep / pos.w;\n" \
"        pos.xyz += vel.xyz * timeStep;\n" \
"        newPositions[id] = pos;\n" \
"        newVelocities[id] = vel;\n" \
"    }\n" \
"}\n";

int main() {
    float4 *positions = (float4 *)malloc(sizeof(float4) * NUM_BODIES);
    float4 *velocities = (float4 *)malloc(sizeof(float4) * NUM_BODIES);
    float4 *newPositions = (float4 *)malloc(sizeof(float4) * NUM_BODIES);
    float4 *newVelocities = (float4 *)malloc(sizeof(float4) * NUM_BODIES);

    for (int i = 0; i < NUM_BODIES; i++) {
        positions[i] = (float4){rand() % 1000, rand() % 1000, rand() % 1000, rand() % 100 + 1};
        velocities[i] = (float4){0.0f, 0.0f, 0.0f, 0.0f};
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_mem bufferPositions = clCreateBuffer(context, CL_MEM_READ_WRITE, NUM_BODIES * sizeof(float4), NULL, NULL);
    cl_mem bufferVelocities = clCreateBuffer(context, CL_MEM_READ_WRITE, NUM_BODIES * sizeof(float4), NULL, NULL);
    cl_mem bufferNewPositions = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM_BODIES * sizeof(float4), NULL, NULL);
    cl_mem bufferNewVelocities = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM_BODIES * sizeof(float4), NULL, NULL);

    clEnqueueWriteBuffer(queue, bufferPositions, CL_TRUE, 0, NUM_BODIES * sizeof(float4), positions, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferVelocities, CL_TRUE, 0, NUM_BODIES * sizeof(float4), velocities, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "nbody_simulation", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferPositions);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferVelocities);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferNewPositions);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufferNewVelocities);
    clSetKernelArg(kernel, 4, sizeof(int), &NUM_BODIES);
    clSetKernelArg(kernel, 5, sizeof(float), &TIME_STEP);

    size_t globalSize = NUM_BODIES;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufferNewPositions, CL_TRUE, 0, NUM_BODIES * sizeof(float4), newPositions, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufferNewVelocities, CL_TRUE, 0, NUM_BODIES * sizeof(float4), newVelocities, 0, NULL, NULL);

    printf("N-Body simulation completed.\n");

    clReleaseMemObject(bufferPositions);
    clReleaseMemObject(bufferVelocities);
    clReleaseMemObject(bufferNewPositions);
    clReleaseMemObject(bufferNewVelocities);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(positions);
    free(velocities);
    free(newPositions);
    free(newVelocities);

    return 0;
}
