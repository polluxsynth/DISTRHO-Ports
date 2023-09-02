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
class AdsrEnvelope
{
private:
	float Value;
	float attack, decay, sustain, release;
	float ua,ud,us,ur;
	float coef;
	enum { ATK, DEC, SUS, REL, OFF } state;
	float SampleRate;
	float uf;
public:
	AdsrEnvelope()
	{
		uf = 1;
		Value = 0.0;
		attack=decay=sustain=release=0.0001;
		ua=ud=us=ur=0.0001;
		coef = 0;
		state = OFF;
		SampleRate = 44000;
	}
	void ResetEnvelopeState()
	{
		Value = 0.0;
		state = OFF;
	}
	void setSampleRate(float sr)
	{
		SampleRate = sr;
	}
	void setUniqueDeriviance(float der)
	{
		uf = der;
		setAttack(ua);
		setDecay(ud);
		setSustain(us);
		setRelease(ur);
	}
	void setAttack(float atk)
	{
		ua = atk;
		attack = atk*uf;
		if (state == ATK)
			coef = (float)((log(0.001) - log(1.3)) / (SampleRate * (atk) / 1000));
	}
	void setDecay(float dec)
	{
		ud = dec;
		decay = dec*uf;
		if (state == DEC)
			coef = (float)((log(jmin(sustain + 0.0001,0.99)) - log(1.0)) / (SampleRate * (dec) / 1000));
	}
	void setSustain(float sust)
	{
		us = sust;
		sustain = sust;
		if (state == DEC)
			coef = (float)((log(jmin(sustain + 0.0001,0.99)) - log(1.0)) / (SampleRate * (decay) / 1000));
	}
	void setRelease(float rel)
	{
		ur = rel;
		release = rel*uf;
		if (state == REL)
			coef = (float)((log(0.00001) - log(Value + 0.0001)) / (SampleRate * (rel) / 1000));
	}
	void triggerAttack()
	{
		state = ATK;
		//Value = Value +0.00001f;
		coef = (float)((log(0.001) - log(1.3)) / (SampleRate * (attack)/1000 ));
	}
	void triggerRelease()
	{
		if (state != REL)
			coef = (float)((log(0.00001) - log(Value+0.0001)) / (SampleRate * (release) / 1000));
		state = REL;
	}
	inline bool isActive()
	{
		return state != OFF;
	}
	inline float processSample()
	{
		switch (state)
		{
		case ATK:
			if (Value - 1  > -0.1)
			{
				Value = jmin(Value, 0.99f);
				state = DEC;
				coef = (float)((log(jmin(sustain + 0.0001, 0.99)) - log(1.0)) / (SampleRate * (decay) / 1000));
				goto dec;
			}
			else
				Value = Value - (1-Value)*(coef);
			break;
		case DEC:
dec:
			if (Value - sustain < 10e-6)
				state = SUS;
			else
				Value =Value + Value * coef;
			break;
		case SUS:
			Value = jmin(sustain, 0.9f);
			break;
		case REL:
			if (Value > 20e-6)
				Value = Value + Value * coef + dc;
			else
				state = OFF;
			break;
		case OFF:
			Value = 0.0f;
			break;
		}
		return Value;
	}
};
