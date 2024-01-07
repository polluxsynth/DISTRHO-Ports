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
	float pitch1;
	float pitch2;
	float sampleRateInv;


	float x1,x2;

	float osc1Factor;
	float osc2Factor;

	float pw1w,pw2w;
	//blep const
	const int n;
	const int hsam;
	//delay line implements fixed sample delay
	DelayLine<Samples> xmodd;
	DelayLineBoolean<Samples> syncd;
	DelayLine<Samples> syncFracd;
	DelayLine<Samples> cvd;
	Random wn;
	SawOsc o1s,o2s;
	PulseOsc o1p,o2p;
	TriangleOsc o1t,o2t;
public:

	float tune;//+-1 
	int oct;

	float dirt;

	float notePlaying;


	float totalSpread;

	float osc1Det;
	float osc1pw,osc2pw;
	float pw1,pw2;


	bool quantizeCw;

	float o1mx,o2mx;
	float nmx;
	float pto1,pto2;

	//osc waveshapes
	float osc1Saw,osc2Saw, osc1Pul,osc2Pul, osc1Tri, osc2Tri;

	float osc1p,osc2p;
	bool hardSync;
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
		pw1w=pw2w=0;
		pto1=pto2=0;
		pw1=pw2=0;
		xmod = 0;
		hardSync = false;
		osc1p=osc2p=10;
		osc1Saw=osc2Saw=osc1Pul=osc2Pul=false;
		osc1Det = 0;
		notePlaying = 30;
		osc1pw = osc2pw = 0;
		o1mx=o2mx=0;
		x1=wn.nextFloat();
		x2=wn.nextFloat();
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
	}
	void removeDecimation()
	{
		o1p.removeDecimation();
		o1t.removeDecimation();
		o1s.removeDecimation();
		o2p.removeDecimation();
		o2t.removeDecimation();
		o2s.removeDecimation();
	}
	void setSampleRate(float sr)
	{
		SampleRate = sr;
		sampleRateInv = 1.0f / SampleRate;
	}
	inline void ProcessSample(float &audioOutput, float &modOutput)
	{
		float noiseGen = wn.nextFloat()-0.5;
		pitch2 = getPitch(dirt * noiseGen + notePlaying + (quantizeCw?((int)(osc2p)):osc2p)+ pto2 + tune + oct+totalSpread*osc2Factor);
		bool hsr = false;
		float hsfrac=0;
		float fs = jmin(pitch2*(sampleRateInv),0.45f);
		x2+=fs;
		hsfrac = 0;
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

		hsr &= hardSync;
		//Delaying our hard sync gate signal and frac
		hsr = syncd.feedReturn(hsr) != 0.0f;
		hsfrac = syncFracd.feedReturn(hsfrac);

		if(osc2Pul)
			osc2mix = o2p.getValue(x2,pwcalc) + o2p.aliasReduction();
		else if(osc2Saw)
			osc2mix = o2s.getValue(x2) + o2s.aliasReduction();
		else if(osc2Tri)
			osc2mix = o2t.getValue(x2) + o2t.aliasReduction();

		//Pitch control needs additional delay buffer to compensate
		//This will give us less aliasing on xmod
		//Hard sync gate signal delayed too
		noiseGen = wn.nextFloat()-0.5;
		// Offset on osc2mix * xmod is to get zero pitch shift at
		// max xmod
		pitch1 = getPitch(cvd.feedReturn(dirt *noiseGen + notePlaying + osc1Det + (quantizeCw?((int)(osc1p)):osc1p) + pto1+ (osc2modout?osc2mix-0.0569:0)*xmod + tune + oct +totalSpread*osc1Factor));

		fs = jmin(pitch1 * (sampleRateInv),0.45f);

		pwcalc = jlimit<float>(0.1f,1.0f,(osc1pw + pw1)*0.5f + 0.5f);

		float osc1mix=0.0f;

		x1 +=fs;

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
		//Delaying osc2 signal
		//And getting delayed back
		osc2mix = xmodd.feedReturn(osc2mix);

		if(osc1Pul)
			osc1mix = o1p.getValue(x1,pwcalc) + o1p.aliasReduction();
		else if(osc1Saw)
			osc1mix = o1s.getValue(x1) + o1s.aliasReduction();
		else if(osc1Tri)
			osc1mix = o1t.getValue(x1) + o1t.aliasReduction();

		//mixing
		float res =o1mx*osc1mix + o2mx*osc2mix + (noiseGen)*(nmx*1.3 + 0.0006);
		audioOutput = res*3;
		modOutput = osc2mix;
	}
};
