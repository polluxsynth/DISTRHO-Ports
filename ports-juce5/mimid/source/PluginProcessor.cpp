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
	: programs()
	, configLock("__" JucePlugin_Name "ConfigLock__")
{
	isHostAutomatedChange = true;

	synth.setSampleRate(44100);

	PropertiesFile::Options options;
	options.applicationName = JucePlugin_Name;
	options.storageFormat = PropertiesFile::storeAsXML;
	options.millisecondsBeforeSaving = 2500;
	options.processLock = &configLock;
	config = new PropertiesFile(getDocumentFolder().getChildFile("Settings.xml"), options);

	currentSkin = config->containsKey("skin") ? config->getValue("skin") : "Pollux Grey";
	currentBank = "Init";

	scanAndUpdateBanks();
	initAllParams();

	if (bankFiles.size() > 0)
	{
		loadFromFXBFile(bankFiles[0]);
	}
}

MimidAudioProcessor::~MimidAudioProcessor()
{
	config->saveIfNeeded();
	config = nullptr;
}

//==============================================================================
void MimidAudioProcessor::initAllParams()
{
	for (int i = 0 ; i < PARAM_COUNT; i++)
	{
		setParameter(i, programs.currentProgramPtr->values[i]);
	}
}

//==============================================================================
int MimidAudioProcessor::getNumParameters()
{
	return PARAM_COUNT;
}

float MimidAudioProcessor::getParameter (int index)
{
	return programs.currentProgramPtr->values[index];
}

