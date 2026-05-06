//
// Created by alisdair chauvin on 2/24/26.
//

#ifndef AUDIOVISUALIZER_02_SPECTROGRAMCOMPONENT_H
#define AUDIOVISUALIZER_02_SPECTROGRAMCOMPONENT_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_dsp/juce_dsp.h>

class SpectrogramComponent : public juce::Component,
                             private juce::Timer
{
public:
    SpectrogramComponent();
    ~SpectrogramComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void pushNextSampleIntoFifo (float sample) noexcept;
    void drawNextLineOfSpectrogram();

    static constexpr auto fftOrder = 11; // size of FFT window. 2^n = the number of points.
    static constexpr auto fftSize = 1 << fftOrder; // create binary number with n

    // Setters for UI sliders
    void setSpeed(double hz);
    void setGain(float newGain);
    void setMix(float newMix);

private:
    void timerCallback() override;

    juce::dsp::FFT forwardFFT;
    juce::Image spectrogramImage;

    // Incoming audio data in samples
    std::array<float, fftSize> fifo;

    // Result of FFT calculations
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0; // temporary count of samples in fifo
    bool nextFFTBlockReady = false; // temporary boolean

    float gain = 1.0f;
    float mix = 1.0f;
};


#endif //AUDIOVISUALIZER_02_SPECTROGRAMCOMPONENT_H