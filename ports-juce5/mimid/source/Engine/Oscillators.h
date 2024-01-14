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

#include "Voice.h"
#include "SynthEngine.h"
#include "AudioUtils.h"
#include "BlepData.h"
#include "DelayLine.h"
#include "SawOsc.h"
#include "PulseOsc.h"
#include "TriangleOsc.h"

class Oscillators
{
private:
	float SampleRate;
	float sampleRateInv;


	float x1,x2,x3;

	float osc1Factor;
	float osc2Factor;

	float pw1w,pw2w,pw3w;
	//blep const
	const int n;
	const int hsam;
	//delay line implements fixed sample delay
	DelayLine<Samples> osc2d;
	DelayLine<Samples> osc3d;
	DelayLineBoolean<Samples> syncd;
	DelayLine<Samples> syncFracd;
	DelayLine<Samples> cvd;
	Random wn;
	SawOsc o1s,o2s,o3s;
	PulseOsc o1p,o2p,o3p;
	TriangleOsc o1t,o2t,o3t;
public:

	float tune;//+-1 
	int oct;

	float dirt;

	float notePlaying;


	float totalSpread;

	float osc1Det,osc2Det,osc3Det;
	float osc3Ofs, osc3Ratio;
	float osc1pw,osc2pw,osc3pw;
	float pw1,pw2;

	float o1mx,o2mx,o3mx;
	float nmx;
	float pto1,pto2;

	//osc waveshapes
	bool osc1Saw, osc1Pul, osc1Tri;
	bool osc2Saw, osc2Pul, osc2Tri;
	bool osc3Saw, osc3Pul, osc3Tri;

	float osc1p,osc2p;
	float syncLevel;
	float xmod;
	bool osc2modout;

	float unused1, unused2; //TODO remove

