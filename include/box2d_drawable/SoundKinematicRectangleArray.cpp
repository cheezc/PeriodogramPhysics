#include "SoundKinematicRectangleArray.hpp"
#include "KinematicRectangle.hpp"
#include "KinematicRectangleArray.hpp"
#include "SFML/Audio/SoundRecorder.hpp"
#include "SFML/System/Vector2.hpp"
#include <limits.h>
#include <mutex>
#include <iostream>
#include <cmath>
#include "fftw3.h"
#include "utils.hpp"
#include "fft_utils.hpp"

#define MAX_SAMPLE_COUNT 4096
#define MIC_SCALING_FACTOR 20.f
#define SAMPLE_CAPTURE_TIME_SECONDS .1f
#define MIDDLE_C_FREQ_HZ 261.625565
#define START_FREQ MIDDLE_C_FREQ_HZ/4
#define NUM_CHROMATIC_KEYS 12

// Note Label
#define NOTE_LABEL_RECT_HEIGHT 40
#define NOTE_LABEL_RECT_Y_POS 900
#define NOTE_CHARACTER_SIZE 10
#define NOTE_ARRAY_SCALING_FACTOR .8f
#define WELCH_OVERLAP_FACTOR .5f
#define WELCH_SEGMENT_SIZE 256
#define NUM_NOTES 88
#define PEAK_THRESHOLD 900

const std::string NoteNames[NUM_CHROMATIC_KEYS] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

static void GenerateChromaticScale(std::vector<Note>& chromaticScale, int numKeys) {
    float freq = START_FREQ;
    int noteNameInd = 0;
    Note n = {.freq = freq, .name=NoteNames[noteNameInd]};
    chromaticScale.push_back(n);
    for (int i = 1; i < numKeys; i++) {
        freq *= pow(2, (1.f/NUM_CHROMATIC_KEYS));
        SHOW(freq);
        noteNameInd = (noteNameInd + 1)%NUM_CHROMATIC_KEYS;
        chromaticScale.push_back(Note{.freq=freq, .name=NoteNames[noteNameInd]});
    }
}

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
    GenerateChromaticScale(m_chromaticScale, NUM_NOTES);
    m_numChannels = sf::SoundRecorder::getChannelCount();
}

struct NoteBin {
    int startIndex;
    int endIndex;
    std::string note;
};

void DrawableKinematicRectangleArrayRecorder::DisplayNotes() {
    // Get frequency distribution
    float freqSpan = SoundRecorder::getSampleRate()/2;
    SHOW(freqSpan);
    float freqPixelWidth = m_arrayDimensions.x;
    float freqHzPerPixel = freqSpan/freqPixelWidth;

    // Map rectangles to bin sizes
    float currNoteStartFreq = 0;
    float currSampleFreq = 0;
    float currNoteEndFreq = 0;

    for (Note note: m_chromaticScale) {
        currNoteEndFreq = note.freq;
        if (currNoteEndFreq < freqSpan) {
            float pixelWidth = (currNoteEndFreq - currNoteStartFreq)/freqHzPerPixel;
            float startPosX = currNoteStartFreq/freqHzPerPixel;
            float startPosY = NOTE_LABEL_RECT_Y_POS;
            sf::Vector2f size(pixelWidth, NOTE_LABEL_RECT_HEIGHT);
            sf::Vector2f pos(startPosX + size.x/2, startPosY + size.y/2);
            auto box = m_drawableWorld->createStaticBox(pos, size);
            bool displayNote = size.x >= NOTE_CHARACTER_SIZE;
            if (box) {
                box->setFillColor(sf::Color::White);
                if (displayNote) {
                    box->SetText(note.name, NOTE_CHARACTER_SIZE, sf::Color::Black);
                }
            }
            currNoteStartFreq = note.freq;
        }
    }
}

void DrawableKinematicRectangleArrayRecorder::processInput(const int16_t* samples, float* fftinput, int sampleCount) {
    float mean = 0;
    for (int i = 0; i < sampleCount; i++) {
        mean += static_cast<float>(samples[i]);
    }
    mean /= sampleCount;
    for (int i = 0; i < sampleCount; i++) {
        fftinput[i] = static_cast<float>(samples[i]-mean)/SHRT_MAX;
    }
}

bool DrawableKinematicRectangleArrayRecorder::onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) {
    std::scoped_lock<std::mutex> lock(m_drawableWorld->m_drawMutex);
    if (sampleCount == 0) return true;

    float* input = (float*)fftwf_malloc(sampleCount*sizeof(float));

    // Convert int16 to float
    processInput(samples, input, sampleCount);

    int welchSegmentSize = (GetRectangles().size() - 1)*2;

    // fftProcess(input, output, sampleCount);
    float* output = welch(input, sampleCount, welchSegmentSize, WELCH_OVERLAP_FACTOR);

    std::vector<float> kinematicArrayOutput(welchSegmentSize/2 + 1);

    // Simple spectral density calculation without any windowing
    for (int i = 0; i < kinematicArrayOutput.size(); i++) {
        kinematicArrayOutput[i] = MIC_SCALING_FACTOR*output[i]*m_arrayDimensions.y;
    }

    fftwf_free(input);
    fftwf_free(output);

    SetArrayPositions(kinematicArrayOutput);

    return true;
}
