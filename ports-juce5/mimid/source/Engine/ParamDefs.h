/*
	==============================================================================
	This file is part of the MiMi-d synthesizer.

	Copyright 2024 Ricard Wanderlof

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

	// Parameter definitions. This file is pulled into various other
	// files using individual specific macros to extract the relevant
	// information for each case.
	// ParamsEnum.h just extracts the parameter symbol in order to create
	// the Parameters enum (order is important).
	// PluginProcessor sets up the process callbacks and names of the
	// parameters (and in this case the order is not important).

	PARAM(UNUSED_1, "Unused 1", procUnused1)
	PARAM(VOLUME, "Volume", processVolume)
	PARAM(VOICE_COUNT, "VoiceCount", setVoiceCount)
	PARAM(TUNE, "Tune", processTune)
	PARAM(OCTAVE, "Octave", processOctave)
	PARAM(BENDRANGE, "BendRange", procPitchWheelAmount)
	PARAM(BENDDEST, "BendDest", procPitchWheelDest)
	PARAM_NULL(LEGATOMODE_NOTUSED, "LegatoMode_NOTUSED")
	PARAM(LFO2FREQ, "Lfo2Frequency", processLfo2Frequency)
	PARAM(VFLTENV, "VFltFactor", procFltVelocityAmount)
	PARAM(VAMPENV, "VAmpFactor", procAmpVelocityAmount)

	PARAM_NULL(ASPLAYEDALLOCATION_NOTUSED, "AsPlayedAllocation_NOTUSED")
	PARAM(PORTAMENTO, "Portamento", processPortamento)
	PARAM(UNISON, "Unison", processUnison) // TODO part of keyasgn?
	PARAM(UDET, "OscSpread", processOscSpread)
	PARAM(OSC1_DET, "Oscillator1detune", processOsc1Det)
	PARAM(LFO1FREQ, "Lfo1Frequency", processLfo1Frequency)
	PARAM(LFO1WAVE, "Lfo1Wave", processLfo1Wave)
	PARAM(LFO2WAVE, "Lfo2Wave", processLfo2Wave)
	PARAM_NULL(LFOSHWAVE_UNUSED, "LfoSampleHoldWave")
	PARAM(LFO1AMT, "Lfo1Amount", processLfo1Amt)
	PARAM(LFO2AMT, "Lfo2Amount", processLfo2Amt)
	PARAM(LFO1DEST, "Lfo1Dest", processLfo1Dest)
	PARAM(LFO2DEST, "Lfo2Dest", processLfo2Dest)
	PARAM_NULL(LFOFILTER_UNUSED, "LfoFilter")
	PARAM_NULL(LFOPW1_UNUSED, "LfoPw1_NOTUSED")
	PARAM_NULL(LFOPW2_UNUSED, "LfoPw2_NOTUSED")
	PARAM(OSC2_DET, "Oscillator2detune", processOsc2Det)
	PARAM(XMOD, "Xmod", processOsc2Xmod)
	PARAM(OSC1P, "Osc1Pitch", processOsc1Pitch)
	PARAM(OSC2P, "Osc2Pitch", processOsc2Pitch)
	PARAM_NULL(OSCQuantize_NOTUSED, "PitchQuant_NOTUSED")
	PARAM_NULL(OSC1Saw_NOTUSED, "Osc1Saw_NOTUSED")
	PARAM_NULL(OSC1Pul_NOTUSED, "Osc1Pulse_NOTUSED")
	PARAM_NULL(OSC2Saw_NOTUSED, "Osc2Saw_NOTUSED")
	PARAM_NULL(OSC2Pul_NOTUSED, "Osc2Pulse_NOTUSED")
	PARAM(OSC1PW, "Osc1PW", processOsc1PulseWidth)
	PARAM(HPFFREQ, "HPFfreq", processHPFfreq)
	PARAM_NULL(ENVPITCH_NOTUSED, "EnvelopeToPitch_NOTUSED")
	PARAM(OSC1MIX, "Osc1Mix", processOsc1Mix)
	PARAM(OSC2MIX, "Osc2Mix", processOsc2Mix)
	PARAM(OSC3MIX, "Osc3Mix", processOsc3Mix)
	PARAM(FLT_KF, "FilterKeyFollow", processFilterKeyFollow)
	PARAM(CUTOFF, "Cutoff", processCutoff)
	PARAM(RESONANCE, "Resonance", processResonance)
	PARAM(RESPONSE, "Response", processResponse)
	PARAM(OVERSAMPLE, "Oversample", processOversampling)
	PARAM_NULL(BANDPASS_NOTUSED, "BandpassBlend_NOTUSED")
	PARAM_NULL(FOURPOLE_NOTUSED, "FourPole_NOTUSED")
	PARAM(ENVELOPE_AMT, "FilterEnvAmount", processFilterEnvelopeAmt)
	PARAM(LATK, "Attack", processLoudnessEnvelopeAttack)
	PARAM(LDEC, "Decay", processLoudnessEnvelopeDecay)
	PARAM(LSUS, "Sustain", processLoudnessEnvelopeSustain)
	PARAM(LSUST, "SustainTime", processLoudnessEnvelopeSustainTime)
	PARAM(FATK, "FilterAttack", processFilterEnvelopeAttack)
	PARAM(FDEC, "FilterDecay", processFilterEnvelopeDecay)
	PARAM(FSUS, "FilterSustain", processFilterEnvelopeSustain)
	PARAM(FSUST, "FilterSustainTime", processFilterEnvelopeSustainTime)
	PARAM(ENVDER, "EnvelopeSpread", processEnvelopeSpread)
	PARAM(FILTERDER, "FilterSpread", processFilterSpread)
	PARAM(PORTADER, "PortamentoSpread", processPortamentoSpread)
	PARAM_NULL(PAN1_NOTUSED, "Pan1_NOTUSED")
	PARAM_NULL(PAN2_NOTUSED, "Pan2_NOTUSED")
	PARAM_NULL(PAN3_NOTUSED, "Pan3_NOTUSED")
	PARAM_NULL(PAN4_NOTUSED, "Pan4_NOTUSED")
	PARAM_NULL(PAN5_NOTUSED, "Pan5_NOTUSED")
	PARAM_NULL(PAN6_NOTUSED, "Pan6_NOTUSED")
	PARAM_NULL(PAN7_NOTUSED, "Pan7_NOTUSED")
	PARAM_NULL(PAN8_NOTUSED, "Pan8_NOTUSED")
	PARAM(UNUSED_2, "Unused 2", procUnused2)
	PARAM(ECONOMY_MODE, "EconomyMode", procEconomyMode)
	PARAM(LFO1SYNC, "Lfo1Sync", procLfo1Sync)
	PARAM_NULL(PW_ENV_NOTUSED, "PwEnv_NOTUSED")
	PARAM_NULL(PW_ENV_BOTH_NOTUSED, "PwEnvBoth_NOTUSED")
	PARAM_NULL(ENV_PITCH_BOTH_NOTUSED, "EnvPitchBoth_NOTUSED")
	PARAM(FENV_INVERT, "FenvInvert", processInvertFenv)
	PARAM(OSC2PW, "Osc2PW", processOsc2PulseWidth)
	PARAM(LEVEL_DIF, "LevelSpread", processLoudnessSpread)
	PARAM_NULL(SELF_OSC_PUSH_NOTUSED, "SelfOscPush_NOTUSED")
	PARAM(OSC1WAVE, "Osc1Wave", processOsc1Wave)
	PARAM(OSC2WAVE, "Osc2Wave", processOsc2Wave)
	PARAM(FREL, "FilterRelease", processFilterEnvelopeRelease)
	PARAM(LREL, "Release", processLoudnessEnvelopeRelease)
	PARAM(FILTER_TYPE, "FilterType", processFilterType)
	PARAM(OSC1FLTMOD, "Osc1FilterMod", processOsc1FltMod)

        // ReSet to Zero (lowest) voice (default cyclic)
	PARAM(ASGN_RSZ, "KeyAssignRsz", procKeyAsgnRsz)
        // Prefer assign to voice previously with same note
	PARAM(ASGN_MEM, "KeyAssignMem", procKeyAsgnMem)
        // Rob a playing voice if no unplaying available
        // two modes: oldest (O) and next-to-lowest (NL)
	PARAM(ASGN_ROB, "KeyAssignRob", procKeyAsgnRob)
        // Restore mode: Store notes until voice available
	PARAM(ASGN_RES, "KeyAssignRes", procKeyAsgnRes)
        // Single trig: behavior during rob and restore
	PARAM(ASGN_STRG, "KeyAssignStrg", procKeyAsgnStrg)

	PARAM(LFO2SYNC, "Lfo2Sync", procLfo2Sync)

	PARAM(LFOSPREAD, "LfoSpread", processLfoSpread)

	PARAM(PANSPREAD, "PanSpread", processPanSpread)

	PARAM_NULL(GATK_NOTUSED, "GenAttack_NOTUSED")
	PARAM_NULL(GDEC_NOTUSED, "GenDecay_NOTUSED")
	PARAM_NULL(GSUS_NOTUSED, "GenSustain_NOTUSED")
	PARAM_NULL(GSUST_NOTUSED, "GenSustainTime_NOTUSED")
	PARAM_NULL(GREL_NOTUSED, "GenRelease_NOTUSED")

	PARAM_NULL(GAMT_NOTUSED, "GenEnvAmount_NOTUSED")
	PARAM_NULL(GDEST_NOTUSED, "GenEnvDest_NOTUSED")
	PARAM_NULL(GUNI_NOTUSED, "GenEnvUnipol_NOTUSED")
	PARAM_NULL(GINV_NOTUSED, "GenEnvInvert_NOTUSED")

	PARAM_NULL(VGENENV_NOTUSED, "VGenFactor_NOTUSED")
	PARAM(VCADRIVE, "VCADrive", processVCADrive)

	PARAM(OSCSYNC_LEVEL, "SyncLevel", processOsc2SyncLevel)

	PARAM(OSC3WAVE, "Osc3Wave", processOsc3Wave)

	// 111
	PARAM(VEL_SCALE, "VelocityScale", procVelocityScale)
	PARAM(AT_SCALE, "AfterTouchScale", procAfterTouchScale)

	// 113
	PARAM(LFO1_AMT_CTRL, "Lfo1AmtCont", procLfo1Controller)
	PARAM(LFO2_AMT_CTRL, "Lfo2AmtCont", procLfo2Controller)

	// 115
	PARAM(LFO1_CONTRAMT, "Lfo1ContrAmt", procLfo1ControllerAmt)
	PARAM(LFO2_CONTRAMT, "Lfo2ContrAmt", procLfo2ControllerAmt)

	// 117
	PARAM(LFO1_POLARITY, "Lfo1Polarity", procLfo1Polarity)
	PARAM(LFO2_POLARITY, "Lfo2Polarity", procLfo2Polarity)

	// 119
	PARAM(OSC_KEY_SYNC, "OscKeySync", procOscKeySync)
	PARAM(ENV_RST, "EnvRst", procEnvRst)
