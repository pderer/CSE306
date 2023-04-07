// Project - Code Optimization
// System Programming, DGIST, Prof. Yeseong Kim
// 
// YOU WILL TURN IN THIS FILE.
// Read the provided instruction carefully.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "bmplib.h"
#include "proj.h"

// This implementation is simply copied from "main.c".
// Your job is to modify and optimize it for better performance!

inline static void* convolution_r(void* args[]) {
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];
	float r = 0;
	for(int y = 0; y < height; ++y){
		Pixel* output_y = output + y*width;
		for(int x = 0; x < width; ++x){
    		for (int dy = -1; dy < 2; ++dy) {
				int y_accum = y + dy;
				if (y_accum < 0 || y_accum >= height)
					continue;
				Pixel* input_y = input + (y + dy)*width;
				int dy_accum = 3*dy + 4;
   		    	for (int dx = -1; dx < 2; ++dx) {
					int x_accum = x + dx;
           			if (x_accum < 0 || x_accum >= width)
                		continue;
            		r += (input_y + x_accum)->r * filter[dx + dy_accum];
        		}
    		}
			//r update
			if (r < 0) r = 0;
			else if (r > 255) r = 255;
			(output_y + x)->r = (unsigned char)r;
			r = 0;
		}
	}
	pthread_exit(NULL);
}

inline static void* convolution_g(void* args[]) {
	unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];
    float g = 0;
	for(int y = 0; y < height; ++y){
		Pixel* output_y = output + y*width;
		for(int x = 0; x < width; ++x){
    		for (int dy = -1; dy < 2; ++dy) {
				int y_accum = y + dy;
				if (y_accum < 0 || y_accum >= height)
					continue;
				Pixel* input_y = input + (y + dy)*width;
				int dy_accum = 3*dy + 4;
   		    	for (int dx = -1; dx < 2; ++dx) {
					int x_accum = x + dx;
           			if (x_accum < 0 || x_accum >= width)
                		continue;
            		g += (input_y + x_accum)->g * filter[dx + dy_accum];
        		}
    		}
			//g update
			if (g < 0) g = 0;
			else if (g > 255) g = 255;
			(output_y + x)->g = (unsigned char)g;
			g = 0;
		}
	}
	pthread_exit(NULL);
}

inline static void* convolution_b(void* args[]) {
	unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];
    float b = 0;
	for(int y = 0; y < height; ++y){
		Pixel* output_y = output + y*width;
		for(int x = 0; x < width; ++x){
    		for (int dy = -1; dy < 2; ++dy) {
				int y_accum = y + dy;
				if (y_accum < 0 || y_accum >= height)
					continue;
				Pixel* input_y = input + (y + dy)*width;
				int dy_accum = 3*dy + 4;
   		    	for (int dx = -1; dx < 2; ++dx) {
					int x_accum = x + dx;
           			if (x_accum < 0 || x_accum >= width)
                		continue;
            		b += (input_y + x_accum)->b * filter[dx + dy_accum];
        		}
    		}
			//b update
			if (b < 0) b = 0;
			else if (b > 255) b = 255;
			(output_y + x)->b = (unsigned char)b;
			b = 0;
		}
	}
	pthread_exit(NULL);
}


void filter_optimized(void* args[]) {
	pthread_t r_thr, g_thr, b_thr;
	if(pthread_create(&r_thr, NULL, convolution_r, args) != 0){
		perror("thread error: ");
		exit(1);
	}
	if(pthread_create(&g_thr, NULL, convolution_g, args) != 0){
		perror("thread error: ");
		exit(1);
	}
	if(pthread_create(&b_thr, NULL, convolution_b, args) != 0){
		perror("thread error: ");
		exit(1);
	}
	pthread_join(r_thr, NULL);
	pthread_join(g_thr, NULL);
	pthread_join(b_thr, NULL);
}

