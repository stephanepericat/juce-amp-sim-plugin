/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define DEFAULT_VOLUME .16f
#define DEFAULT_GAIN 7.57f
#define DEFAULT_BASS_EQ 3.56f
#define DEFAULT_MID_EQ 2.72f
#define DEFAULT_TREBLE_EQ 3.64f
#define DEFAULT_PRESENCE 1.63f

//==============================================================================
/**
*/
class AmpSimAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AmpSimAudioProcessor();
    ~AmpSimAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    juce::AudioProcessorValueTreeState state;
    juce::String currentIrName = "No IR loaded...";
    
    juce::String loadImpulseResponse();
    void updateEQ();
    void updateInput();
    void updatePreamp();
    void updateVolume();
    static float asymptoticClipping(float x);
//    static float diodeClipping(float sample);
    static float arcTanClipping(float x);
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    using FilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Gain = juce::dsp::Gain<float>;
    using Shaper = juce::dsp::WaveShaper<float>;
    
//    juce::dsp::ProcessorChain<FilterBand, FilterBand, Gain, Shaper, FilterBand, FilterBand, Gain, Shaper> preamp;
    juce::dsp::ProcessorChain<FilterBand, FilterBand, Gain, Shaper, FilterBand, FilterBand, Gain, Shaper, Gain> preamp;
    juce::dsp::ProcessorChain<FilterBand, FilterBand, FilterBand, FilterBand> eq;
    
    juce::dsp::Gain<float> inputGain;
//    juce::dsp::WaveShaper<float> gainShaper { juce::dsp::FastMathApproximations::tanh };
//    juce::dsp::WaveShaper<float> gainShaper { asymptoticClipping };
//    juce::dsp::WaveShaper<float> gainShaper { diodeClipping };
    juce::dsp::WaveShaper<float> gainShaper { arcTanClipping };

    juce::dsp::Convolution cab;
    
    juce::dsp::Gain<float> outputVolume;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpSimAudioProcessor)
};
