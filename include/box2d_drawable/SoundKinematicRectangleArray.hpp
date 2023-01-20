#ifndef SOUND_RECTANGLE_H
#define SOUND_RECTANGLE_H
#include "DrawableBodyFactory.hpp"
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
        DrawableKinematicRectangleArrayRecorder(
            int numRectangles,
            sf::Vector2f& arrayDimensions,
            sf::Vector2f& arrayOrigin,
            DrawableWorld *world,
            DrawableBodyFactory* factory);
        void DisplayNotes();
        void SetMicrophoneGain(float gain);
    protected:
        bool onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) override;
    private:
        // Signal processing helpers
        void processInput(const std::int16_t* samples, float* fftinput, int sampleCount);
        void displayNoteFromPeaks();

        // Private members
        sf::Vector2f m_arrayDimensions;
        std::vector<Note> m_chromaticScale;
        int m_numChannels;
        float m_microphoneGain;
        DrawableBodyFactory* m_factory;
        std::mutex &m_renderMutex;

};
#endif