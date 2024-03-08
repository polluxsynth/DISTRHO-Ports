/*
==============================================================================

This file was auto-generated!

It contains the basic startup code for a Juce application.

Manually patched for initial work on the MiMi-d synthesizer.

Copyright 2023 Ricard Wanderlof

==============================================================================
*/
#include "PluginProcessor.h"
#include "Engine/Params.h"

//==============================================================================
#define S(T) (juce::String(T))

//==============================================================================
MimidAudioProcessor::MimidAudioProcessor()
{
	synth.setSampleRate(44100);

	// Bring in parameter definitions

#define SEFUNC(FUNCNAME) &SynthEngine::FUNCNAME

#define PARAM(PARAMNO, NAME, MIN, MAX, DEFAULT, SETFUNC) \
	paramdef[PARAMNO].name = S(NAME); \
	paramdef[PARAMNO].setFunc = SEFUNC(SETFUNC);

#define PARAM_NULL(PARAMNO, NAME) \
	paramdef[PARAMNO].name = S(NAME); \
	paramdef[PARAMNO].setFunc = NULL;
#include "Engine/ParamDefs.h"

	initAllParams();
}

MimidAudioProcessor::~MimidAudioProcessor()
{
}

//==============================================================================

void MimidAudioProcessor::initAllParams()
{
	for (int i = 0 ; i < PARAM_COUNT; i++)
		setParameter(i, parameters.values[i]);
}

int MimidAudioProcessor::getNumParameters()
{
	return PARAM_COUNT;
}

float MimidAudioProcessor::getParameter (int index)
{
	return parameters.values[index];
}

void MimidAudioProcessor::setParameter (int index, float newValue)
{
	parameters.values[index] = newValue;
	if (index >= 0 && index < PARAM_COUNT && paramdef[index].setFunc)
		(synth.*paramdef[index].setFunc)(newValue);
}

const String MimidAudioProcessor::getParameterName (int index)
{
	if (index >= 0 && index < PARAM_COUNT)
		return paramdef[index].name;
	return String();
}

const String MimidAudioProcessor::getParameterText (int index)
{
	return String(parameters.values[index],2);
}

//==============================================================================
const String MimidAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

const String MimidAudioProcessor::getInputChannelName (int channelIndex) const
{
	return String (channelIndex + 1);
}

const String MimidAudioProcessor::getOutputChannelName (int channelIndex) const
{
	return String (channelIndex + 1);
}

bool MimidAudioProcessor::isInputChannelStereoPair (int index) const
{
	return true;
}

bool MimidAudioProcessor::isOutputChannelStereoPair (int index) const
{
	return true;
}

bool MimidAudioProcessor::acceptsMidi() const
{
	return true;
}

bool MimidAudioProcessor::producesMidi() const
{
	return false;
}

bool MimidAudioProcessor::silenceInProducesSilenceOut() const
{
	return false;
}

double MimidAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

//==============================================================================
int MimidAudioProcessor::getNumPrograms()
{
	return 1;
}

int MimidAudioProcessor::getCurrentProgram()
{
	return 0;
}

void MimidAudioProcessor::setCurrentProgram (int index)
{
}

const String MimidAudioProcessor::getProgramName (int index)
{
	return "Nothing";
}

void MimidAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MimidAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	nextMidi = MidiMessage(0xF0);
	midiMsg  = MidiMessage(0xF0);
	synth.setSampleRate(sampleRate);
}

void MimidAudioProcessor::releaseResources()
{

}

inline void MimidAudioProcessor::processMidiPerSample(MidiBuffer::Iterator* iter,const int samplePos)
{
	while (getNextEvent(iter, samplePos))
	{
		if(midiMsg.isNoteOn())
		{
			synth.procNoteOn(midiMsg.getNoteNumber(),midiMsg.getFloatVelocity());
		}
		if (midiMsg.isNoteOff())
		{
			synth.procNoteOff(midiMsg.getNoteNumber());
		}
		if(midiMsg.isPitchWheel())
		{
			// [0..16383] center = 8192;
			synth.procPitchWheel((midiMsg.getPitchWheelValue()-8192) / 8192.0);
		}
		if(midiMsg.isController() && midiMsg.getControllerNumber()==1)
			synth.procModWheel(midiMsg.getControllerValue() / 127.0);
		if(midiMsg.isChannelPressure())
			synth.procAfterTouch(midiMsg.getChannelPressureValue() / 127.0);
		if(midiMsg.isSustainPedalOn())
		{
			synth.sustainOn();
		}
		if(midiMsg.isSustainPedalOff() || midiMsg.isAllNotesOff()||midiMsg.isAllSoundOff())
		{
			synth.sustainOff();
		}
		if(midiMsg.isAllNotesOff())
		{
			synth.allNotesOff();
		}
		if(midiMsg.isAllSoundOff())
		{
			synth.allSoundOff();
		}

	}
}

bool MimidAudioProcessor::getNextEvent(MidiBuffer::Iterator* iter,const int samplePos)
{
	if (hasMidiMessage && midiEventPos <= samplePos)
	{
		midiMsg = nextMidi;
		hasMidiMessage = iter->getNextEvent(nextMidi, midiEventPos);
		return true;
	} 
	return false;
}

void MimidAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	MidiBuffer::Iterator ppp(midiMessages);
	hasMidiMessage = ppp.getNextEvent(nextMidi,midiEventPos);

	int samplePos = 0;
	int numSamples = buffer.getNumSamples();
	float* channelData1 = buffer.getWritePointer(0);
	float* channelData2 = buffer.getWritePointer(1);

	AudioPlayHead::CurrentPositionInfo pos;
    if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (pos))
    {
		synth.setPlayHead(pos.bpm,pos.ppqPosition);
    }

	while (samplePos < numSamples)
	{
		processMidiPerSample(&ppp,samplePos);

		synth.processSample(channelData1+samplePos,channelData2+samplePos);

		samplePos++;
	}
}

//==============================================================================
bool MimidAudioProcessor::hasEditor() const
{
	return false;
}

AudioProcessorEditor* MimidAudioProcessor::createEditor()
{
	return NULL;
}

//==============================================================================
void MimidAudioProcessor::getStateInformation (MemoryBlock& destData)
{
}

void MimidAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MimidAudioProcessor();
}
