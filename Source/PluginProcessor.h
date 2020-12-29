/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define DEFAULT_VOLUME -6.0f
#define DEFAULT_GAIN 30.0f
#define DEFAULT_BASS_EQ 1.90f
#define DEFAULT_MID_EQ 3.21f
#define DEFAULT_TREBLE_EQ 5.89f
#define DEFAULT_PRESENCE 1.39f

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
    void updateOverdrive();
    void updatePreGain();
    void updateVolume();
    
    static float asymptoticClipping(float sample);
    static float arcTanClipping(float sample);
    static float asymetricClipping(float sample);
    static float softClipping(float sample);
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    using FilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Gain = juce::dsp::Gain<float>;
    using Shaper = juce::dsp::WaveShaper<float>;
    using Convolution = juce::dsp::Convolution;
    using Bias = juce::dsp::Bias<float>;
    using OverSampling = juce::dsp::Oversampling<float>;

    juce::dsp::ProcessorChain<Gain, Bias, Shaper, Gain, FilterBand> overdrive;
    juce::dsp::ProcessorChain<FilterBand, FilterBand, FilterBand, FilterBand> eq;
    juce::dsp::ProcessorChain<FilterBand, FilterBand> postEq;
    
    Bias preGainBias1;
    
    OverSampling overdriveOV { 2, 2, OverSampling::filterHalfBandPolyphaseIIR, true, false };
    
    Gain inputGain, inputPostGain, preGain1, prePostGain1, outputVolume;

    Shaper inputShaper { arcTanClipping };
    Shaper preGainShaper1 { arcTanClipping };

    Convolution cab;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpSimAudioProcessor)
};