	Oscillators() :
		n(Samples*2),
		hsam(Samples),
		o1s(),o2s(),
		o1p(),o2p(),
		o1t(),o2t()
	{
		dirt = 0.1;
		totalSpread = 0;
		wn = Random(Random::getSystemRandom().nextInt64());
		osc1Factor = wn.nextFloat()-0.5;
		osc2Factor = wn.nextFloat()-0.5;
		nmx=0;
		oct=0;
		tune=0;
		pw1w=pw2w=pw3w=0;
		pto1=pto2=0;
		pw1=pw2=0;
		xmod = 0;
		syncLevel = 1;
		osc1p=osc2p=24;
		osc1Saw=osc2Saw=osc1Pul=osc2Pul=false;
		osc1Det = osc2Det = 0;
		osc3Det = 1.0; // multiplicative
		osc3Ofs = 0;
		osc3Ratio = 1.0;
		notePlaying = 30;
		osc1pw = osc2pw = osc3pw = 0;
		o1mx=o2mx=0;
		x1=wn.nextFloat();
		x3=x2=wn.nextFloat(); // osc2 and 3 start in phase
	}
	~Oscillators()
	{
	}
	void setDecimation()
	{
		o1p.setDecimation();
		o1t.setDecimation();
		o1s.setDecimation();
		o2p.setDecimation();
		o2t.setDecimation();
		o2s.setDecimation();
		o3p.setDecimation();
		o3t.setDecimation();
		o3s.setDecimation();
	}
	void removeDecimation()
	{
		o1p.removeDecimation();
		o1t.removeDecimation();
		o1s.removeDecimation();
		o2p.removeDecimation();
		o2t.removeDecimation();
		o2s.removeDecimation();
		o3p.removeDecimation();
		o3t.removeDecimation();
		o3s.removeDecimation();
	}
	void setSampleRate(float sr)
	{
		SampleRate = sr;
		sampleRateInv = 1.0f / SampleRate;
	}
	inline void ProcessSample(float &audioOutput, float &modOutput)
	{
		// osc 2 = master oscillator
		float noiseGen = wn.nextFloat()-0.5;
		float pitch2 = getPitch(dirt * noiseGen + notePlaying + osc2Det + osc2p + pto2 + tune + oct+totalSpread*osc2Factor);
		bool hsr = false;
		float hsfrac=0;
		float fs = jmin(pitch2*(sampleRateInv),0.45f);
		x2+=fs;
		float osc2mix=0.0f;
		float pwcalc =jlimit<float>(0.1f,1.0f,(osc2pw + pw2)*0.5f + 0.5f);
		if(osc2Pul)
			o2p.processMaster(x2,fs,pwcalc,pw2w);
		else if(osc2Saw)
			o2s.processMaster(x2,fs);
		else if(osc2Tri)
			o2t.processMaster(x2,fs);

		if(x2 >= 1.0f)
		{
			x2-=1.0f;
			hsfrac = x2/fs;
			hsr = true;
		}

		pw2w = pwcalc;

		//Delaying our hard sync gate signal and frac
		hsr = syncd.feedReturn(hsr);
		hsfrac = syncFracd.feedReturn(hsfrac);

		if(osc2Pul)
			osc2mix = o2p.getValue(x2,pwcalc) + o2p.aliasReduction();
		else if(osc2Saw)
			osc2mix = o2s.getValue(x2) + o2s.aliasReduction();
		else if(osc2Tri)
			osc2mix = o2t.getValue(x2) + o2t.aliasReduction();

		// osc3 = osc2 sub oscillator
		// TODO: This is not a true sub, since the phase to osc2 is
		// not fixed. Should we have fixed key sync on osc3 to
		// mitigate this, or implement osc3 as a true sub oscillator
		// dividing down from osc2 (which precludes and Hz offset)
		// Fixed ratio to osc2, plus an offset in Hz.
		// TODO: Do we need both detune and offset?
		float pitch3 = pitch2 * osc3Ratio * osc3Det + osc3Ofs;

		fs = jmin(pitch3 * (sampleRateInv),0.45f);
		x3+=fs;

		pwcalc = jlimit<float>(0.1f,1.0f,osc3pw*0.5f + 0.5f);

		float osc3mix=0.0f;

		if(osc3Pul)
			o3p.processMaster(x3,fs,pwcalc,pw3w);
		else if(osc3Saw)
			o3s.processMaster(x3,fs);
		else if(osc3Tri)
			o3t.processMaster(x3,fs);

		if(x3 >= 1.0f)
			x3-=1.0f;

		pw3w = pwcalc;

		if(osc3Pul)
			osc3mix = o3p.getValue(x3,pwcalc) + o3p.aliasReduction();
		else if(osc3Saw)
			osc3mix = o3s.getValue(x3) + o3s.aliasReduction();
		else if(osc3Tri)
			osc3mix = o3t.getValue(x3) + o3t.aliasReduction();

		// osc1 = slave oscillator

		//Pitch control needs additional delay buffer to compensate
		//This will give us less aliasing on xmod
		//Hard sync gate signal delayed too
		noiseGen = wn.nextFloat()-0.5;
		// Offset on osc2mix * xmod is to get zero pitch shift at
		// max xmod
		float pitch1 = getPitch(cvd.feedReturn(dirt *noiseGen + notePlaying + osc1Det + osc1p + pto1 + (osc2modout?osc2mix-0.0569:0)*xmod + tune + oct +totalSpread*osc1Factor));

		fs = jmin(pitch1 * (sampleRateInv),0.45f);

		pwcalc = jlimit<float>(0.1f,1.0f,(osc1pw + pw1)*0.5f + 0.5f);

		float osc1mix=0.0f;

		x1 +=fs;

		// Sync level: we check if x1 is above the sync level when
		// the reset from the master oscillator occurs.
		// If the sync level is high enough, disable completely,
		// to avoid artefacts when osc2 is running at an insanely
		// high frequency (empirically).
		hsr &= (syncLevel <= 0.99) && (x1 - hsfrac*fs >= syncLevel);

		if(osc1Pul)
			o1p.processSlave(x1,fs,hsr,hsfrac,pwcalc,pw1w);
		else if(osc1Saw)
			o1s.processSlave(x1,fs,hsr,hsfrac);
		else if(osc1Tri)
			o1t.processSlave(x1,fs,hsr,hsfrac);

		if(x1 >= 1.0f)
			x1-=1.0;

		pw1w=pwcalc;
		//On hard sync reset slave phase is affected that way
		if(hsr)
		{
			float fracMaster = (fs * hsfrac);
			x1 =fracMaster;
		}

		// Delay osc2 and osc3 to get in phase with osc1 which is
		// in itself delayed due to xmod
		osc2mix = osc2d.feedReturn(osc2mix);
		osc3mix = osc3d.feedReturn(osc3mix);

		if(osc1Pul)
			osc1mix = o1p.getValue(x1,pwcalc) + o1p.aliasReduction();
		else if(osc1Saw)
			osc1mix = o1s.getValue(x1) + o1s.aliasReduction();
		else if(osc1Tri)
			osc1mix = o1t.getValue(x1) + o1t.aliasReduction();

		//mixing
		float res =o1mx*osc1mix + o2mx*osc2mix + o3mx*osc3mix + (noiseGen)*(nmx*1.3 + 0.0006);
		audioOutput = res*3;
		modOutput = osc2mix;
	}
};
