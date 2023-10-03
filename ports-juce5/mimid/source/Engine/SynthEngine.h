/*
	==============================================================================
	This file is part of the MiMi-d synthesizer,
	originally from Obxd synthesizer.

	Copyright © 2013-2014 Filatov Vadim
	Copyright 2023 Ricard Wanderlof

	Contact original author via email :
	justdat_@_e1.ru

	This file may be licensed under the terms of of the
	GNU General Public License Version 2 (the ``GPL'').

	Software distributed under the License is distributed
	on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
	express or implied. See the GPL for the specific language
	governing rights and limitations.

	You should have received a copy of the GPL along with this
	program. If not, go to http://www.gnu.org/licenses/gpl.html
	or write to the Free Software Foundation, Inc.,  
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
	==============================================================================
 */
#pragma once

#include "../PluginProcessor.h"
#include "Voice.h"
#include "Motherboard.h"
#include "Params.h"
#include "ParamSmoother.h"

class SynthEngine
{
private:
	Motherboard synth;
	ParamSmoother cutoffSmoother;
	ParamSmoother pitchWheelSmoother;
	ParamSmoother modWheelSmoother;
	float sampleRate;
	// TODO Remove unused1,2:
	float unused1, unused2;
	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEngine)
public:
	SynthEngine():
		cutoffSmoother(),
		//synth = new Motherboard();
		pitchWheelSmoother(),
		modWheelSmoother()
	{
	}
	~SynthEngine()
	{
		//delete synth;
	}
	void setPlayHead(float bpm,float retrPos)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1.hostSyncRetrigger(bpm,retrPos);
			synth.voices[i].lfo2.hostSyncRetrigger(bpm,retrPos);
		}
	}
	void setSampleRate(float sr)
	{
		sampleRate = sr;
		cutoffSmoother.setSampleRate(sr);
		pitchWheelSmoother.setSampleRate(sr);
		modWheelSmoother.setSampleRate(sr);
		synth.setSampleRate(sr);
	}
	void processSample(float *left,float *right)
	{
		processCutoffSmoothed(cutoffSmoother.smoothStep());
		procPitchWheelSmoothed(pitchWheelSmoother.smoothStep());
		procModWheelSmoothed(modWheelSmoother.smoothStep());

		synth.processSample(left,right);
	}
	void allNotesOff()
	{
		for(int i = 0 ;  i < 128;i++)
			{
				procNoteOff(i);
			}
	}
	void allSoundOff()
	{
		allNotesOff();
		for(int i = 0 ; i < Motherboard::MAX_VOICES;i++)
			{
				synth.voices[i].ResetEnvelope();
			}
	}
	void sustainOn()
	{
		synth.sustainOn();
	}
	void sustainOff()
	{
		synth.sustainOff();
	}
	void procLfo1Sync(float val)
	{
		for(int i = 0 ; i < Motherboard::MAX_VOICES;i++)
		{
			synth.voices[i].lfo1.setClockSync(val > 0.5);
		}
	}
	void procKeyAsgnRsz(float val)
	{
		synth.voiceAlloc.rsz = val > 0.5;
	}
	void procKeyAsgnMem(float val)
	{
		synth.voiceAlloc.mem = val > 0.5;
	}
	void procKeyAsgnRob(float val)
	{
		int intval = roundToInt(val*2);
		synth.voiceAlloc.rob_oldest = intval == 1;
		synth.voiceAlloc.rob_next_to_lowest = intval == 2;
	}
	void procKeyAsgnRes(float val)
	{
		synth.voiceAlloc.restore = val > 0.5;
	}
	void procKeyAsgnStrg(float val)
	{
		int intval = roundToInt(val*2);
		synth.voiceAlloc.strgNoteOff = intval == 1 || intval == 2;
		synth.voiceAlloc.strgNoteOn = intval == 2;
	}
	void processUnison(float param)
	{
		synth.voiceAlloc.uni = param>0.5f;
	}
	void procNoteOn(int noteNo,float velocity)
	{
		synth.voiceAlloc.setNoteOn(noteNo,velocity);
	}
	void procNoteOff(int noteNo)
	{
		synth.voiceAlloc.setNoteOff(noteNo);
	}
	void procEconomyMode(float val)
	{
		synth.economyMode = val>0.5;
	}
