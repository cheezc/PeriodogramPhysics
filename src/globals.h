#ifndef GLOBALS_H
#define GLOBALS_H

#include "box2d_drawable/SoundKinematicRectangleArray.hpp"
#include "Slider.hpp"

static DrawableKinematicRectangleArrayRecorder* gRecorder;
static gui::Slider* gSlider;

void SliderMicrophoneCallback() {
    if (gRecorder && gSlider) {
        gRecorder->SetMicrophoneGain(gSlider->getValue());
    }
}

#endif