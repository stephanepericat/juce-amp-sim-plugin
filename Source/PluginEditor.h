/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AmpSimAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AmpSimAudioProcessorEditor (AmpSimAudioProcessor&);
    ~AmpSimAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label irNameLabel;
    juce::TextButton loadIrBtn;
    
    juce::Slider volumeKnob;
    juce::Label volumeLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeKnobAttachment;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AmpSimAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpSimAudioProcessorEditor)
};
