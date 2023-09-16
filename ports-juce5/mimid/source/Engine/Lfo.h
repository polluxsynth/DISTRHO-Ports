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
#include "SynthEngine.h"
class Lfo
{
private:
	float phase; // 0 -> 1
	float sh; // peak +1/-1
	float s1;
    Random rg;
	float SampleRate;
	float SampleRateInv;

	float syncRate;
	bool synced;

public:
	float Frequency;
	float phaseInc;
	float frUnsc;//frequency value without sync
	float rawParam;
	int waveForm;
	Lfo()
	{
		phaseInc = 0;
		frUnsc=0;
		syncRate = 1;
		rawParam=0;
		synced = false;
		s1=0;
		Frequency=1;
		phase=0;
		sh=0;
		rg=Random();
	}
	void setSynced()
	{
		synced = true;
		recalcRate(rawParam);
	}
	void setUnsynced()
	{
		synced = false;
		phaseInc = frUnsc;
	}
	void hostSyncRetrigger(float bpm,float quaters)
	{
		if(synced)
		{
			phaseInc = (bpm/60.0)*syncRate;
			phase = phaseInc*quaters;
			phase = (fmod(phase,1)*2-1);
		}
	}
	inline float getVal()
	{
		float Res = 0;
		switch (waveForm)
		// OFF - TRI - SQU - SAWU - SAWD - S/H
		{
			case 0: break;
			case 1: Res = phase < 0.25 ? 4 * phase :
				      phase < 0.75 ? 2 - 4 * phase :
					             4 * phase - 4;
				break;
			case 2: Res = phase < 0.5 ? 1 : -1;
				break;
			case 3: Res = phase * 2 - 1;
				break;
			case 4: Res = 1 - 2 * phase;
				break;
			case 5: Res = sh;
				break;
		}
		return tptlpupw(s1, Res,3000,SampleRateInv);
	}
	void setSampleRate(float sr)
	{
		SampleRate=sr;
		SampleRateInv = 1 / SampleRate;
	}
	inline void update()
	{
		phase+=((phaseInc * SampleRateInv));
		if(phase > 1)
		{
			phase-=1;
			sh = rg.nextFloat()*2-1;
		}

	}
	void setFrequency(float val)
	{
		frUnsc = val;
		if(!synced)
			phaseInc = val;
	}
	void setRawParam(float param)//used for synced rate changes
	{
		rawParam = param;
		if(synced)
		{
			recalcRate(param);
		}
	}
	void recalcRate(float param)
	{
		const int ratesCount = 9;
		int parval = (int)(param*(ratesCount-1));
		float rt = 1;
		switch(parval)
		{
		case 0:
			rt = 1.0 / 8;
			break;
		case 1:
			rt = 1.0 / 4;
			break;
		case 2:
			rt = 1.0 / 3;
			break;
		case 3:
			rt = 1.0 / 2;
			break;
		case 4:
			rt = 1.0;
			break;
		case 5:
			rt = 3.0 / 2;
			break;
		case 6:
			rt = 2;
			break;
		case 7:
			rt = 3;
			break;
		case 8:
			rt = 4;
			break;
		default:
			rt = 1;
			break;
		}
		syncRate = rt;
	}
};
