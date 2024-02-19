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
class AdssrEnvelope
{
private:
	float Value;
	float attack, decay, sustain, sustainTime, release; // saved parameter values with deriverence
	bool adsrMode;
	float ua,ud,us,ur; // saved parameter values (not for sustain)
	float coef;
	enum { ATK, DEC, SUS, REL, OFF } state;
	float SampleRate;
	float uf;

	inline float coef_atk(float timeparam)
	{
		return 7.0f / (SampleRate * (timeparam)/1000);
	}
	inline float coef_dec(float timeparam)
	{
		return 7.0f / (SampleRate * (timeparam) / 1000);
	}
	inline float coef_rel(float timeparam)
	{
		return 7.0f / (SampleRate * (timeparam) / 1000);
	}
public:
	float unused1; // TODO: remove
	float unused2; // TODO: remove

	AdssrEnvelope()
	{
		uf = 1;
		Value = 0.0;
		attack=decay=sustain=sustainTime=release=0.0001;
		ua=ud=us=ur=0.0001;
		coef = 0;
		state = OFF;
		SampleRate = 44000;
		adsrMode = true;
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
	void setSpread(float spread)
	{
		uf = spread;
		setAttack(ua);
		setDecay(ud);
		setSustainTime(us);
		setRelease(ur);
	}
	void setAdsr(bool adsr)
	{
		adsrMode = adsr;
	}
	void setAttack(float atk)
	{
		ua = atk;
		attack = atk*uf;
		if (state == ATK)
			coef = coef_atk(atk);
	}
	void setDecay(float dec)
	{
		ud = dec;
		decay = dec*uf;
		if (state == DEC)
			coef = coef_dec(dec);
	}
	void setSustain(float sus)
	{
		sustain = sus;
	}
	void setSustainTime(float sust)
	{
		us = sust;
		sustainTime = sust*uf;
		if (state == SUS)
			coef = coef_rel(sust);
	}
	void setRelease(float rel)
	{
		ur = rel;
		release = rel*uf;
		if (state == REL)
			coef = coef_rel(rel);
	}
	void triggerAttack()
	{
		state = ATK;
		//Value = Value +0.00001f;
		coef = coef_atk(attack);
	}
	void triggerRelease()
	{
		if (state != REL)
			coef = coef_rel(release);
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
			if (Value > 0.99f)
			{
				Value = jmin(Value, 0.99f);
				state = DEC;
				coef = coef_dec(decay);
				goto dec;
			}
			else
				Value = Value + (1.3-Value)*(coef);
			break;
		case DEC:
dec:
			if (!adsrMode && Value - sustain < 0.01)
			{
				Value = jmin(sustain, 0.99f);
				state = SUS;
				coef = coef_rel(sustainTime);
				goto sus;
			}
			else
				Value = Value - (Value - sustain) * coef;
			break;
		case SUS:
sus:
			if (Value > 20e-6)
				Value = Value - Value * coef + dc;
			break;
		case REL:
			if (Value > 20e-6)
				Value = Value - Value * coef + dc;
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
