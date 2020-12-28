/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpSimAudioProcessor::AmpSimAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), state(*this, nullptr, "parameters", createParams())
#endif
{
    outputVolume.setGainLinear(DEFAULT_VOLUME);
}

AmpSimAudioProcessor::~AmpSimAudioProcessor()
{
}

//==============================================================================
const juce::String AmpSimAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmpSimAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmpSimAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmpSimAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmpSimAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmpSimAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmpSimAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmpSimAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmpSimAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmpSimAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmpSimAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec = juce::dsp::ProcessSpec();
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    inputGain.prepare(spec);
    inputPostGain.prepare(spec);
    
    inputShaper.prepare(spec);
    inputShaper.reset();
    
    eq.prepare(spec);
    eq.reset();
    
    cab.prepare(spec);
    outputVolume.prepare(spec);
}

void AmpSimAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmpSimAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AmpSimAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    juce::dsp::AudioBlock<float> audioBlock = juce::dsp::AudioBlock<float>(buffer);
    
    // gain
    updateInput();
    inputGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    inputShaper.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    inputPostGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    
    // EQ
    updateEQ();
    eq.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    
    // Cab Impulse Response
    cab.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    
    // Output Volume
    updateVolume();
    outputVolume.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
}

//==============================================================================
bool AmpSimAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmpSimAudioProcessor::createEditor()
{
    return new AmpSimAudioProcessorEditor (*this);
}

//==============================================================================
void AmpSimAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AmpSimAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpSimAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout AmpSimAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("INPUTGAIN", "InputGain", juce::NormalisableRange<float>(-12.f, 12.f, .1f), DEFAULT_GAIN, "dB"));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWEQGAIN", "LowEqGain", 0.1f, 10.f, DEFAULT_BASS_EQ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIDEQGAIN", "MidEqGain", 0.1f, 10.f, DEFAULT_MID_EQ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("HIEQGAIN", "HiEqGain", 0.1f, 10.f, DEFAULT_TREBLE_EQ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PRESENCE", "Presence", 0.1f, 3.f, DEFAULT_PRESENCE));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume", juce::NormalisableRange<float>(-12.f, 12.f, .1f), DEFAULT_VOLUME, "dB"));
    
    return { params.begin(), params.end() };
}

juce::String AmpSimAudioProcessor::loadImpulseResponse()
{
    juce::FileChooser chooser { "Please load an impulse response" };

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();

        cab.loadImpulseResponse(
            file,
            juce::dsp::Convolution::Stereo::no,
            juce::dsp::Convolution::Trim::no,
            file.getSize()
        );
        
        currentIrName = file.getFileName();
    }
    
    return currentIrName;
}

void AmpSimAudioProcessor::updateEQ()
{
    float sampleRate = getSampleRate();

    auto LEG = state.getRawParameterValue("LOWEQGAIN");
    float lowEqGainValue = LEG->load();
    *eq.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 200.f, .7071f, lowEqGainValue);
    
    auto MEG = state.getRawParameterValue("MIDEQGAIN");
    float midEqGainValue = MEG->load();
    *eq.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1700.f, .7071f, midEqGainValue);
    
    auto HEG = state.getRawParameterValue("HIEQGAIN");
    float hiEqGainValue = HEG->load();
    *eq.get<2>().state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 3500.f, .7071f, hiEqGainValue);
    
    auto PRS = state.getRawParameterValue("PRESENCE");
    float presenceValue = PRS->load();
    *eq.get<3>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3900.f, .7071f, presenceValue);
}

void AmpSimAudioProcessor::updateInput()
{
    auto IN = state.getRawParameterValue("INPUTGAIN");
    float inputVal = IN->load();
    float postVal = inputVal > 0.f ? -((2 / juce::MathConstants<float>::pi) * inputVal) : 0.f;

    inputGain.setGainDecibels(inputVal);
    // compensating...
    inputPostGain.setGainDecibels(postVal);
}

void AmpSimAudioProcessor::updateVolume()
{
    auto VOL = state.getRawParameterValue("VOLUME");
    float val = VOL->load();
    outputVolume.setGainDecibels(val);
}

float AmpSimAudioProcessor::asymptoticClipping(float x)
{
    return x / (abs(x) + 1);
}

float AmpSimAudioProcessor::arcTanClipping(float x)
{
    // TODO: see if coef can be dynamic...
    return 2.f / juce::MathConstants<float>::pi * juce::dsp::FastMathApproximations::tanh(juce::MathConstants<float>::twoPi * x);
}
