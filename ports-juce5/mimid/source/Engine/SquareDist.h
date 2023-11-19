/*
	==============================================================================
        This file is part of the MiMi-d synthesizer.

        Copyright 2023 Ricard Wanderlof

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

class SquareDist
{
private:
	const float distTarget = 1.5;

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareDist)
public:
	float diodeImbalance;
	float distAmount;
	struct distParams {
		float amount, limit, peak;
	};
	distParams distP, distM;
	float distGainComp;

	SquareDist()
	{
		distP.amount=0.001;
		distP.limit=10;
		distP.peak=10;
		distM.amount=0.001;
		distM.limit=10;
		distM.peak=10;
		distAmount= 0;
		diodeImbalance=0;
		distGainComp=1;
	}
	~SquareDist()
	{
	}
private:
	// Square distortion, to get distortion with minimal bandwidth expansion
	// Essentially, x*x is x ring modulated with itself
	// We change the sign when x < 0 to get symmetry
	inline float distfunc(float amount, float sample)
	{
		return sample * (sample >= 0 ? (1 - amount * sample) :
					       (1 + amount * sample));
	}
public:
	void calcParams(void)
	{
		// Add a small amount to avoid division by zero and get
		// distLimit to be larger than any practical signal when
		// amount is set to 0.
		distP.amount = distAmount * (1 + diodeImbalance) + 0.001;
		distM.amount = distAmount + 0.001;
		// distLimit: solve distortion derivative equation f'(x) = 0
		// to get limit for when the signal flatlines
		distP.limit = 1/(2 * distP.amount); // max input before flatline
		distP.peak = distfunc(distP.amount, distP.limit); // max output
		distM.limit = -1/(2 * distM.amount); // min input before flatline
		distM.peak = distfunc(distM.amount, distM.limit); // min output

		// Take average of gain compensation for plus and negative
		// portions.
		float distGainCompP =
			distTarget/distfunc(distP.amount,
					    distP.limit > distTarget ?
						distTarget :
						distP.limit);
		float distGainCompM =
			distTarget/distfunc(distM.amount,
					    distM.limit < -distTarget ?
						distTarget :
						-distM.limit);

		distGainComp = (distGainCompP + distGainCompM) / 2;
	}
	void setAmount(float val)
	{
		distAmount = val;
		calcParams();
	}
	void setImbalance(float val)
	{
		diodeImbalance = val;
		calcParams();
	}
	inline float Apply(float sample)
	{
		if (sample > distP.limit) sample = distP.peak;
		else if (sample < distM.limit) sample = distM.peak;
		else if (sample < 0) sample = distfunc(distM.amount, sample);
		else sample = distfunc(distP.amount, sample);
		sample *= distGainComp; // compensate for amplitude drop
		return sample;
	}
};