void MimidAudioProcessor::setParameter (int index, float newValue)
{
	programs.currentProgramPtr->values[index] = newValue;
	switch(index)
	{
	case PW_ENV_BOTH:
		synth.processPwEnvBoth(newValue);
		break;
	case OSC2PW:
		synth.processOsc2PulseWidth(newValue);
		break;
	case ENV_PITCH_BOTH:
		synth.processPitchModBoth(newValue);
		break;
	case FENV_INVERT:
		synth.processInvertFenv(newValue);
		break;
	case LEVEL_DIF:
		synth.processLoudnessDetune(newValue);
		break;
	case PW_ENV:
		synth.processPwEnv(newValue);
		break;
	case LFO_SYNC:
		synth.procLfoSync(newValue);
		break;
	case ECONOMY_MODE:
		synth.procEconomyMode(newValue);
		break;
	case VAMPENV:
		synth.procAmpVelocityAmount(newValue);
		break;
	case VFLTENV:
		synth.procFltVelocityAmount(newValue);
		break;
	case ASPLAYEDALLOCATION:
		synth.procAsPlayedAlloc(newValue);
		break;
	case BENDLFORATE:
		synth.procModWheelFrequency(newValue);
		break;
	case LEGATOMODE:
		synth.processLegatoMode(newValue);
		break;
	case ENVPITCH:
		synth.processEnvelopeToPitch(newValue);
		break;
	case OSCQuantize:
		synth.processPitchQuantization(newValue);
		break;
	case VOICE_COUNT:
		synth.setVoiceCount(newValue);
		break;
	case FILTER_WARM:
		synth.processOversampling(newValue);
		break;
	case FILTER_TYPE:
		synth.processFilterType(newValue);
		break;
	case BENDOSC2:
		synth.procPitchWheelOsc2Only(newValue);
		break;
	case BENDRANGE:
		synth.procPitchWheelAmount(newValue);
		break;
	case NOISEMIX:
		synth.processNoiseMix(newValue);
		break;
	case OCTAVE:
		synth.processOctave(newValue);
		break;
	case TUNE:
		synth.processTune(newValue);
		break;
	case BRIGHTNESS:
		synth.processBrightness(newValue);
		break;
	case RESPONSE:
		synth.processResponse(newValue);
		break;
	case LFOFREQ:
		synth.processLfoFrequency(newValue);
		break;
	case LFO1AMT:
		synth.processLfoAmt1(newValue);
		break;
	case LFO2AMT:
		synth.processLfoAmt2(newValue);
		break;
	case LFOSINWAVE:
		synth.processLfoSine(newValue);
		break;
	case LFOSQUAREWAVE:
		synth.processLfoSquare(newValue);
		break;
	case LFOSHWAVE:
		synth.processLfoSH(newValue);
		break;
	case LFOFILTER:
		synth.processLfoFilter(newValue);
		break;
	case LFOOSC1:
		synth.processLfoOsc1(newValue);
		break;
	case LFOOSC2:
		synth.processLfoOsc2(newValue);
		break;
	case LFOPW1:
		synth.processLfoPw1(newValue);
		break;
	case LFOPW2:
		synth.processLfoPw2(newValue);
		break;
	case PORTADER:
		synth.processPortamentoDetune(newValue);
		break;
	case FILTERDER:
		synth.processFilterDetune(newValue);
		break;
	case ENVDER:
		synth.processEnvelopeDetune(newValue);
		break;
	case XMOD:
		synth.processOsc2Xmod(newValue);
		break;
	case OSC2HS:
		synth.processOsc2HardSync(newValue);
		break;
	case OSC2P:
		synth.processOsc2Pitch(newValue);
		break;
	case OSC1P:
		synth.processOsc1Pitch(newValue);
		break;
	case PORTAMENTO:
		synth.processPortamento(newValue);
		break;
	case UNISON:
		synth.processUnison(newValue);
		break;
	case FLT_KF:
		synth.processFilterKeyFollow(newValue);
		break;
	case OSC1MIX:
		synth.processOsc1Mix(newValue);
		break;
	case OSC2MIX:
		synth.processOsc2Mix(newValue);
		break;
	case OSC1PW:
		synth.processOsc1PulseWidth(newValue);
		break;
	case OSC1WAVE:
		synth.processOsc1Wave(newValue);
		break;
	case OSC2WAVE:
		synth.processOsc2Wave(newValue);
		break;
	case VOLUME:
		synth.processVolume(newValue);
		break;
	case UDET:
		synth.processDetune(newValue);
		break;
	case OSC2_DET:
		synth.processOsc2Det(newValue);
		break;
	case CUTOFF:
		synth.processCutoff(newValue);
		break;
	case RESONANCE:
		synth.processResonance(newValue);
		break;
	case ENVELOPE_AMT:
		synth.processFilterEnvelopeAmt(newValue);
		break;
	case LATK:
		synth.processLoudnessEnvelopeAttack(newValue);
		break;
	case LDEC:
		synth.processLoudnessEnvelopeDecay(newValue);
		break;
	case LSUS:
		synth.processLoudnessEnvelopeSustain(newValue);
		break;
	case LSUST:
		synth.processLoudnessEnvelopeSustainTime(newValue);
		break;
	case LREL:
		synth.processLoudnessEnvelopeRelease(newValue);
		break;
	case FATK:
		synth.processFilterEnvelopeAttack(newValue);
		break;
	case FDEC:
		synth.processFilterEnvelopeDecay(newValue);
		break;
	case FSUS:
		synth.processFilterEnvelopeSustain(newValue);
		break;
	case FSUST:
		synth.processFilterEnvelopeSustainTime(newValue);
		break;
	case FREL:
		synth.processFilterEnvelopeRelease(newValue);
		break;
	case PAN1:
		synth.processPan(newValue,1);
		break;
	case PAN2:
		synth.processPan(newValue,2);
		break;
	case PAN3:
		synth.processPan(newValue,3);
		break;
	case PAN4:
		synth.processPan(newValue,4);
		break;
	case PAN5:
		synth.processPan(newValue,5);
		break;
	case PAN6:
		synth.processPan(newValue,6);
		break;
	case PAN7:
		synth.processPan(newValue,7);
		break;
	case PAN8:
		synth.processPan(newValue,8);
		break;
	}
	//DIRTY HACK
	//This should be checked to avoid stalling on gui update
	//It is needed because some hosts do  wierd stuff
	if(isHostAutomatedChange)
		sendChangeMessage();
}

