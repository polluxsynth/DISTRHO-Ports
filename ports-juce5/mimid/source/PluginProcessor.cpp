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

#define SE(FUNCNAME) &SynthEngine::FUNCNAME

#define PARAM(PARAMNO, NAME, SETFUNC) \
	paramdef[PARAMNO].name = S(NAME); \
	paramdef[PARAMNO].setFunc = SE(SETFUNC);

#define PARAM_NULL(PARAMNO, NAME) \
	paramdef[PARAMNO].name = S(NAME); \
	paramdef[PARAMNO].setFunc = NULL;

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

	PARAM(OSC2PW, "Osc2PW", processOsc2PulseWidth);
	PARAM_NULL(PW_ENV_BOTH_NOTUSED, "PwEnvBoth_NOTUSED");
	PARAM_NULL(ENV_PITCH_BOTH_NOTUSED, "EnvPitchBoth_NOTUSED");
	PARAM(FENV_INVERT, "FenvInvert", processInvertFenv);
	PARAM(LEVEL_DIF, "LevelSpread", processLoudnessSpread);
	PARAM_NULL(PW_ENV_NOTUSED, "PwEnv_NOTUSED");
	PARAM(LFO1SYNC, "Lfo1Sync", procLfo1Sync);
	PARAM(LFO2SYNC, "Lfo2Sync", procLfo2Sync);
	PARAM(ECONOMY_MODE, "EconomyMode", procEconomyMode);
	PARAM(VAMPENV, "VAmpFactor", procAmpVelocityAmount);
	// TODO Remove
	PARAM(UNUSED_1, "Unused 1", procUnused1);
	PARAM(UNUSED_2, "Unused 2", procUnused2);
	PARAM(VFLTENV, "VFltFactor", procFltVelocityAmount);
	PARAM_NULL(VGENENV_NOTUSED, "VGenFactor_NOTUSED");
	PARAM_NULL(ASPLAYEDALLOCATION_NOTUSED, "AsPlayedAllocation_NOTUSED");
	PARAM(LFO2FREQ, "Lfo2Frequency", processLfo2Frequency);
	PARAM_NULL(LEGATOMODE_NOTUSED, "LegatoMode_NOTUSED");
	PARAM_NULL(ENVPITCH_NOTUSED, "EnvelopeToPitch_NOTUSED");
	PARAM_NULL(OSCQuantize_NOTUSED, "PitchQuant_NOTUSED");
	PARAM(VOICE_COUNT, "VoiceCount", setVoiceCount);
	PARAM(OVERSAMPLE, "Oversample", processOversampling);
	PARAM(FILTER_TYPE, "FilterType", processFilterType);
	PARAM(BENDDEST, "BendDest", procPitchWheelDest);
	PARAM(BENDRANGE, "BendRange", procPitchWheelAmount);
	PARAM(OSC3MIX, "Osc3Mix", processOsc3Mix);
	PARAM(OCTAVE, "Octave", processOctave);
	PARAM(TUNE, "Tune", processTune);
	PARAM(HPFFREQ, "HPFfreq", processHPFfreq);
	PARAM(OSC1FLTMOD, "Osc1FilterMod", processOsc1FltMod);
	PARAM(RESPONSE, "Response", processResponse);
	PARAM(LFO1FREQ, "Lfo1Frequency", processLfo1Frequency);
	PARAM(LFO1AMT, "Lfo1Amount", processLfo1Amt);
	PARAM(LFO2AMT, "Lfo2Amount", processLfo2Amt);
	PARAM(LFO1WAVE, "Lfo1Wave", processLfo1Wave);
	PARAM(LFO2WAVE, "Lfo2Wave", processLfo2Wave);
	PARAM_NULL(LFOSHWAVE_UNUSED, "LfoSampleHoldWave");
	PARAM(LFOFILTER_UNUSED, "LfoFilter", processLfoFilter_unused);
	PARAM(LFO1DEST, "Lfo1Dest", processLfo1Dest);
	PARAM(LFO2DEST, "Lfo2Dest", processLfo2Dest);
	PARAM(LFOPW1_UNUSED, "LfoPw1", processLfoPw1_unused);
	PARAM(PORTADER, "PortamentoSpread", processPortamentoSpread);
	PARAM(FILTERDER, "FilterSpread", processFilterSpread);
	PARAM(ENVDER, "EnvelopeSpread", processEnvelopeSpread);
	PARAM(LFOSPREAD, "LfoSpread", processLfoSpread);
	PARAM(PANSPREAD, "PanSpread", processPanSpread);
	PARAM(XMOD, "Xmod", processOsc2Xmod);
	PARAM(OSCSYNC_LEVEL, "SyncLevel", processOsc2SyncLevel);
	PARAM(OSC2P, "Osc2Pitch", processOsc2Pitch);
	PARAM(OSC1P, "Osc1Pitch", processOsc1Pitch);
	PARAM(PORTAMENTO, "Portamento", processPortamento);
	PARAM(UNISON, "Unison", processUnison); // TODO part of keyasgn?
	PARAM(FLT_KF, "FilterKeyFollow", processFilterKeyFollow);
	PARAM(OSC1MIX, "Osc1Mix", processOsc1Mix);
	PARAM(OSC2MIX, "Osc2Mix", processOsc2Mix);
	PARAM(OSC1PW, "Osc1PW", processOsc1PulseWidth);
	PARAM(OSC1WAVE, "Osc1Wave", processOsc1Wave);
	PARAM(OSC2WAVE, "Osc2Wave", processOsc2Wave);
	PARAM(VOLUME, "Volume", processVolume);
	PARAM(UDET, "OscSpread", processOscSpread);
	PARAM(OSC1_DET, "Oscillator1detune", processOsc1Det);
	PARAM(OSC2_DET, "Oscillator2detune", processOsc2Det);
	PARAM(OSC3WAVE, "Osc3Wave", processOsc3Wave);
	PARAM(CUTOFF, "Cutoff", processCutoff);
	PARAM(RESONANCE, "Resonance", processResonance);
	PARAM(ENVELOPE_AMT, "FilterEnvAmount", processFilterEnvelopeAmt);
	PARAM(LATK, "Attack", processLoudnessEnvelopeAttack);
	PARAM(LDEC, "Decay", processLoudnessEnvelopeDecay);
	PARAM(LSUS, "Sustain", processLoudnessEnvelopeSustain);
	PARAM(LSUST, "SustainTime", processLoudnessEnvelopeSustainTime);
	PARAM(LREL, "Release", processLoudnessEnvelopeRelease);
	PARAM(FATK, "FilterAttack", processFilterEnvelopeAttack);
	PARAM(FDEC, "FilterDecay", processFilterEnvelopeDecay);
	PARAM(FSUS, "FilterSustain", processFilterEnvelopeSustain);
	PARAM(FSUST, "FilterSustainTime", processFilterEnvelopeSustainTime);
	PARAM(FREL, "FilterRelease", processFilterEnvelopeRelease);
	PARAM_NULL(GATK_NOTUSED, "GenAttack_NOTUSED");
	PARAM_NULL(GDEC_NOTUSED, "GenDecay_NOTUSED");
	PARAM_NULL(GSUS_NOTUSED, "GenSustain_NOTUSED");
	PARAM_NULL(GSUST_NOTUSED, "GenSustainTime_NOTUSED");
	PARAM_NULL(GREL_NOTUSED, "GenRelease_NOTUSED");
	PARAM_NULL(GAMT_NOTUSED, "GenEnvAmount_NOTUSED");
	PARAM_NULL(GDEST_NOTUSED, "GenEnvDest_NOTUSED");
	PARAM_NULL(GUNI_NOTUSED, "GenEnvUnipol_NOTUSED");
	PARAM_NULL(GINV_NOTUSED, "GenEnvInvert_NOTUSED");
	PARAM(ASGN_RSZ, "KeyAssignRsz", procKeyAsgnRsz);
	PARAM(ASGN_MEM, "KeyAssignMem", procKeyAsgnMem);
	PARAM(ASGN_ROB, "KeyAssignRob", procKeyAsgnRob);
	PARAM(ASGN_RES, "KeyAssignRes", procKeyAsgnRes);
	PARAM(ASGN_STRG, "KeyAssignStrg", procKeyAsgnStrg);
	PARAM(VCADRIVE, "VCADrive", processVCADrive);
	PARAM(LFO1_CONTRAMT, "Lfo1ContrAmt", procLfo1ControllerAmt);
	PARAM(LFO2_CONTRAMT, "Lfo2ContrAmt", procLfo2ControllerAmt);
	PARAM(LFO1_POLARITY, "Lfo1Polarity", procLfo1Polarity);
	PARAM(LFO2_POLARITY, "Lfo2Polarity", procLfo2Polarity);
	PARAM(VEL_SCALE, "VelocityScale", procVelocityScale);
	PARAM(AT_SCALE, "AfterTouchScale", procAfterTouchScale);
	PARAM(LFO1_AMT_CTRL, "Lfo1AmtCont", procLfo1Controller);
	PARAM(LFO2_AMT_CTRL, "Lfo2AmtCont", procLfo2Controller);
	PARAM(OSC_KEY_SYNC, "OscKeySync", procOscKeySync);
	PARAM(ENV_RST, "EnvRst", procEnvRst);
	PARAM_NULL(PAN1_NOTUSED, "Pan1_NOTUSED");
	PARAM_NULL(PAN2_NOTUSED, "Pan2_NOTUSED");
	PARAM_NULL(PAN3_NOTUSED, "Pan3_NOTUSED");
	PARAM_NULL(PAN4_NOTUSED, "Pan4_NOTUSED");
	PARAM_NULL(PAN5_NOTUSED, "Pan5_NOTUSED");
	PARAM_NULL(PAN6_NOTUSED, "Pan6_NOTUSED");
	PARAM_NULL(PAN7_NOTUSED, "Pan7_NOTUSED");
	PARAM_NULL(PAN8_NOTUSED, "Pan8_NOTUSED");
	PARAM_NULL(SELF_OSC_PUSH_NOTUSED, "SelfOscPush_NOTUSED");
	PARAM_NULL(FOURPOLE_NOTUSED, "FourPole_NOTUSED");
	PARAM_NULL(BANDPASS_NOTUSED, "BandpassBlend_NOTUSED");
	PARAM_NULL(LFOPW2_UNUSED, "LfoPw2_NOTUSED");
	PARAM_NULL(OSC2Saw_NOTUSED, "Osc2Saw_NOTUSED");
	PARAM_NULL(OSC1Saw_NOTUSED, "Osc1Saw_NOTUSED");
	PARAM_NULL(OSC1Pul_NOTUSED, "Osc1Pulse_NOTUSED");
	PARAM_NULL(OSC2Pul_NOTUSED, "Osc2Pulse_NOTUSED");

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
	//setFuncType setFunc = paramdef[index].setFunc;
	//(synth.*setFunc)(newValue);
	if (index >= 0 && index < PARAM_COUNT && paramdef[index].setFunc)
		(synth.*paramdef[index].setFunc)(newValue);
	//DIRTY HACK
	//This should be checked to avoid stalling on gui update
	//It is needed because some hosts do  wierd stuff
	if(isHostAutomatedChange)
		sendChangeMessage();
}

const String MimidAudioProcessor::getParameterName (int index)
{
	if (index >= 0 && index < PARAM_COUNT)
		return paramdef[index].name;
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
