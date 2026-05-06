//
// Created by alisdair chauvin on 2/24/26.
//

#include "SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent()
    : forwardFFT(fftOrder),
      spectrogramImage (juce::Image::RGB, 512, 512, true),
      fifo{},
      fftData{}
{
    startTimerHz(90);
    //DBG("Spectrogram size: " << getWidth() << " x " << getHeight());
}

SpectrogramComponent::~SpectrogramComponent() {
    stopTimer();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0, 0, 76));
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());

    // ===== Frequency scale overlay =====//
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::lightblue.withAlpha(0.25f));
    std::vector<float> frequencies = {20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f};
    for (float freq : frequencies)
    {
        // Normalize frequencies logarithmically for acurate positioning
        float normFreq = std::log10(freq / 20.0f) / std::log10(20000.0f / 20.0f);
        auto y = static_cast<float>(bounds.getHeight()) * (1.0f - normFreq);  // float
        auto yInt = juce::roundToInt(y);

        g.drawHorizontalLine(yInt, 0.0f, static_cast<float>(bounds.getWidth()));
        g.setFont(10.0f);
        g.drawText(juce::String(freq < 1000 ? juce::String(freq) + "Hz"
                                            : juce::String(freq / 1000, 1) + "kHz"),
                   5, yInt - 5, 60, 10, juce::Justification::left);
    }
}


void SpectrogramComponent::resized() {}

// ===== Push all samples in audio buffer for fifo processing ===== //
void SpectrogramComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) {
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);
        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo (channelData[i]);
    }
}

void SpectrogramComponent::pushNextSampleIntoFifo (float sample) noexcept {
    // if the fifo contains enough data, render next line
    if (fifoIndex >= fftSize / 2)
    {
        if (!nextFFTBlockReady)
        {
            // store sample in fifo
            std::ranges::fill (fftData.begin(), fftData.end(), 0.0f);
            std::ranges::copy (fifo.begin(), fifo.end(), fftData.begin());
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[static_cast<size_t>(fifoIndex++)] = sample; // increase index
}

void SpectrogramComponent::drawNextLineOfSpectrogram() {
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight = spectrogramImage.getHeight();

    // Shuffle  image leftwards by 1 pixel
    spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

    // Render FFT data
    forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());

    // Find range of values produced and scale rendering to clear detail
    auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData.data(), fftSize / 2);

    // BitmapData instance referring to rightmost column of pixels
    juce::Image::BitmapData bitmap { spectrogramImage, rightHandEdge, 0, 1, imageHeight, juce::Image::BitmapData::writeOnly };

    // For every pixel in height, calculate level proportional to sample set
    for (auto y = 1; y < imageHeight; ++y)
    {
        // log scale for frequencies
        auto skewedProportionY = 1.0f - std::exp (std::log (static_cast<float>(y) / static_cast<float>(imageHeight) * 0.35f));
        auto fftDataIndex = static_cast<size_t>(juce::jlimit (0, fftSize / 2, static_cast<int>(skewedProportionY * fftSize / 2)));
        auto level = juce::jmap (fftData[fftDataIndex], 0.0f,
                         juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        // Exposure: gain shifts the signal up the color ramp
        level *= gain;

        // Soft saturation — never hits 1.0 fully, preserves highlight color detail
        level = level / (1.0f + level);

        // Contrast curve — lower exponent = more visible quiet detail
        level = std::pow(level, 0.5f);

        // Mix
        level = level * mix + (1.0f - mix) * 0.1f;

        // Boost highlights to create glow effect
        float glow = juce::jlimit(0.0f, 1.0f, level * 1.5f);

        // Custom neon gradient (blue, purple, red, yellow, and white)
        juce::Colour colour;

        if (level < 0.2f)
        {
            // deep blue
            colour = juce::Colour::fromFloatRGBA(0.0f, level * 2.0f, 0.3f + level, 1.0f);
        }
        else if (level < 0.4f)
        {
            // purple
            float t = (level - 0.2f) / 0.2f;
            colour = juce::Colour::fromFloatRGBA(t, 0.0f, 1.0f - t, 1.0f);
        }
        else if (level < 0.7f)
        {
            // red
            float t = (level - 0.4f) / 0.3f;
            colour = juce::Colour::fromFloatRGBA(1.0f, t, 0.0f, 1.0f);
        }
        else
        {
            // white (hot peaks)
            float t = (level - 0.7f) / 0.3f;
            colour = juce::Colour::fromFloatRGBA(1.0f, 1.0f, t, 1.0f);
        }

        // apply glow
        colour = colour.brighter(glow * 0.5f);

        bitmap.setPixelColour(0, y, colour);
    }
}

void SpectrogramComponent::timerCallback() {
    if (nextFFTBlockReady) {
        drawNextLineOfSpectrogram();
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrogramComponent::setSpeed(double hz)
{
    startTimerHz(static_cast<int>(hz));
}

void SpectrogramComponent::setGain(float newGain)
{
    gain = newGain;
}

void SpectrogramComponent::setMix(float newMix)
{
    mix = newMix;
}