const String MimidAudioProcessor::getParameterName (int index)
{
	switch(index)
	{
	case SELF_OSC_PUSH_NOTUSED:
		return S("SelfOscPush_NOTUSED");
	case ENV_PITCH_BOTH:
		return S("EnvPitchBoth");
	case FENV_INVERT:
		return S("FenvInvert");
	case OSC2PW:
		return S("Osc2PW");
	case LEVEL_DIF:
		return S("LevelDif");
	case PW_ENV_BOTH:
		return S("PwEnvBoth");
	case PW_ENV:
		return S("PwEnv");
	case LFO_SYNC:
		return S("LfoSync");
	case ECONOMY_MODE:
		return S("EconomyMode");
	case UNUSED_1:
		return S("Unused 1");
	case UNUSED_2:
		return S("Unused 2");
	case VAMPENV:
		return S("VAmpFactor");
	case VFLTENV:
		return S("VFltFactor");
	case ASPLAYEDALLOCATION:
		return S("AsPlayedAllocation");
	case BENDLFORATE:
		return S("VibratoRate");
	case FOURPOLE_NOTUSED:
		return S("FourPole_NOTUSED");
	case LEGATOMODE:
		return S("LegatoMode");
	case ENVPITCH:
		return S("EnvelopeToPitch");
	case OSCQuantize:
		return S("PitchQuant");
	case VOICE_COUNT:
		return S("VoiceCount");
	case BANDPASS_NOTUSED:
		return S("BandpassBlend_NOTUSED");
	case FILTER_WARM:
		return S("Filter_Warm");
	case FILTER_TYPE:
		return S("FilterType");
	case BENDRANGE:
		return S("BendRange");
	case BENDOSC2:
		return S("BendOsc2Only");
	case OCTAVE:
		return S("Octave");
	case TUNE:
		return S("Tune");
	case BRIGHTNESS:
		return S("Brightness");
	case NOISEMIX:
		return S("NoiseMix");
	case OSC1MIX:
		return S("Osc1Mix");
	case OSC2MIX:
		return S("Osc2Mix");
	case RESPONSE:
		return S("Response");
	case LFOSHWAVE:
		return S("LfoSampleHoldWave");
	case LFOSINWAVE:
		return S("LfoSineWave");
	case LFOSQUAREWAVE:
		return S("LfoSquareWave");
	case LFO1AMT:
		return S("LfoAmount1");
	case LFO2AMT:
		return S("LfoAmount2");
	case LFOFILTER:
		return S("LfoFilter");
	case LFOOSC1:
		return S("LfoOsc1");
	case LFOOSC2:
		return S("LfoOsc2");
	case LFOFREQ:
		return S("LfoFrequency");
	case LFOPW1:
		return S("LfoPw1");
	case LFOPW2:
		return S("LfoPw2");
	case PORTADER:
		return S("PortamentoDetune");
	case FILTERDER:
		return S("FilterDetune");
	case ENVDER:
		return S("EnvelopeDetune");
	case PAN1:
		return S("Pan1");
	case PAN2:
		return S("Pan2");
	case PAN3:
		return S("Pan3");
	case PAN4:
		return S("Pan4");
	case PAN5:
		return S("Pan5");
	case PAN6:
		return S("Pan6");
	case PAN7:
		return S("Pan7");
	case PAN8:
		return S("Pan8");
	case XMOD:
		return S("Xmod");
	case OSC2HS:
		return S("Osc2HardSync");
	case OSC1P:
		return S("Osc1Pitch");
	case OSC2P:
		return S("Osc2Pitch");
	case PORTAMENTO:
		return S("Portamento");
	case UNISON:
		return S("Unison");
	case FLT_KF:
		return S("FilterKeyFollow");
	case OSC1PW:
		return S("Osc1PW");
	case OSC2Saw_NOTUSED:
		return S("Osc2Saw_NOTUSED");
	case OSC1Saw_NOTUSED:
		return S("Osc1Saw_NOTUSED");
	case OSC1Pul_NOTUSED:
		return S("Osc1Pulse_NOTUSED");
	case OSC2Pul_NOTUSED:
		return S("Osc2Pulse_NOTUSED");
	case OSC1WAVE:
		return S("Osc1Wave");
	case OSC2WAVE:
		return S("Osc2Wave");
	case VOLUME:
		return S("Volume");
	case UDET:
		return S("VoiceDetune");
	case OSC2_DET:
		return S("Oscillator2detune");
	case CUTOFF:
		return S("Cutoff");
	case RESONANCE:
		return S("Resonance");
	case ENVELOPE_AMT:
		return S("FilterEnvAmount");
	case LATK:
		return S("Attack");
	case LDEC:
		return S("Decay");
	case LSUS:
		return S("Sustain");
	case LSUST:
		return S("SustainTime");
	case LREL:
		return S("Release");
	case FATK:
		return S("FilterAttack");
	case FDEC:
		return S("FilterDecay");
	case FSUS:
		return S("FilterSustain");
	case FSUST:
		return S("FilterSustainTime");
	case FREL:
		return S("FilterRelease");
	}
	return String();
}

