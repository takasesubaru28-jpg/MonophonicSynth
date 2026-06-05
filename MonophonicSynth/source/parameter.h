#pragma once
#include "pluginterfaces/base/ftypes.h"

namespace Steinberg
{
	namespace Vst
	{
		namespace MonophonicSynth
		{
			// -----------------------------------------------------------
			// OscillatorParameter
			// -----------------------------------------------------------
			enum OscillatorParam 
			{
				TUNE_Osc1,
				RANGE_Osc1,
				WAVEFORM_Osc1,

				FREQUENCY_Osc2,
				RANGE_Osc2,
				WAVEFORM_Osc2,

				FREQUENCY_Osc3,
				RANGE_Osc3,
				WAVEFORM_Osc3,
				CONTROL_Osc3,

				OSILLATORMODULATION,
				KYBD_TRACK_1,
				KYBD_TRACK_2
			};

			enum Controllers
			{
				OSC3_FILTER = 400,
				NOISE_LFO,
				MODULATIONMIX,
				GLIDE_VAL
			};

			enum MixerNoise
			{
				VOLUME_OSC1 = 100,
				VOLUME_OSC2,
				VOLUME_OSC3,
				NOISEVOLUME,
				WHITEPINK
			};

			enum Modifiers
			{
				CUTOFFFREQUENCY=300,
				EMPHASIS,
				AMOUNTOFCONTOUR,
				ATTACKTIME_VOL,
				DECAYTIME_VOL,
				SUSTAINLEVEL_VOL,

				ATTACKTIME_FREQ,
				DECAYTIME_FREQ,
				SUSTAINLEVEL_FREQ,

				FILTERMODULATION
			};

			enum Output
			{
				VOLUME=200,
			};

			enum LeftHand
			{
				LFORATE=500,
				GLIDE,
				DECAY,
				PUSHPULL,
				MOD_WHEEL
			};
		}
	}
}
