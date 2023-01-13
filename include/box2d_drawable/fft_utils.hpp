#ifndef FFT_UTILS_H
#define FFT_UTILS_H

#include <cstring>
#include <fftw3.h>
#include <cmath>
#include <limits.h>

static inline void fftProcess(float* fftinput, fftwf_complex* fftoutput, int sampleCount) {
    // Setup FFT
    fftwf_plan p = fftwf_plan_dft_r2c_1d(sampleCount, fftinput, fftoutput, FFTW_ESTIMATE);
    fftwf_execute(p);
    fftw_free(p);
}

static inline void hanningWindowInput(float* fftinput, int inputSampleCount) {
    for (int i = 0; i < inputSampleCount; i++) {
        float multiplier = .5f * (1.f - cos(2*M_PI*i/(inputSampleCount - 1)));
        fftinput[i] = fftinput[i]*multiplier;
    }
}

static inline void addComplex(fftwf_complex* target, fftwf_complex* input, int numSamples) {
    for (int i = 0; i < numSamples; i++) {
        target[i][0] += input[i][0];
        target[i][1] += input[i][1];
    }
}

static inline void addReal(float* target, float* input, int numSamples) {
    for (int i = 0; i < numSamples; i++) {
        target[i] += input[i];
    }
}

static inline void divideComplex(fftwf_complex* target, float divisor, int numSamples) {
    for (int i = 0; i < numSamples; i++) {
        target[i][0] /= divisor;
        target[i][1] /= divisor;
    }
}

void divideReal(float* target, float divisor, int numSamples) {
    for (int i = 0; i < numSamples; i++) {
        target[i] /= divisor;
    }
}

static inline void accumulatePsd(float* target, fftwf_complex* input, int numSamples) {
    for (int i = 0; i < numSamples; i++) {
        float r = input[i][0]/numSamples;
        float c = input[i][1]/numSamples;
        target[i] += sqrt(c*c + r*r);
    }
}

static inline float* welch(float *fftinput, int inputSampleCount, int segmentSize, float overlapFactor) {
    int overlap = segmentSize*overlapFactor;

    // Allocate magnitudes
    float* magnitudes = (float*)fftwf_malloc(segmentSize*sizeof(float));
    memset(magnitudes, 0, segmentSize*sizeof(float));
    int numSegments = 0;
    int i = 0;

    // Iterate and get overlapping samples
    while (i < inputSampleCount) {
        // Allocate segment input and output
        float* segmentInput = (float*)fftwf_malloc(segmentSize*sizeof(float));
        fftwf_complex* segmentOutput = (fftwf_complex* )fftwf_malloc(segmentSize*sizeof(fftwf_complex));

        // Get the segment input from the fftinput
        for (int j = 0; j < segmentSize; j++) {
            float inputval = 0;

            // Zero pad if the size is not right
            if (i + j < inputSampleCount) {
                inputval = fftinput[i+j];
            }
            segmentInput[j] = inputval;
        }

        // run it through a hanning window
        hanningWindowInput(segmentInput, segmentSize);

        // run the input through the fft
        fftProcess(segmentInput, segmentOutput, segmentSize);

        // get the psd of the segment, and add to magnitudes
        accumulatePsd(magnitudes, segmentOutput, segmentSize);

        // Get the start of the next segment
        i += segmentSize - overlap;

        // Increment the number of segments
        numSegments++;

        // Free the memory
        fftwf_free(segmentInput);
        fftwf_free(segmentOutput);
    }

    // Average the PSD of the total number of segments
    divideReal(magnitudes, numSegments, segmentSize);
    return magnitudes;
}

#endif