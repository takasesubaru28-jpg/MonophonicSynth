#pragma once
#include <cmath>
#include "pluginterfaces/base/ftypes.h"


namespace Steinberg {
	namespace Vst {
		namespace MonophonicSynth {

			// オシレータで使用する波形の定義
			enum class WaveType
			{
				tri, tri_saw, rev_saw, saw, pulse, pulse_wide, pulse_narrow
			};


			//---------------------------------------------宣言部------------------------------------------

			template<class SamplePrecision>
			class Oscillator
			{
			public:
				// コンストラクタ，デストラクタ
				Oscillator(int32 bufferSize, SamplePrecision sampleRate);
				~Oscillator();

				// privateメンバのゲッタ
				SamplePrecision at(int32 pos) const { return buffer[pos]; }
				int32 getSize() const { return bufferSize; }

				SamplePrecision nextSample();
				void setFrequency(SamplePrecision f);
				void setWaveType(WaveType changeType);
				void resetPhase() { phase = 0.0f; };

			protected:
				SamplePrecision* buffer;
				int32 bufferSize;
				SamplePrecision sampleRate;
				WaveType waveType;
				SamplePrecision phase;
				SamplePrecision phaseIncrement;
				SamplePrecision freq;
				void generateWave();
				void generateTri();
				void generateTri_Saw();
				void generateRev_Saw();
				void generateSaw();
				void generatePulse();
				void generatePulse_Wide();
				void generatePulse_Narrow();
			};


			//------------------------------------------------定義部--------------------------------------------

			template<class SamplePrecision>
			Oscillator<SamplePrecision>::Oscillator(int32 bufferSize, SamplePrecision sampleRate)
				: buffer(nullptr), bufferSize(bufferSize), sampleRate(sampleRate), waveType(WaveType::tri), phase(0), freq(440), phaseIncrement(0)
			{
				buffer = new SamplePrecision[bufferSize];
				generateWave();
			};

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generateWave()
			{
				switch (waveType)
				{
				case WaveType::tri:
					generateTri();
					break;
				case WaveType::saw:
					generateSaw();
					break;
				case WaveType::tri_saw:
					generateTri_Saw();
					break;
				case WaveType::rev_saw:
					generateRev_Saw();
					break;
				case WaveType::pulse:
					generatePulse();
					break;
				case WaveType::pulse_wide:
					generatePulse_Wide();
					break;
				case WaveType::pulse_narrow:
					generatePulse_Narrow();
					break;
				}
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::setFrequency(SamplePrecision f)
			{
				freq = f;
				/*phaseIncrement = (bufferSize * freq) / sampleRate;*/
				phaseIncrement = (freq) / sampleRate;
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::setWaveType(WaveType changeType)
			{
				waveType = changeType;
				generateWave();
			}

			template<class SamplePrecision>
			SamplePrecision Oscillator<SamplePrecision>::nextSample()
			{
				// 1. 現在の位相(0.0〜1.0)に対応するバッファ位置（浮動小数点）を計算
				SamplePrecision p = phase * static_cast<SamplePrecision>(bufferSize);

				// 2. 線形補間のためのインデックス計算
				int32 i0 = static_cast<int32>(p);           // 現在のインデックス
				int32 i1 = (i0 + 1) % bufferSize;           // 次のインデックス（ラップ周回対応）
				SamplePrecision frac = p - static_cast<SamplePrecision>(i0); // 小数点以下の余り

				// 3. バッファから値を読み取って補間（これがないと階段状のノイズになる）
				SamplePrecision s0 = buffer[i0];
				SamplePrecision s1 = buffer[i1];
				SamplePrecision sample = s0 + frac * (s1 - s0);

				// 4. 位相を進める（Processorからセットされた phaseIncrement を使用）
				phase += phaseIncrement;

				// 5. 1.0を超えたらループ
				if (phase >= 1.0f) phase -= 1.0f;
				if (phase < 0.0f)  phase += 1.0f; // 逆再生・負の周波数対応

				return sample;
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generateTri()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					buffer[i] = 2.0f * std::fabs(2.0f * t - 1.0f) - 1.0f;
				}
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generateSaw()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					buffer[i] = 2.0f * t - 1.0f;
				}
			}


			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generateRev_Saw()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					buffer[i] = -2.0f * t + 1.0f;
				}
			}


			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generateTri_Saw()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					buffer[i] = ((2.0f * t - 1.0f) + (2.0f * std::fabs(2.0f * t - 1.0f) - 1.0f)) / 2.0f;
				}
			}


			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generatePulse()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					if (i < (int)(bufferSize / 2))
					{
						buffer[i] = 1;
					}
					else
					{
						buffer[i] = 0;
					}
				}
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generatePulse_Wide()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					if (i < (int)(3*bufferSize / 4))
					{
						buffer[i] = 1;
					}
					else
					{
						buffer[i] = 0;
					}
				}
			}

			template<class SamplePrecision>
			void Oscillator<SamplePrecision>::generatePulse_Narrow()
			{
				for (int i = 0; i < bufferSize; i++)
				{
					SamplePrecision t = static_cast<SamplePrecision>(i) / bufferSize;
					if (i < (int)(bufferSize / 4))
					{
						buffer[i] = 1;
					}
					else
					{
						buffer[i] = 0;
					}
				}
			}

			template<class SamplePrecision>
			Oscillator<SamplePrecision>::~Oscillator() {
				if (buffer) delete[] buffer;
				buffer = nullptr;
			}



		}
	}
}