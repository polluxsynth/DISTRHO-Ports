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

class SquareGentle
{
private:
	const float distTarget = 1.5;

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareGentle)
public:
	float distSlope, distAmount, distLimit, distBreakpoint, distGainComp;

	SquareGentle()
	{
		distSlope=0;
		distAmount=0.001;
		distLimit=10;
		distBreakpoint=10;
		distGainComp=1;
	}
	~SquareGentle()
	{
	}
private:
	// Square distortion, to get distortion with minimal bandwidth expansion
	// Essentially, x*x is x ring modulated with itself
	// We change the sign when x < 0 to get symmetry
	inline float distfunc(float sample)
	{
		// Pre gain to bring maximum sample value into line with
		// cube dist
		sample *= 1.74;
		return sample * (sample >= 0 ? (1 - distAmount * sample) :
					       (1 + distAmount * sample));
	}
	void calcParams(void)
	{
		// distLimit: solve distortion derivative equation f'(x) = g
		// to get limit for when the transfer function turns into
		// a straight line
		distLimit = (1 - distSlope) / (2 * distAmount * 1.74); // max input before flatline
		distBreakpoint = distfunc(distLimit); // output at breakpoint
		distGainComp = distTarget/distfunc(distLimit > distTarget ?
						   distTarget :
						   distLimit);
	}
public:
	void setAmount(float val)
	{
		// Add a small amount to avoid division by zero and get
		// distLimit to be larger than any practical signal when
		// amount is set to 0.
		distAmount = val + 0.001;
		calcParams();
	}
	void setSlope(float val)
	{
		distSlope = val;
		calcParams();
	}
	inline float Apply(float sample)
	{
		if (sample > distLimit)
			sample = (sample - distLimit) * distSlope + distBreakpoint;
		else if (sample < -distLimit)
			sample = (sample + distLimit) * distSlope - distBreakpoint;
		else sample = distfunc(sample);
		sample *= distGainComp; // compensate for amplitude drop
		return sample;
	}
};
