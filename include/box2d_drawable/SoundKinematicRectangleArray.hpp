#ifndef SOUND_RECTANGLE_H
#define SOUND_RECTANGLE_H
#include "KinematicRectangleArray.hpp"
#include "SFML/Audio/SoundRecorder.hpp"
#include "DrawableWorld.hpp"
#include <fftw3.h>

struct Note {
    float freq;
    const std::string name;
};

class DrawableKinematicRectangleArrayRecorder: public sf::SoundRecorder, public DrawableKinematicRectangleArray {
    public:
        DrawableKinematicRectangleArrayRecorder(int numRectangles, sf::Vector2f& arrayDimensions, sf::Vector2f& arrayOrigin, DrawableWorld *world);
        void DisplayNotes();
    protected:
        bool onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) override;
    private:
        // Signal processing helpers
        void processInput(const std::int16_t* samples, float* fftinput, int sampleCount);
        void displayNoteFromPeaks();

        // Private members
        sf::Vector2f m_arrayDimensions;
        DrawableWorld* m_drawableWorld;
        std::vector<Note> m_chromaticScale;
        int m_numChannels;
};
#endif