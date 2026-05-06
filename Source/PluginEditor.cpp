#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    addAndMakeVisible(spectrogram);
    setSize(800, 400);

    // Title
    titleLabel.setFont(juce::FontOptions("Courier New", 30.f, juce::Font::bold | juce::Font::italic));
    titleLabel.setText("spectroComp", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(titleLabel);

    // Sliders
    auto setupSlider = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::LinearBar);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    };

    for (auto* s : { &gainSlider, &speedSlider, &mixSlider })
    {
        s->setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        s->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    }

    setupSlider(gainSlider);
    setupSlider(speedSlider);
    setupSlider(mixSlider);

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(mixSlider);

    // === Adjust and connect Speed Slider === //
    speedSlider.setRange(10.0, 90.0, 1.0);
    speedSlider.setValue(60.0);

    speedSlider.onValueChange = [this]
    {
        spectrogram.setSpeed(speedSlider.getValue());
    };

    // === Adjust and connect Gain Slider === //
    gainSlider.setRange(0.1, 10.0, 0.01); // Subtle to aggressive boost
    gainSlider.setValue(1.0); // Default
    gainSlider.onValueChange = [this]
    {
        spectrogram.setGain(static_cast<float>(gainSlider.getValue()));
    };

    // === Adjust and connect Mix Slider === //
    mixSlider.setRange(0.0, 1.0, 0.01); // 0 = dry, 1 = full visual
    mixSlider.setValue(1.0); // Default
    mixSlider.onValueChange = [this]
    {
        spectrogram.setGain(static_cast<float>(mixSlider.getValue()));
    };

    // Labels
    gainLabel.setFont(juce::FontOptions("Courier New", 16.f, juce::Font::bold));
    gainLabel.setText("Gain", juce::dontSendNotification);
    speedLabel.setFont(juce::FontOptions("Courier New", 16.f, juce::Font::bold));
    speedLabel.setText("Speed", juce::dontSendNotification);
    mixLabel.setFont(juce::FontOptions("Courier New", 16.f, juce::Font::bold));
    mixLabel.setText("Mix", juce::dontSendNotification);

    for (auto* l : { &gainLabel, &speedLabel, &mixLabel })
    {
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(l);
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0, 0, 76));

    g.setColour(juce::Colours::darkblue.withAlpha(0.1f));
    g.drawRect(getLocalBounds(), 10);
}

void AudioPluginAudioProcessorEditor::pushSampleToSpectrogram(float sample)
{
    // static int counter = 0;
    // if (++counter % 5000 == 0)
    //     DBG("Editor received samples");

    spectrogram.pushNextSampleIntoFifo(sample);
}


void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    auto header = area.removeFromTop(40);
    titleLabel.setBounds(header);

    // Spectrogram takes 75% of remaining area
    auto spectroArea = area.removeFromTop(juce::roundToInt(area.getHeight() * 0.75));
    spectrogram.setBounds(spectroArea);

    // Shrink area by 20 pixels to create padding
    auto controlArea = area.reduced(20);

    // Divide space by 3 for each sliders
    int sliderWidth = controlArea.getWidth() / 3;

    gainSlider.setBounds(controlArea.removeFromLeft(sliderWidth).reduced(10));
    speedSlider.setBounds(controlArea.removeFromLeft(sliderWidth).reduced(10));
    mixSlider.setBounds(controlArea.reduced(10));

    // Labels
    gainLabel.attachToComponent(&gainSlider, false);
    speedLabel.attachToComponent(&speedSlider, false);
    mixLabel.attachToComponent(&mixSlider, false);
}

