#include "SoundKinematicRectangleArray.hpp"
#include <fftw3.h>
#include <limits.h>
#include <mutex>
#include <iostream>
#include <cmath>

#define MAX_SAMPLE_COUNT 4096
#define MIC_SCALING_FACTOR 1.f
#define SAMPLE_CAPTURE_TIME_SECONDS .1f

DrawableKinematicRectangleArrayRecorder::DrawableKinematicRectangleArrayRecorder(
    int numRectangles,
    sf::Vector2f& arrayDimensions,
    sf::Vector2f& arrayOrigin,
    DrawableWorld *world
): DrawableKinematicRectangleArray(numRectangles, arrayDimensions, arrayOrigin, world),
   m_drawableWorld(world),
   m_arrayDimensions(arrayDimensions)
{
    sf::Time t = sf::seconds(SAMPLE_CAPTURE_TIME_SECONDS);
    setProcessingInterval(t);
}

bool DrawableKinematicRectangleArrayRecorder::onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) {
    std::scoped_lock<std::mutex> lock(m_drawableWorld->m_drawMutex);
    std::vector<float> kinematicArrayOutput(sampleCount);
    if (sampleCount == 0) return true;

    double* input = (double*)fftw_malloc(sampleCount*sizeof(double));

    for (int i = 0; i < sampleCount; i++) {
        input[i] = MIC_SCALING_FACTOR*(static_cast<double>(samples[i])/SHRT_MAX)*m_arrayDimensions.y;
    }

    // Setup FFT
    fftw_complex* output= (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*sampleCount);
    fftw_plan p = fftw_plan_dft_r2c_1d(sampleCount, input, output, FFTW_ESTIMATE);
    fftw_execute(p);

    auto frequencyBinSize = getSampleRate()/sampleCount;

    for (int i = 0; i < sampleCount; i++) {
        double r = output[i][0];
        double c = output[i][1];
        double magnitude = sqrt(c*c + r*r);

        kinematicArrayOutput[i] = float(magnitude);
    }
    fftw_free(output);
    fftw_free(input);

    SetArrayPositions(kinematicArrayOutput);
    return true;
}
