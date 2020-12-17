/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpSimAudioProcessorEditor::AmpSimAudioProcessorEditor (AmpSimAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    gainKnob.setName("Volume");
    gainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(gainKnob);
    
    gainKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "INPUTGAIN", gainKnob);
    
    volumeKnob.setName("Volume");
    volumeKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(volumeKnob);
    
    volumeKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "VOLUME", volumeKnob);

    loadIrBtn.setButtonText("Load IR");
    loadIrBtn.onClick = [&]() {
        juce::String fileName = audioProcessor.loadImpulseResponse();
        irNameLabel.setText(fileName, juce::NotificationType::dontSendNotification);
    };
    
    addAndMakeVisible(loadIrBtn);
    
    irNameLabel.setText("No IR loaded...", juce::NotificationType::dontSendNotification);
    irNameLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(irNameLabel);
    
    volumeLabel.setText(volumeKnob.getName(), juce::NotificationType::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volumeLabel);
    
    lowEqGain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lowEqGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(lowEqGain);
    lowEqGainbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "LOWEQGAIN", lowEqGain);
    
    midEqGain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    midEqGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(midEqGain);
    midEqGainbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "MIDEQGAIN", midEqGain);
    
    hiEqGain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    hiEqGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(hiEqGain);
    hiEqGainbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "HIEQGAIN", hiEqGain);
    
    presenceGain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    presenceGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(presenceGain);
    presenceGainbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.state, "PRESENCE", presenceGain);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1280, 768);
}

AmpSimAudioProcessorEditor::~AmpSimAudioProcessorEditor()
{
}

//==============================================================================
void AmpSimAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    g.fillAll(juce::Colours::darkgrey);
}

void AmpSimAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // IR
    loadIrBtn.setBounds(10, 10, 150, 40);
    irNameLabel.setBounds(170, 10, 500, 40);
    
    // Gain
    //Volume
    gainKnob.setBounds(getWidth() - 310, 50, 140, 140);
    
    //Volume
    volumeLabel.setBounds(getWidth() - 150, 10, 140, 40);
    volumeKnob.setBounds(getWidth() - 150, 50, 140, 140);
    
    // EQ
    lowEqGain.setBounds(20, 200, 140, 140);
    midEqGain.setBounds(180, 200, 140, 140);
    hiEqGain.setBounds(340, 200, 140, 140);
    presenceGain.setBounds(500, 200, 140, 140);
}