const String MimidAudioProcessor::getParameterText (int index)
{
	return String(programs.currentProgramPtr->values[index],2);
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
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool MimidAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
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
	return PROGRAMCOUNT;
}

int MimidAudioProcessor::getCurrentProgram()
{
	return programs.currentProgram;
}

void MimidAudioProcessor::setCurrentProgram (int index)
{
	programs.currentProgram = index;
	programs.currentProgramPtr = programs.programs + programs.currentProgram;
	isHostAutomatedChange = false;
	for(int i = 0 ; i < PARAM_COUNT;i++)
		setParameter(i,programs.currentProgramPtr->values[i]);
	isHostAutomatedChange = true;
	sendChangeMessage();
	updateHostDisplay();
}

const String MimidAudioProcessor::getProgramName (int index)
{
	return programs.programs[index].name;
}

void MimidAudioProcessor::changeProgramName (int index, const String& newName)
{
	 programs.programs[index].name = newName;
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
	XmlElement xmlState = XmlElement("Pollux");
	xmlState.setAttribute(S("currentProgram"), programs.currentProgram);

	XmlElement* xprogs = new XmlElement("programs");
	for (int i = 0; i < PROGRAMCOUNT; ++i)
	{
		XmlElement* xpr = new XmlElement("program");
		xpr->setAttribute(S("programName"), programs.programs[i].name);

		for (int k = 0; k < PARAM_COUNT; ++k)
		{
			xpr->setAttribute(String(k), programs.programs[i].values[k]);
		}

		xprogs->addChildElement(xpr);
	}

	xmlState.addChildElement(xprogs);

	copyXmlToBinary(xmlState,destData);
}

void MimidAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	if (XmlElement* const xmlState = getXmlFromBinary(data,sizeInBytes))
	{
		XmlElement* xprogs = xmlState->getFirstChildElement();
		if (xprogs->hasTagName(S("programs")))
		{
			int i = 0;
			forEachXmlChildElement(*xprogs, e)
			{
				programs.programs[i].setDefaultValues();

				for (int k = 0; k < PARAM_COUNT; ++k)
				{
					programs.programs[i].values[k] = e->getDoubleAttribute(String(k), programs.programs[i].values[k]);
				}

				programs.programs[i].name = e->getStringAttribute(S("programName"), S("Default"));

				++i;
			}
		}

		setCurrentProgram(xmlState->getIntAttribute(S("currentProgram"), 0));

		delete xmlState;
	}
}

void  MimidAudioProcessor::setCurrentProgramStateInformation(const void* data,int sizeInBytes)
{
	if (XmlElement* const e = getXmlFromBinary(data, sizeInBytes))
	{
		programs.currentProgramPtr->setDefaultValues();

		for (int k = 0; k < PARAM_COUNT; ++k)
		{
			programs.currentProgramPtr->values[k] = e->getDoubleAttribute(String(k), programs.currentProgramPtr->values[k]);
		}

		programs.currentProgramPtr->name =  e->getStringAttribute(S("programName"), S("Default"));

		setCurrentProgram(programs.currentProgram);

		delete e;
	}
}

void MimidAudioProcessor::getCurrentProgramStateInformation(MemoryBlock& destData)
{
	XmlElement xmlState = XmlElement("Pollux");

	for (int k = 0; k < PARAM_COUNT; ++k)
	{
		xmlState.setAttribute(String(k), programs.currentProgramPtr->values[k]);
	}

	xmlState.setAttribute(S("programName"), programs.currentProgramPtr->name);

	copyXmlToBinary(xmlState, destData);
}

