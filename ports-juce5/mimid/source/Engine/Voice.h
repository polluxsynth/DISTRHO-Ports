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
#include "Lfo.h"
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
	AdssrEnvelope genv;
	Lfo lfo1;
	Lfo lfo2;
	OscillatorB osc;
	Filter flt;

	Random ng;

	float vamp,vflt,vgen;

	float cutoff;
	float fenvamt;

	float EnvSpread;
	float FenvSpread;
	float GenvSpread;

	float Lfo1Spread;
	float Lfo2Spread;

	float FltSpread;
	float FltSpreadAmt;

	float PortaSpread;
	float PortaSpreadAmt;

	float levelSpread;
	float levelSpreadAmt;

	float brightCoef;
	float osc1FltMod;

	int midiIndx;

	bool Active;
	bool shouldProcessed;

	float fltKF;

	float porta;
	float prtst;

	float modWheelSmoothed;

	float cutoffwas,envelopewas;

	float pitchWheel;
	float pitchWheelAmt;
	bool pitchWheelOsc2Only;

	float lfo1a,lfo2a;
	bool lfo1o1,lfo1o2,lfo1f;
	bool lfo1pw1,lfo1pw2;
	bool lfo2o1,lfo2o2,lfo2f;
	bool lfo2pw1,lfo2pw2;

	bool selfOscPush;

	float genvamt;
	bool genvo1, genvo2, genvpw1, genvpw2, genvres;
	bool invertFenv;
	bool unipolGenv;
	bool invertGenv;


	bool fourpole;


	DelayLine<Samples*2> lenvd,fenvd,lfo1d,lfo2d;

	float oscpsw;
	float briHold;

	bool oscmodEnable; // Oscillator modulation output enabled

	int voiceNumber; // Handy to have in the voice itself

	float unused1, unused2; // TODO: remove

	Voice()
	{
		selfOscPush = false;
		invertFenv = false;
		unipolGenv = false;
		invertGenv = false;
		ng = Random(Random::getSystemRandom().nextInt64());
		sustainHold = false;
		shouldProcessed = false;
		vamp=vflt=vgen=0;
		velocityValue=0;
		fourpole = false;
		brightCoef =briHold= 1;
		osc1FltMod = 0;
		oscpsw = 0;
		cutoffwas = envelopewas=0;
		c1=c2=d1=d2=0;
		pitchWheel=pitchWheelAmt=0;
		PortaSpreadAmt=0;
		FltSpreadAmt=0;
		levelSpreadAmt=0;
		porta =0;
		prtst=0;
		fltKF= false;
		cutoff=0;
		fenvamt = 0;
		Active = false;
		midiIndx = 30;
		levelSpread = Random::getSystemRandom().nextFloat()-0.5;
		EnvSpread = Random::getSystemRandom().nextFloat()-0.5;
		FenvSpread = Random::getSystemRandom().nextFloat()-0.5;
		GenvSpread = Random::getSystemRandom().nextFloat()-0.5;
		Lfo1Spread = Random::getSystemRandom().nextFloat()-0.5;
		Lfo2Spread = Random::getSystemRandom().nextFloat()-0.5;
		FltSpread = Random::getSystemRandom().nextFloat()-0.5;
		PortaSpread =Random::getSystemRandom().nextFloat()-0.5;
	//	lenvd=new DelayLine(Samples*2);
	//	fenvd=new DelayLine(Samples*2);
		oscmodEnable = false;
		lfo2.waveForm = 1; // Triangle
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
		float lfo1In, lfo2In;

		lfo1In = lfo1.getVal() * lfo1a;
		lfo2In = lfo2.getVal() * lfo2a * modWheelSmoothed;

		//portamento on osc input voltage
		//implements rc circuit
		// Midi note 81 is A5 (880 Hz), so ptNote == 0 => 880 Hz
		float ptNote  =tptlpupw(prtst, midiIndx-81, porta * (1+PortaSpread*PortaSpreadAmt),sampleRateInv);
		osc.notePlaying = ptNote;
		//both envelopes and filter cv need a delay equal to osc internal delay
		float lfo1Delayed = lfo1d.feedReturn(lfo1In);
		float lfo2Delayed = lfo2d.feedReturn(lfo2In);
		//filter envelope undelayed
		float envm = fenv.processSample() * (1 - (1-velocityValue)*vflt);
		if(invertFenv)
			envm = -envm;

		//filter envelope undelayed
		float genvm = genv.processSample() * (1 - (1-velocityValue)*vgen) * genvamt;
		if (!unipolGenv) // Bipolar
			genvm = 2 * genvm - 1;
		if (invertGenv)
			genvm = -genvm;

		//PW modulation
		osc.pw1 = (lfo1pw1?lfo1In:0) + (lfo2pw1?lfo1In:0) + (genvpw1?genvm:0);
		osc.pw2 = (lfo1pw2?lfo1In:0) + (lfo2pw2?lfo2In:0) + (genvpw2?genvm:0);

		//Pitch modulation
		osc.pto1 =   (!pitchWheelOsc2Only? (pitchWheel*pitchWheelAmt):0 ) + (lfo1o1?lfo1In:0) + (lfo2o1?lfo2In:0) + (genvo1?(genvm*36):0);
		osc.pto2 =  (pitchWheel*pitchWheelAmt) + (lfo1o2?lfo1In:0) + (lfo2o2?lfo2In:0) + (genvo2?(genvm*36):0);

		//variable sort magic - upsample trick
		float envVal = lenvd.feedReturn(env.processSample() * (1 - (1-velocityValue)*vamp));

		osc.ProcessSample(oscps, oscmod);

		oscps *= 1 - levelSpreadAmt*levelSpread;
		oscps = oscps - tptlpupw(c1,oscps,12,sampleRateInv);

		//filter exp cutoff calculation
		//needs to be done after we've gotten oscmod
		// ptNote+40 => F2 = 87.31 Hz is base note for filter tracking
		float cutoffnote =
			(lfo1f?lfo1Delayed:0)+
			(lfo2f?lfo2Delayed:0)+
			cutoff+
			FltSpread*FltSpreadAmt+
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
	void setEnvSpreadAmt(float d)
	{
		// Derivation: start with 2 ** (rndval * amt), which
		// gives a range of 2**(-0.5) .. 2**(0.5) = 0.707 .. 1.41,
		// which gives us values centered around 1 when applied
		// multiplicatively as is done in the envelope.
		// Replacing 2** with exp() gives us exp(rndval * amt * ln(a)),
		// where a = maxval**2, i.e. exp(rndval * amt * ln(2)).
		// Consequently, we have exp(rndval * amt * ln(maxval ** 2)) =
		// exp(rndval * amt * 2 * ln(maxval)).
		// If we then set maxval to be the more even 1.5, we get
		// the final exp(rndval * amt * 2 * ln(1.5)) .
		// The total range will then be 0.67 .. 1.5 .
		env.setSpread(expf(EnvSpread*d * 2 * logf(1.5)));
		fenv.setSpread(expf(EnvSpread*d * 2 * logf(1.5)));
		genv.setSpread(expf(EnvSpread*d * 2 * logf(1.5)));
	}
	void setLfoSpreadAmt(float d)
	{
		lfo1.setSpread(expf(Lfo1Spread*d * 2 * logf(1.5)));
		lfo2.setSpread(expf(Lfo2Spread*d * 2 * logf(1.5)));
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
		genv.setSampleRate(sr);
		lfo1.setSampleRate(sr);
		lfo2.setSampleRate(sr);
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
		genv.ResetEnvelopeState();
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
			genv.triggerAttack();
			lfo1.keyResetPhase(); // TODO: Always?
			lfo2.keyResetPhase(); // TODO: Always?
		}
		Active = true;
	}
	void NoteOff()
	{
		if(!sustainHold) {
			env.triggerRelease();
			fenv.triggerRelease();
			genv.triggerRelease();
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
			genv.triggerRelease();
		}

	}
};
