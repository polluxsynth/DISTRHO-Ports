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
enum Parameters
{
	UNDEFINED,
	UNUSED_1,
	VOLUME,
	VOICE_COUNT,
	TUNE,
	OCTAVE,
	BENDRANGE,
	BENDOSC2,
	LEGATOMODE,
	BENDLFORATE,
	VFLTENV,
	VAMPENV,

	ASPLAYEDALLOCATION,
	PORTAMENTO,
	UNISON,		
	UDET,
	OSC2_DET,
	LFOFREQ,
	LFOSINWAVE,LFOSQUAREWAVE,LFOSHWAVE,
	LFO1AMT,LFO2AMT,
	LFOOSC1,LFOOSC2,LFOFILTER,LFOPW1,LFOPW2,
	OSC2HS,
	XMOD,
	OSC1P,
	OSC2P,
	OSCQuantize,
	OSC1Saw_NOTUSED,
	OSC1Pul_NOTUSED,
	OSC2Saw_NOTUSED,
	OSC2Pul_NOTUSED,
	OSC1PW,
	BRIGHTNESS,
	ENVPITCH,
	OSC1MIX,
	OSC2MIX,
	NOISEMIX,
	FLT_KF,
	CUTOFF,
	RESONANCE,
	RESPONSE,
	OVERSAMPLE,
	BANDPASS_NOTUSED,
	FOURPOLE_NOTUSED,
	ENVELOPE_AMT,
	LATK,
	LDEC,
	LSUS,
	LSUST,
	FATK,
	FDEC,
	FSUS,
	FSUST,
	ENVDER,FILTERDER,PORTADER,
	PAN1,PAN2,PAN3,PAN4,PAN5,PAN6,PAN7,PAN8,
	UNUSED_2,
	ECONOMY_MODE,
	LFO_SYNC,
	PW_ENV,
	PW_ENV_BOTH,
	ENV_PITCH_BOTH,
	FENV_INVERT,
	OSC2PW,
	LEVEL_DIF,
	SELF_OSC_PUSH_NOTUSED,
	OSC1WAVE,
	OSC2WAVE,
	FREL,
	LREL,
	FILTER_TYPE,
	PARAM_COUNT,
};