//==============================================================================
bool MimidAudioProcessor::loadFromFXBFile(const File& fxbFile)
{
	MemoryBlock mb;
	if (! fxbFile.loadFileAsData(mb))
		return false;

	const void* const data = mb.getData();
	const size_t dataSize = mb.getSize();

	if (dataSize < 28)
		return false;

	const fxSet* const set = (const fxSet*) data;

	if ((! compareMagic (set->chunkMagic, "CcnK")) || fxbSwap (set->version) > fxbVersionNum)
		return false;

	if (compareMagic (set->fxMagic, "FxBk"))
	{
		// bank of programs
		if (fxbSwap (set->numPrograms) >= 0)
		{
			const int oldProg = getCurrentProgram();
			const int numParams = fxbSwap (((const fxProgram*) (set->programs))->numParams);
			const int progLen = (int) sizeof (fxProgram) + (numParams - 1) * (int) sizeof (float);

			for (int i = 0; i < fxbSwap (set->numPrograms); ++i)
			{
				if (i != oldProg)
				{
					const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + i * progLen);
					if (((const char*) prog) - ((const char*) set) >= (ssize_t) dataSize)
						return false;

					if (fxbSwap (set->numPrograms) > 0)
						setCurrentProgram (i);

					if (! restoreProgramSettings (prog))
						return false;
				}
			}

			if (fxbSwap (set->numPrograms) > 0)
				setCurrentProgram (oldProg);

			const fxProgram* const prog = (const fxProgram*) (((const char*) (set->programs)) + oldProg * progLen);
			if (((const char*) prog) - ((const char*) set) >= (ssize_t) dataSize)
				return false;

			if (! restoreProgramSettings (prog))
				return false;
		}
	}
	else if (compareMagic (set->fxMagic, "FxCk"))
	{
		// single program
		const fxProgram* const prog = (const fxProgram*) data;

		if (! compareMagic (prog->chunkMagic, "CcnK"))
			return false;

		changeProgramName (getCurrentProgram(), prog->prgName);

		for (int i = 0; i < fxbSwap (prog->numParams); ++i)
			setParameter (i, fxbSwapFloat (prog->params[i]));
	}
	else if (compareMagic (set->fxMagic, "FBCh"))
	{
		// non-preset chunk
		const fxChunkSet* const cset = (const fxChunkSet*) data;

		if ((size_t) fxbSwap (cset->chunkSize) + sizeof (fxChunkSet) - 8 > (size_t) dataSize)
			return false;

		setStateInformation(cset->chunk, fxbSwap (cset->chunkSize));
	}
	else if (compareMagic (set->fxMagic, "FPCh"))
	{
		// preset chunk
		const fxProgramSet* const cset = (const fxProgramSet*) data;

		if ((size_t) fxbSwap (cset->chunkSize) + sizeof (fxProgramSet) - 8 > (size_t) dataSize)
			return false;

		setCurrentProgramStateInformation(cset->chunk, fxbSwap (cset->chunkSize));

		changeProgramName (getCurrentProgram(), cset->name);
	}
	else
	{
		return false;
	}

	currentBank = fxbFile.getFileName();

	updateHostDisplay();

	return true;
}

bool MimidAudioProcessor::restoreProgramSettings(const fxProgram* const prog)
{
	if (compareMagic (prog->chunkMagic, "CcnK")
		&& compareMagic (prog->fxMagic, "FxCk"))
	{
		changeProgramName (getCurrentProgram(), prog->prgName);

		for (int i = 0; i < fxbSwap (prog->numParams); ++i)
			setParameter (i, fxbSwapFloat (prog->params[i]));

		return true;
	}

	return false;
}

//==============================================================================
void MimidAudioProcessor::scanAndUpdateBanks()
{
	bankFiles.clearQuick();

	DirectoryIterator it(getBanksFolder(), false, "*.fxb", File::findFiles);
	while (it.next())
	{
		bankFiles.add(it.getFile());
	}
}

const Array<File>& MimidAudioProcessor::getBankFiles() const
{
	return bankFiles;
}

File MimidAudioProcessor::getCurrentBankFile() const
{
	return getBanksFolder().getChildFile(currentBank);
}

//==============================================================================
File MimidAudioProcessor::getDocumentFolder() const
{
	File folder = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Pollux").getChildFile("MiMi-d");
	if (folder.isSymbolicLink())
		folder = folder.getLinkedTarget();
	return folder;
}

File MimidAudioProcessor::getSkinFolder() const
{
	return getDocumentFolder().getChildFile("Skins");
}

File MimidAudioProcessor::getBanksFolder() const
{
	return getDocumentFolder().getChildFile("Banks");
}

File MimidAudioProcessor::getCurrentSkinFolder() const
{
	return getSkinFolder().getChildFile(currentSkin);
}

void MimidAudioProcessor::setCurrentSkinFolder(const String& folderName)
{
	currentSkin = folderName;

	config->setValue("skin", folderName);
	config->setNeedsToBeSaved(true);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MimidAudioProcessor();
}
