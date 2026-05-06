#pragma once

#include "PluginProcessor.h"
#include "SpectrogramComponent.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);

    //==============================================================================

    void pushSampleToSpectrogram(float sample);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    SpectrogramComponent spectrogram;
    std::vector<std::vector<float>> spectrogramBuffer;
    static constexpr int bufferWidth = 512; // Width in pixels
    static constexpr int bufferHeight = 256; // Frequency bins

    // ===== UI ELEMENTS ===== //
    juce::Label titleLabel;

    juce::Slider gainSlider;
    juce::Slider speedSlider;
    juce::Slider mixSlider;

    juce::Label gainLabel;
    juce::Label speedLabel;
    juce::Label mixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
