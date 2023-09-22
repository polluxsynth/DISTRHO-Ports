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
#include "OscillatorB.h"
#include "AdssrEnvelope.h"
#include "Filter.h"
#include "Decimator.h"

class Voice
{
private:
	float SampleRate;
	float sampleRateInv;
	float Volume;
	float port;
	float velocityValue;

	float d1,d2;
	float c1,c2;

	// offset to get apparent zero cutoff frequency shift with oscmod
	const float oscmod_offset = 0.20;
	// maximum peak of oscmod waveform w/ offset applied
	const float oscmod_maxpeak = 0.5 - oscmod_offset;
	const float oscmod_maxpeak_inv = oscmod_maxpeak ? 1/oscmod_maxpeak:0;

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Voice)
public:
	bool sustainHold;
	//bool resetAdssrsOnAttack;

	AdssrEnvelope env;
	AdssrEnvelope fenv;
	OscillatorB osc;
	Filter flt;

	Random ng;

	float vamp,vflt;

	float cutoff;
	float fenvamt;

	float EnvDetune;
	float FenvDetune;

	float FltDetune;
	float FltDetAmt;

	float PortaDetune;
	float PortaDetuneAmt;

	float levelDetune;
	float levelDetuneAmt;

	float brightCoef;
	float osc1FltMod;

	int midiIndx;

	bool Active;
	bool shouldProcessed;

	float fltKF;

	float porta;
	float prtst;

	float cutoffwas,envelopewas;

	float lfoIn;
	float lfoVibratoIn;

	float pitchWheel;
	float pitchWheelAmt;
	bool pitchWheelOsc2Only;

	float lfoa1,lfoa2;
	bool lfoo1,lfoo2,lfof;
	bool lfopw1,lfopw2;

	bool selfOscPush;

	float envpitchmod;
	float pwenvmod;

	bool pwEnvBoth;
	bool pitchModBoth;

	bool invertFenv;


	bool fourpole;


	DelayLine<Samples*2> lenvd,fenvd,lfod;

	float oscpsw;
	float briHold;

	bool oscmodEnable; // Oscillator modulation output enabled

	int voiceNumber; // Handy to have in the voice itself

	float unused1, unused2; // TODO: remove

	Voice()
	{
		selfOscPush = false;
		pitchModBoth = false;
		invertFenv = false;
		pwEnvBoth = false;
		ng = Random(Random::getSystemRandom().nextInt64());
		sustainHold = false;
		shouldProcessed = false;
		vamp=vflt=0;
		velocityValue=0;
		lfoVibratoIn=0;
		fourpole = false;
		brightCoef =briHold= 1;
		osc1FltMod = 0;
		envpitchmod = 0;
		pwenvmod = 0;
		oscpsw = 0;
		cutoffwas = envelopewas=0;
		c1=c2=d1=d2=0;
		pitchWheel=pitchWheelAmt=0;
		lfoIn=0;
		PortaDetuneAmt=0;
		FltDetAmt=0;
		levelDetuneAmt=0;
		porta =0;
		prtst=0;
		fltKF= false;
		cutoff=0;
		fenvamt = 0;
		Active = false;
		midiIndx = 30;
		levelDetune = Random::getSystemRandom().nextFloat()-0.5;
		EnvDetune = Random::getSystemRandom().nextFloat()-0.5;
		FenvDetune = Random::getSystemRandom().nextFloat()-0.5;
		FltDetune = Random::getSystemRandom().nextFloat()-0.5;
		PortaDetune =Random::getSystemRandom().nextFloat()-0.5;
	//	lenvd=new DelayLine(Samples*2);
	//	fenvd=new DelayLine(Samples*2);
		oscmodEnable = false;
		voiceNumber = 0; // Until someone else says something else
		unused1=unused2=0; // TODO: Remove
	}
	~Voice()
	{
	//	delete lenvd;
	//	delete fenvd;
	}
	inline float ProcessSample()
	{
		float oscps, oscmod;

		//portamento on osc input voltage
		//implements rc circuit
		// Midi note 81 is A5 (880 Hz), so ptNote == 0 => 880 Hz
		float ptNote  =tptlpupw(prtst, midiIndx-81, porta * (1+PortaDetune*PortaDetuneAmt),sampleRateInv);
		osc.notePlaying = ptNote;
		//both envelopes and filter cv need a delay equal to osc internal delay
		float lfoDelayed = lfod.feedReturn(lfoIn);
		//filter envelope undelayed
		float envm = fenv.processSample() * (1 - (1-velocityValue)*vflt);
		if(invertFenv)
			envm = -envm;

		//PW modulation
		osc.pw1 = (lfopw1?(lfoIn * lfoa2):0) + (pwEnvBoth?(pwenvmod * envm) : 0);
		osc.pw2 = (lfopw2?(lfoIn * lfoa2):0) + pwenvmod * envm;

		//Pitch modulation
		osc.pto1 =   (!pitchWheelOsc2Only? (pitchWheel*pitchWheelAmt):0 ) + ( lfoo1?(lfoIn * lfoa1):0) + (pitchModBoth?(envpitchmod * envm):0) + lfoVibratoIn;
		osc.pto2 =  (pitchWheel *pitchWheelAmt) + (lfoo2?lfoIn*lfoa1:0) + (envpitchmod * envm) + lfoVibratoIn;



		//variable sort magic - upsample trick
		float envVal = lenvd.feedReturn(env.processSample() * (1 - (1-velocityValue)*vamp));

		osc.ProcessSample(oscps, oscmod);

		oscps *= 1 - levelDetuneAmt*levelDetune;
		oscps = oscps - tptlpupw(c1,oscps,12,sampleRateInv);

		//filter exp cutoff calculation
		//needs to be done after we've gotten oscmod
		// ptNote+40 => F2 = 87.31 Hz is base note for filter tracking
		float cutoffnote =
			(lfof?lfoDelayed*lfoa1:0)+
			cutoff+
			FltDetune*FltDetAmt+
			fenvamt*fenvd.feedReturn(envm)+
			-45 + (fltKF*(ptNote+40));
		if (oscmodEnable) {
			// Alias limiting for oscillator filter modulation:
			// When the positive peak of the mod signal would cause
			// the filter frequency to go above ~22 kHz, scale down
			// the mod amount accordingly.
			// This gives a seamless transition from the mod value
			// set by osc1FltMod down to zero as the filter
			// frequency is increased. The effectiveness of this
			// mod routing is rather diminished at high filter
			// frequency settings anyway.
			// Note that we only limit the modulation amount, the
			// filter frequency without modulation is not limited
			// at this time.
			static const float maxfltfreq = 22000;
			static const float maxallowednote = getNote(maxfltfreq);
			// maxcutoff = cutoff freq at +ve peak of mod wave
			// (without considering oscmod_offset, which gives
			// us a bit of extra margin, as the final offset is
			// in fact negative).
			float maxcutoff = cutoffnote + oscmod_maxpeak * osc1FltMod;
			float osc1FltModTmp = osc1FltMod;
			if (cutoffnote > maxallowednote)
				// outside range; disable modulation
				osc1FltModTmp = 0;
			else if (maxcutoff > maxallowednote)
				// limit osc1FltMod to keep under max allowed.
				// note: divide by peak of mod signal.
				osc1FltModTmp = (maxallowednote - cutoffnote) *
						oscmod_maxpeak_inv;
			cutoffnote += (oscmod-oscmod_offset) * osc1FltModTmp;
		}
		float cutoffcalc = jmin(
			getPitch(cutoffnote)
			//noisy filter cutoff
			+(ng.nextFloat()-0.5f)*3.5f
			, (flt.SampleRate*0.5f-120.0f));//for numerical stability purposes
		//limit our max cutoff on self osc to prevent alising
		//TODO: higher limit when oversample enabled?
		if(selfOscPush)
			cutoffcalc = jmin(cutoffcalc,19000.0f);

		float x1 = oscps;
		x1 = tptpc(d2,x1,brightCoef);
		//TODO: filter oscmod as well to reduce aliasing?
		if(fourpole)
			x1 = flt.Apply4Pole(x1,(cutoffcalc)); 
		else
			x1 = flt.Apply(x1,(cutoffcalc)); 
		x1 *= (envVal);
		return x1;
	}
	void setBrightness(float val)
	{
		briHold = val;
		brightCoef = tan(jmin(val,flt.SampleRate*0.5f-10)* (juce::float_Pi)*flt.sampleRateInv);

	}
	void setEnvDer(float d)
	{
		env.setUniqueDeriviance(1 + EnvDetune*d);
		fenv.setUniqueDeriviance(1 + FenvDetune*d);
	}
	void setHQ(bool hq)
	{
		if(hq)
		{
			osc.setDecimation();
		}
		else
		{
			osc.removeDecimation();
		}
	}
	void setSampleRate(float sr)
	{
		flt.setSampleRate(sr);
		osc.setSampleRate(sr);
		env.setSampleRate(sr);
		fenv.setSampleRate(sr);
		SampleRate = sr;
		sampleRateInv = 1 / sr;
		brightCoef = tan(jmin(briHold,flt.SampleRate*0.5f-10)* (juce::float_Pi) * flt.sampleRateInv);
	}
	void checkAdssrState()
	{
		shouldProcessed = env.isActive();
	}
	void ResetEnvelope()
	{
		env.ResetEnvelopeState();
		fenv.ResetEnvelopeState();
	}
	void NoteOn(int mididx,float velocity,bool multiTrig)
	{
		if(!shouldProcessed)
		{
			//When your processing is paused we need to clear delay lines and envelopes
			//Not doing this will cause clicks or glitches
			lenvd.fillZeroes();
			fenvd.fillZeroes();
			ResetEnvelope();
		}
		shouldProcessed = true;
		if(velocity!=-0.5)
			velocityValue = velocity;
		midiIndx = mididx;
		if(!Active || multiTrig) {
			env.triggerAttack();
			fenv.triggerAttack();
		}
		Active = true;
	}
	void NoteOff()
	{
		if(!sustainHold)
		{
		env.triggerRelease();
		fenv.triggerRelease();
		}
		Active = false;
	}
	void sustOn()
	{
		sustainHold = true;
	}
	void sustOff()
	{
		sustainHold = false;
		if(!Active)
		{
			env.triggerRelease();
			fenv.triggerRelease();
		}

	}
};
