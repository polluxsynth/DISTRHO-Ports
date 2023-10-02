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
#include <climits>
#include "VoiceAllocator.h"
#include "SynthEngine.h"
#include "Lfo.h"

class Motherboard
{
public:
	const static int MAX_VOICES=8;
private:
	Voice *voiceList[MAX_VOICES];
	int totalvc;
	bool wasUni;
	bool awaitingkeys[129];
	int priorities[129];

	Decimator17 left,right;
	int asPlayedCounter;
	float lkl,lkr;
	float sampleRate,sampleRateInv;
	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Motherboard)
public:
	bool asPlayedMode;

	float Volume;
	float pannings[MAX_VOICES];
	Voice voices[MAX_VOICES];
	VoiceAllocator<MAX_VOICES> voiceAlloc;
	bool Oversample;

	bool economyMode;
	Motherboard(): left(),right()
	{
		economyMode = true;
		lkl=lkr=0;
		asPlayedMode = false;
		asPlayedCounter = 0;
		for(int i = 0 ; i < 129 ; i++)
		{
			awaitingkeys[i] = false;
			priorities[i] = 0;
		}
		Oversample=false;
		wasUni = false;
		Volume=0;
	//	voices = new Voice* [MAX_VOICES];
	//	pannings = new float[MAX_VOICES];
		totalvc = MAX_VOICES;
		voiceAlloc.init(MAX_VOICES,voiceList);
		for(int i = 0 ; i < MAX_VOICES;++i)
		{
			voices[i].voiceNumber = i;
			voiceList[i] = &voices[i];
			pannings[i]= 0.5;
		}
	}
	~Motherboard()
	{
		//delete pannings;
		//for(int i = 0 ; i < MAX_VOICES;++i)
		//{
		//	delete voices[i];
		//}
		//delete voices;
	}
	void setVoiceCount(int count)
	{
		for(int i = count ; i < MAX_VOICES;i++)
		{
			voices[i].NoteOff();
			voices[i].ResetEnvelope();
		}
		voiceAlloc.init(count, voiceList);
		totalvc = count;
	}
	void setSampleRate(float sr)
	{
		sampleRate = sr;
		sampleRateInv = 1 / sampleRate;
		for(int i = 0 ; i < MAX_VOICES;++i)
			voices[i].setSampleRate(sr);
		SetOversample(Oversample);
	}
	void sustainOn()
	{
		for(int i = 0 ; i < MAX_VOICES;i++)
			voices[i].sustOn();
	}
	void sustainOff()
	{
		for(int i = 0 ; i < MAX_VOICES;i++)
			voices[i].sustOff();
	}
	void SetOversample(bool over)
	{
		for(int i = 0 ; i < MAX_VOICES;i++)
		{
			voices[i].setHQ(over);
			if(over)
				voices[i].setSampleRate(sampleRate*2);
			else
				voices[i].setSampleRate(sampleRate);
		}
		Oversample = over;
	}
	inline float processSynthVoice(Voice& voice)
	{
		if(economyMode)
			voice.checkAdssrState();
		// Always update LFOs to keep them in phase even if voice
		// is not playing.
		voice.lfo1.update();
		voice.lfo2.update();
		if(voice.shouldProcessed||(!economyMode))
		{
			return voice.ProcessSample();
		}
		return 0;
	}
	void processSample(float* sm1,float* sm2)
	{
		float vl=0,vr=0;
		float vlo = 0 , vro = 0 ;

		for(int i = 0 ; i < totalvc;i++)
		{
				float x1 = processSynthVoice(voices[i]);
				if(Oversample)
				{
					float x2 =  processSynthVoice(voices[i]);
					vlo+=x2*(1-pannings[i]);
					vro+=x2*(pannings[i]);
				}
				vl+=x1*(1-pannings[i]);
				vr+=x1*(pannings[i]);
		}
		if(Oversample)
		{
			vl = left.Calc(vl,vlo);
			vr = right.Calc(vr,vro);
		}
		*sm1 = vl*Volume;
		*sm2 = vr*Volume;
	}
};