#define ForEachVoice(expr) \
	for(int i = 0 ; i < synth.MAX_VOICES;i++) \
		{\
			synth.voices[i].expr;\
		}\

	// TODO: Remove
	void procUnused1(float val)
	{
		unused1=val;
		ForEachVoice(unused1=val);
		ForEachVoice(osc.unused1=val);
		ForEachVoice(flt.unused1=val);
		ForEachVoice(env.unused1=val);
		ForEachVoice(fenv.unused1=val);
	}
	// TODO: Remove
	void procUnused2(float val)
	{
		unused2=val;
		ForEachVoice(unused2=val);
		ForEachVoice(osc.unused2=val);
		ForEachVoice(flt.unused2=val);
		ForEachVoice(env.unused2=val);
		ForEachVoice(fenv.unused2=val);
	}
	void procAmpVelocityAmount(float val)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].vamp= val;
		}
	}
	void procFltVelocityAmount(float val)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].vflt= val;
		}
	}
	void procModWheel(float val)
	{
		modWheelSmoother.setSteep(val);
	}
	void procModWheelSmoothed(float val)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].modWheelSmoothed = val;
		}
	}
	void procModWheelFrequency(float val)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo2.setRawFrequency(val);
			synth.voices[i].lfo2.setFrequency(logsc(val,0,50,120));
		}
	}
	void procPitchWheel(float val)
	{
		pitchWheelSmoother.setSteep(val);
		//for(int i = 0 ; i < synth->MAX_VOICES;i++)
		//{
		//	synth->voices[i]->pitchWheel = val;
		//}
	}
	inline void procPitchWheelSmoothed(float val)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].pitchWheel = val;
		}
	}
	void setVoiceCount(float param)
	{
		synth.setVoiceCount(roundToInt((param*7) +1));
	}
	void procPitchWheelAmount(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].pitchWheelAmt = param>0.5?12:2;
		}
	}
	void procPitchWheelOsc2Only(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].pitchWheelOsc2Only = param>0.5;
		}
	}
	void processPan(float param,int idx)
	{
		synth.pannings[idx-1] = param;
	}
	void processTune(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.tune = param*2-1;
		}
	}
	void processOctave(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.oct = (roundToInt(param*4) -2)*12;
		}
	}
	void processFilterKeyFollow(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fltKF = param;
		}
	}
	void processPortamento(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].porta =logsc(1-param,0.14,250,150);
		}
	}
	void processVolume(float param)
	{
		synth.Volume = linsc(param,0,0.30);
	}
	void processLfoFrequency(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1.setRawFrequency(param);
			synth.voices[i].lfo1.setFrequency(logsc(param,0,50,120));
		}
	}
	void processLfo1Wave(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1.waveForm =  roundToInt(param*5);
		}
	}
	void processLfo2Wave(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo2.waveForm =  roundToInt(param*5);
		}
	}
	void processLfo1Amt(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1a = logsc(logsc(param,0,1,60),0,60,10);
		}
	}
	void processLfo1Dest(float param)
	{
		int intparam = roundToInt(param*7);
		// off, osc1, osc1+2, osc2, pw1, pw1+2, pw2, filt
		// 0    1     2       3     4    5      6    7
		bool lfo1o1 = intparam == 1 || intparam == 2;
		bool lfo1o2 = intparam == 2 || intparam == 3;
		bool lfo1pw1 = intparam == 4 || intparam == 5;
		bool lfo1pw2 = intparam == 5 || intparam == 6;
		bool lfo1filt = intparam == 7;
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1o1 = lfo1o1;
			synth.voices[i].lfo1o2 = lfo1o2;
			synth.voices[i].lfo1pw1 = lfo1pw1;
			synth.voices[i].lfo1pw2 = lfo1pw2;
			synth.voices[i].lfo1f = lfo1filt;
		}
	}
	void processLfo2Dest(float param)
	{
		int intparam = roundToInt(param*7);
		// off, osc1, osc1+2, osc2, pw1, pw1+2, pw2, filt
		// 0    1     2       3     4    5      6    7
		bool lfo2o1 = intparam == 1 || intparam == 2;
		bool lfo2o2 = intparam == 2 || intparam == 3;
		bool lfo2pw1 = intparam == 4 || intparam == 5;
		bool lfo2pw2 = intparam == 5 || intparam == 6;
		bool lfo2filt = intparam == 7;
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo2o1 = lfo2o1;
			synth.voices[i].lfo2o2 = lfo2o2;
			synth.voices[i].lfo2pw1 = lfo2pw1;
			synth.voices[i].lfo2pw2 = lfo2pw2;
			synth.voices[i].lfo2f = lfo2filt;
		}
	}
	void processLfoFilter_unused(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1f = param>0.5;
		}
	}
	void processLfoPw1_unused(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1pw1 = param>0.5;
		}
	}
	void processLfoPw2_unused(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo1pw2 = param>0.5;
		}
	}
	void processLfo2Amt(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].lfo2a = logsc(logsc(param,0,1,60),0,60,10);
		}
	}
	void processDetune(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.totalDetune = logsc(param,0.001,0.90);
		}
	}
	void processOsc1PulseWidth(float param)
	{
		ForEachVoice (osc.osc1pw=linsc(param,0.0,0.95));
	}
	void processPwEnv(float param)
	{
		ForEachVoice (pwenvmod=linsc(param,0,0.85));
	}
	void processOsc2PulseWidth(float param)
	{
		ForEachVoice(osc.osc2pw = linsc(param,0.0,0.95));
	}
	void processPwEnvBoth(float param)
	{
		ForEachVoice(pwEnvBoth = param>0.5);
	}
	void processInvertFenv(float param)
	{
		ForEachVoice(invertFenv = param>0.5);
	}
	void processPitchModBoth(float param)
	{
		ForEachVoice(pitchModBoth = param>0.5);
	}
	void processOsc2Xmod(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.xmod= param*24;
		}
	}
	void processEnvelopeToPitch(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].envpitchmod= param*36;
		}
	}
	void processOsc2HardSync(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.hardSync = param>0.5;
		}
	}
	void processOsc1Pitch(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.osc1p = (param * 48);
		}
	}
	void processOsc2Pitch(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.osc2p = (param * 48);
		}
	}
	void processPitchQuantization(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.quantizeCw = param>0.5;
		}
	}
	void processOsc1Mix(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.o1mx = param;
		}
	}
	void processOsc2Mix(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.o2mx = param;
		}
	}
	void processNoiseMix(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.nmx = logsc(param,0,1,35);
		}
	}
	void processBrightness(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].setBrightness(  linsc(param,7000,26000));
		}
	}
	void processOsc1FltMod(float param)
	{
		ForEachVoice(osc1FltMod = param*100);
	}
	void processOsc2Det(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.osc2Det = logsc(param,0.001,0.6);
		}
	}

	void processOsc1Wave(float param)
	{
		int intparam = roundToInt(param*3);
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.osc1Saw = intparam == 1;
			synth.voices[i].osc.osc1Pul = intparam == 2;
			synth.voices[i].osc.osc1Tri = intparam == 3;
			synth.voices[i].osc.osc1modout =
			synth.voices[i].oscmodEnable = intparam != 0;
		}
	}

	void processOsc2Wave(float param)
	{
		int intparam = roundToInt(param*3);
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].osc.osc2Saw = intparam == 1;
			synth.voices[i].osc.osc2Pul = intparam == 2;
			synth.voices[i].osc.osc2Tri = intparam == 3;
		}
	}

	void processCutoff(float param)
	{
		cutoffSmoother.setSteep( linsc(param,0,120));
	//	for(int i = 0 ; i < synth->MAX_VOICES;i++)
	//	{
			//synth->voices[i]->cutoff = logsc(param,60,19000,30);
		//	synth->voices[i]->cutoff = linsc(param,0,120);
	//	}
	}
	inline void processCutoffSmoothed(float param)
	{
		ForEachVoice(cutoff=param);
	}
	void processFilterType(float param)
	{
		int intparam = roundToInt(param*3);
		// 0 => fourpole, response sets slope 6..24 dB/octave
		// 1 => SVF: response sets LP - BP - HP
		// 2 => SVF w/ self osc: response sets LP -BP -HP
		// 3 => SVF: response sets LP - notch - HP
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fourpole = intparam == 0;
			synth.voices[i].flt.bandPassSw = intparam == 1 || intparam == 2;
			synth.voices[i].selfOscPush = intparam == 2;
			synth.voices[i].flt.selfOscPush = intparam == 2;
		}
	}
	void processResonance(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].flt.setResonance(0.991-logsc(1-param,0,0.991,40));
		}
	}
	void processResponse(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			//synth.voices[i].flt ;
			synth.voices[i].flt.setResponse(linsc(param,0,1));
		}
	}
	void processOversampling(float param)
	{
		synth.SetOversample(param>0.5);
	}
	void processFilterEnvelopeAmt(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenvamt = linsc(param,0,140);
		}
	}
	void processLoudnessEnvelopeAttack(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].env.setAttack(logsc(param,4,60000,900));
		}
	}
	void processLoudnessEnvelopeDecay(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].env.setDecay(logsc(param,4,60000,900));
		}
	}
	void processLoudnessEnvelopeSustainTime(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].env.setSustainTime(logsc(param,4,60000,900));
			// When time is set to 1.0, sustain time is infinite
			synth.voices[i].env.setAdsr(param > 0.991);
		}
	}
	void processLoudnessEnvelopeRelease(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].env.setRelease(logsc(param,8,60000,900));
		}
	}
	void processLoudnessEnvelopeSustain(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].env.setSustain(param);
		}
	}
	void processFilterEnvelopeAttack(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenv.setAttack(logsc(param,1,60000,900));
		}
	}
	void processFilterEnvelopeDecay(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenv.setDecay(logsc(param,1,60000,900));
		}
	}
	void processFilterEnvelopeSustainTime(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenv.setSustainTime(logsc(param,1,60000,900));
			// When time is set to 1.0, sustain time is infinite
			synth.voices[i].fenv.setAdsr(param > 0.991);
		}
	}
	void processFilterEnvelopeRelease(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenv.setRelease(logsc(param,1,60000,900));
		}
	}
	void processFilterEnvelopeSustain(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].fenv.setSustain(param);
		}
	}
	void processEnvelopeDetune(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].setEnvDer(linsc(param,0.0,1));
		}
	}
	void processFilterDetune(float param)
	{
for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].FltDetAmt = linsc(param,0.0,18);
		}
	}
	void processPortamentoDetune(float param)
	{
		for(int i = 0 ; i < synth.MAX_VOICES;i++)
		{
			synth.voices[i].PortaDetuneAmt = linsc(param,0.0,0.75);
		}
	}
	void processLoudnessDetune(float param)
	{
		ForEachVoice(levelDetuneAmt = linsc(param,0.0,0.67));
	}

		 
};
