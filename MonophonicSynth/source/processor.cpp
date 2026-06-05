#include "processor.h"
#include "parameter.h"
#include "oscillator.h"
#include "fuid.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vstspeaker.h"
#include "noise.h"
#include "filter.h"
#include "contour.h"



namespace Steinberg {
	namespace Vst {
		namespace MonophonicSynth {

			//============================================================
			// コンストラクタ
			// ============================================================
			Processor::Processor()
				:osc1(1024, 44100.0f), osc2(1024, 44100.0f), osc3(1024, 44100.0f), lfo(1024, 44100.0f), noise(true), VolumeContour(), FreqContour()
			{
				setControllerClass(ControllerUID);

				normalizedVelocity = 1.0f;
				mixed = 0.0f;
				currentPitch = 0.0f;

				// オシレータ用パラメータ
				tune = 0.0f;
				tune_Osc1 = 0.0f;
				range_Osc1 = 1.0f;
				waveform_Osc1 = WaveType::tri;

				frequency_Osc2 = 0.0f;
				range_Osc2 = 1.0f;
				waveform_Osc2 = WaveType::tri;

				frequency_Osc3 = 0.0f;
				range_Osc3 = 1.0f;
				waveform_Osc3 = WaveType::tri;
				controll_Osc3 = 1.0f;

				osillatorModulation = 0.0f;

				// Contorollers
				osc3Filter = true;
				noiseLfo = true;
				modulationMix = 0.5f;

				// Mixer&Noise
				volume1 = 1.0f;
				volume2 = 1.0f;
				volume3 = 1.0f;
				white = true;
				volume_noise = 0.0f;

				// modifires
				//filter = Filter();
				emphasis = 3.0f;
				cut_freq = 1000.0f;
				FreqContour.setOrg(cut_freq);

				filterModulation = true;
				filter_mod = 0.0f;
				loud_mod = 0.0f;

				amountContour_freq = 0.5f;
				attackTime_freq = 5.0f;
				decayTime_freq = 5.0f;
				sustainLevel_freq = 1.0f;

				attackTime_vol = 5.0f;
				decayTime_vol = 5.0f;
				sustainLevel_vol = 1.0f;

				// アウトプットセクションパラメータ
				volume = 0.5f;
				VolumeContour.setOrg(volume);
				mainOutput = 1.0f;

				// LeftHand
				lfoRate = 0.5f;
				glide = false;
				decay = false;
				push_pull = WaveType::tri;
				lfo.setWaveType(push_pull);
				lfo.setFrequency(lfoRate);
			}

			tresult PLUGIN_API Processor::initialize(FUnknown* context)
			{
				tresult result = AudioEffect::initialize(context);
				if (result == kResultTrue)
				{
					addAudioOutput(STR16("Audio Output"), SpeakerArr::kStereo);
					addEventInput(STR16("Event Input"), 1);
				}
				return result;
			}

			// only support one stereo output
			tresult PLUGIN_API Processor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
			{
				if (numIns == 0 && numOuts == 1 && outputs[0] == SpeakerArr::kStereo)
				{
					return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
				}
				return kResultFalse;
			}

			// 音声処理部
			tresult PLUGIN_API Processor::process(ProcessData& data)
			{
				// 与えられたパラメータがあるときdata.inputParameterChangesにIParameterChangesクラスへのポインタが入る
				if (data.inputParameterChanges != NULL)
				{
					int32 paramChangeCount = data.inputParameterChanges->getParameterCount();

					for (int32 i = 0; i < paramChangeCount; i++)
					{
						IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);

						if (queue != NULL)
						{
							// どのパラメータが変更されたかタグを取得
							int32 tag = queue->getParameterId();

							// 変更された回数を取得
							int32 valueChangeCount = queue->getPointCount();
							ParamValue value;
							int32 sampleOffset;

							// 最後に変更された値を取得
							if (queue->getPoint(valueChangeCount - 1, sampleOffset, value) == kResultTrue)
							{
								// tagに応じた処理を実施
								switch (tag)
								{
								case TUNE_Osc1:
								{
									tune = value * 2 - 1;
									break;
								}

								case RANGE_Osc1:
								{
									// StringListParameterは0〜1の範囲の離散値を返すので、整数インデックスに変換
									int index = static_cast<int>(value * 5.999f); // 6段階 ("LO","32'","16'","8'","4'","2'")

									switch (index)
									{
									case 0: range_Osc1 = 0.125f; break;  // "LO" (Low frequency)
									case 1: range_Osc1 = 0.25f; break;  // "32’"
									case 2: range_Osc1 = 0.5f;  break;  // "16’"
									case 3: range_Osc1 = 1.0f;  break;  // "8’"
									case 4: range_Osc1 = 2.0f;  break;  // "4’"
									case 5: range_Osc1 = 4.0f;  break;  // "2’"
									default: range_Osc1 = 1.0f; break;  // fallback
									}
									break;
								}

								case WAVEFORM_Osc1:
								{
									WaveType type = static_cast<WaveType>(static_cast<int>(value * 5));
									osc1.setWaveType(type);
									break;
								}

								case FREQUENCY_Osc2:
								{
									frequency_Osc2 = value * 7.0f - 7.0f;
									break;
								}

								case RANGE_Osc2:
								{
									// StringListParameterは0〜1の範囲の離散値を返すので、整数インデックスに変換
									int index = static_cast<int>(value * 5.999f); // 6段階 ("LO","32'","16'","8'","4'","2'")

									switch (index)
									{
									case 0: range_Osc2 = 0.125f; break;  // "LO" (Low frequency)
									case 1: range_Osc2 = 0.25f; break;  // "32’"
									case 2: range_Osc2 = 0.5f;  break;  // "16’"
									case 3: range_Osc2 = 1.0f;  break;  // "8’"
									case 4: range_Osc2 = 2.0f;  break;  // "4’"
									case 5: range_Osc2 = 4.0f;  break;  // "2’"
									default: range_Osc2 = 1.0f; break;  // fallback
									}
									break;
								}

								case WAVEFORM_Osc2:
								{
									WaveType type = static_cast<WaveType>(static_cast<int>(value * 5));
									osc2.setWaveType(type);
									break;
								}

								case FREQUENCY_Osc3:
								{
									frequency_Osc3 = value * 7.0f - 7.0f;
									break;
								}

								case RANGE_Osc3:
								{
									// StringListParameterは0〜1の範囲の離散値を返すので、整数インデックスに変換
									int index = static_cast<int>(value * 5.999f); // 6段階 ("LO","32'","16'","8'","4'","2'")

									switch (index)
									{
									case 0: range_Osc3 = 0.125f; break;  // "LO" (Low frequency)
									case 1: range_Osc3 = 0.25f; break;  // "32’"
									case 2: range_Osc3 = 0.5f;  break;  // "16’"
									case 3: range_Osc3 = 1.0f;  break;  // "8’"
									case 4: range_Osc3 = 2.0f;  break;  // "4’"
									case 5: range_Osc3 = 4.0f;  break;  // "2’"
									default: range_Osc3 = 1.0f; break;  // fallback
									}
									break;
								}

								case WAVEFORM_Osc3:
								{
									WaveType type = static_cast<WaveType>(static_cast<int>(value * 5));
									osc3.setWaveType(type);
									break;
								}

								case CONTROL_Osc3:
								{
									controll_Osc3 = (value > 0.5);
									break;
								}

								case OSILLATORMODULATION:
								{
									osillatorModulation = (value > 0.5);
									break;
								}

								case OSC3_FILTER:
								{
									osc3Filter = (value < 0.5);
									break;
								}

								case NOISE_LFO:
								{
									noiseLfo = (value > 0.5);
									break;
								}

								case MODULATIONMIX:
								{
									modulationMix = value;
									break;
								}

								case VOLUME_OSC1:
								{
									volume1 = value;
									break;
								}

								case VOLUME_OSC2:
								{
									volume2 = value;
									break;
								}

								case VOLUME_OSC3:
								{
									volume3 = value;
									break;
								}

								case NOISEVOLUME:
								{
									volume_noise = value / 10.0f;
									break;
								}

								case WHITEPINK:
								{
									int index = static_cast<int>(value * 1.999f); // 2段階 white or pink

									switch (index)
									{
									case 0:
										white = true; // white
										noise.setWhite(white);
										break;
									case 1: white = false; // pink
										noise.setWhite(white);
										break;
									}
									break;
								}

								case CUTOFFFREQUENCY:
								{
									// 非線形変換 (例: 対数スケール)
									float minFreq = 10.0f;
									float maxFreq = 32000.0f;

									// value は 0.0〜1.0 の範囲
									// 対数スケールに変換
									cut_freq = minFreq * pow(maxFreq / minFreq, value);

									//filter.setFreq(cut_freq);
									FreqContour.setOrg(cut_freq);
									break;
								}

								case EMPHASIS:
								{
									emphasis = value * 4.0f;
									filter.setEmphasis(emphasis);
									break;
								}

								case AMOUNTOFCONTOUR:
								{
									amountContour_freq = value * 10000;
									FreqContour.setAmount(amountContour_freq);
									break;
								}

								case ATTACKTIME_FREQ:
								{
									// 0〜1 の value を指数スケールで変換
									float minT = 0.001f;  // 0ms
									float maxT = 10.0f;  // 10s
									FreqContour.setAttack(maxT * powf(minT / maxT, 1.0f - value));
									break;
								}

								case DECAYTIME_FREQ:
								{
									float minT = 0.001f;  // 0ms
									float maxT = 10.0f;  // 10s
									FreqContour.setDecay(maxT * powf(minT / maxT, 1.0f - value));
									break;
								}

								case SUSTAINLEVEL_FREQ:
								{
									sustainLevel_freq = value;
									FreqContour.setSustain(sustainLevel_freq);
									break;
								}

								case ATTACKTIME_VOL:
								{
									float minT = 0.001f;  // 0ms
									float maxT = 10.0f;  // 10s
									VolumeContour.setAttack(maxT * powf(minT / maxT, 1.0f - value));
									break;
								}

								case DECAYTIME_VOL:
								{
									float minT = 0.001f;  // 0ms
									float maxT = 10.0f;  // 10s
									VolumeContour.setDecay(maxT * powf(minT / maxT, 1.0f - value));
									break;
								}

								case SUSTAINLEVEL_VOL:
								{
									sustainLevel_vol = value;
									VolumeContour.setSustain(sustainLevel_vol);
									break;
								}

								case FILTERMODULATION:
								{
									filterModulation = (value > 0.5);
									break;
								}

								case VOLUME:
								{
									volume = value;
									VolumeContour.setOrg(volume);
									break;
								}

								case LFORATE:
								{
									// 非線形変換 (例: 対数スケール)
									float minFreq = 0.1f;
									float maxFreq = 500.0f;

									// value は 0.0〜1.0 の範囲
									// 対数スケールに変換
									ParamValue lfoFreq = minFreq * pow(maxFreq / minFreq, value);

									lfo.setFrequency(lfoFreq);
									break;
								}

								case GLIDE:
								{
									glide = (value > 0.5);
									break;
								}

								case GLIDE_VAL:
								{
									glide_val = value;
									break;
								}

								case DECAY:
								{
									decay = (value > 0.5);
									break;
								}

								case PUSHPULL:
								{
									if (value > 0.5)
									{
										push_pull = WaveType::tri;
									}
									else
									{
										push_pull = WaveType::pulse;
									}
									lfo.setWaveType(push_pull);
									break;
								}

								case KYBD_TRACK_1:
								{
									kybdTrack1 = (value > 0.5);
									break;
								}

								case KYBD_TRACK_2:
								{
									kybdTrack2 = (value > 0.5);
									break;
								}

								case MOD_WHEEL:
								{
									modWheel = value;
									break;
								}

								}
							}
						}
					}
				}

				IEventList* eventList = data.inputEvents;
				if (eventList != NULL)
				{
					// イベントの数を取得
					int32 numEvent = eventList->getEventCount();
					for (int32 i = 0; i < numEvent; i++)
					{
						// i番目のイベントのデータを取得する
						Event event;
						if (eventList->getEvent(i, event) == kResultOk)
						{
							int16 channel;
							int16 noteNo;
							float velocity;

							// イベントデータごとの処理
							switch (event.type)
							{
							case Event::kNoteOnEvent:
								channel = event.noteOn.channel;
								noteNo = event.noteOn.pitch;
								velocity = event.noteOn.velocity;

								onNoteOn(channel, noteNo, velocity);

								break;

							case Event::kNoteOffEvent: // ノートオフイベントの場合
								channel = event.noteOff.channel;
								noteNo = event.noteOff.pitch;
								velocity = event.noteOff.velocity;

								onNoteOff(channel, noteNo, velocity);
								break;

							}
						}
					}
				}

				Sample32* outL = data.outputs[0].channelBuffers32[0];
				Sample32* outR = data.outputs[0].channelBuffers32[1];


				for (int32 i = 0; i < data.numSamples; i++)
				{
					float targetPitch = currentPitch;

					// 最新のノートをオシレータにセット
					if (!pitchList.empty())
					{
						targetPitch = pitchList.back();
					}

					if (glide) // GLIDEスイッチがONの場合
					{
						// currentPitch を targetPitch へ近づける
						// glide_val (0.0~1.0) を元にスピードを計算
						// 0.00001f〜0.01f 程度の範囲で調整（お好みで係数を変えてください）
						float minSpeed = 0.00001f;
						float maxSpeed = 0.1f;
						float shape = 4.0f;

						float curvedVal = powf(glide_val, shape);

						// その curvedVal を使ってスピードを決定
						float glideSpeed = maxSpeed * powf(minSpeed / maxSpeed, curvedVal);

						if (std::abs(targetPitch - currentPitch) < glideSpeed) {
							currentPitch = targetPitch;
						}
						else {
							if (currentPitch < targetPitch) currentPitch += glideSpeed;
							else currentPitch -= glideSpeed;
						}

					}
					else // GLIDEスイッチがOFFの場合
					{
						currentPitch = targetPitch; // 即座に目標ピッチへ
					}

					float bendRange = 2.0f;
					float bendFactor = std::pow(2.0f, (pitchBend * bendRange) / 12.0f);

					float pitchTune = std::pow(2.0f, tune / 12.0f);
					float deTune2 = std::pow(2.0f, frequency_Osc2 / 12.0f);
					float deTune3 = std::pow(2.0f, frequency_Osc3 / 12.0f);

					float currentLfo = lfo.nextSample();
					float currentNoise = noise.process();

					// --- OSC. 3 CONTROL の実装 ---
					if (controll_Osc3) {
						// ON の時：他のオシレーターと同様、キーボードのピッチに従う
						osc3.setFrequency(currentPitch * range_Osc3 * pitchTune * deTune3);
					}
					else {
						// OFF の時：独立したオシレーターとして動作（currentPitch を無視）
						// 基底周波数（例: 440Hz相当）に RANGE と FREQUENCY(±7半音) だけを反映
						// これにより、鍵盤を弾いてもモジュレーションの速度が一定になる
						float baseFreq = 440.0f;
						osc3.setFrequency(baseFreq * range_Osc3 * deTune3);
					}
					float s3 = osc3.nextSample();

					float modSig = 0.0f;
					if (osillatorModulation) { // スイッチがONなら
						float srcA = osc3Filter ? s3 : FreqContour.getCurrentValue();
						float srcB = noiseLfo ? currentLfo : noise.process();
						modSig = (srcA * (1.0f - modulationMix)) + (srcB * modulationMix);
					}

					float targetModFactor = std::pow(2.0f, (modSig * 2.0f) / 12.0f);
					static float smoothModFactor = 1.0f;
					smoothModFactor = smoothModFactor + (targetModFactor - smoothModFactor) * 0.1f;

					osc1.setFrequency(currentPitch * range_Osc1 * pitchTune * smoothModFactor);
					osc2.setFrequency(currentPitch * range_Osc2 * pitchTune * deTune2 * smoothModFactor);

					// それぞれのオシレータから次のサンプル値を取得
					float s1 = osc1.nextSample();
					float s2 = osc2.nextSample();
					filter_mod = 0.0f;


					if (filterModulation)
					{
						// 1. エンベロープの基本値を取得（共通）
						float envFreq = FreqContour.process(false);

						// --- [追加] KEYBOARD CONTROL (Key Tracking) の計算 ---
						// 基準ノートを MIDI 60 (C3: 約261.63Hz) とした計算例
						float trackAmount = 0.0f;
						if (kybdTrack1) trackAmount += 1.0f / 3.0f;
						if (kybdTrack2) trackAmount += 2.0f / 3.0f;

						if (trackAmount > 0.0f) {
							// 弾いている音(currentPitch)が基準(261.63Hz)から何オクターブ離れているか計算
							// log2(f1/f0) でオクターブ差が出る
							float octaveDiff = std::log2(currentPitch / 261.63f);

							// 1オクターブにつきカットオフが1オクターブ分（2倍）移動するように倍率計算
							// finalFreq = envFreq * 2^(octaveDiff * trackAmount)
							envFreq *= std::pow(2.0f, octaveDiff * trackAmount);
						}
						// モジュレーション源の計算
						float sourceA = osc3Filter ? s3 : FreqContour.getCurrentValue();
						float sourceB = noiseLfo ? currentLfo : noise.process();
						float modSource = (sourceA * (1.0f - modulationMix)) + (sourceB * modulationMix);

						// 揺れの適用（トラッキング後のenvFreqに対して加算）
						float finalFreq = envFreq + (modSource * 5000.0f * modWheel);

						if (finalFreq < 10.0f) finalFreq = 10.0f;
						filter.setFreq(finalFreq);
					}
					else
					{
						// モジュレーションOFFの時はエンベロープ（Contour）のみ
						filter.setFreq(FreqContour.process(false));
					}


					mixed = (s1 * volume1 + s2 * volume2 + s3 * volume3) / 3.0f;

					if (!pitchList.empty())
					{

						//filter.setFreq(FreqContour.process(false));
						mixed = filter.process(mixed);
						mixed += volume_noise * noise.process();

						// 出力（左右同じ信号＝モノラル）
						float volEnv = VolumeContour.process(true);
						outL[i] = normalizedVelocity * volEnv * mixed;
						outR[i] = normalizedVelocity * volEnv * mixed;


						//outL[i] = normalizedVelocity * volume * mixed;
						//outR[i] = normalizedVelocity * volume * mixed;
					}
					else
					{
						//FreqContour.noteOff();
						//VolumeContour.noteOff();
						//filter.setFreq(FreqContour.process(false));
						mixed = filter.process(mixed);
						// 出力（左右同じ信号＝モノラル）
						float volEnv = VolumeContour.process(true);
						outL[i] = normalizedVelocity * volEnv * mixed;
						outR[i] = normalizedVelocity * volEnv * mixed;


						//outL[i] = normalizedVelocity * volume * mixed;
						//outR[i] = normalizedVelocity * volume * mixed;
					}

				}

				return kResultTrue;

			}

			void Processor::onNoteOn(int channel, int note, float velocity)
			{
				// midiノートオンイベントの処理を行う

				float pitch = (440.0f * powf(2.0f, (float)(note - (69)) / 12.0));
				targetPitch = pitch;

				// pitchListの最後に追加する
				pitchList.push_back(pitch);

				if (pitchList.size() == 1) {
					currentPitch = targetPitch;
				}

				normalizedVelocity = static_cast<float>(velocity);
				FreqContour.noteOn();
				VolumeContour.noteOn();
			}

			void Processor::onNoteOff(int channel, int note, float velocity)
			{
				// MIDIノートオフイベントの処理を行う

				// 押されたノートから、音程を計算
				float pitch = (440.0f * powf(2.0f, (float)(note - (69)) / 12.0));

				// pitchListを最初から検索し、pitchに合致するものを削除する
				for (int i = 0; i < (int)pitchList.size(); i++)
				{
					if (pitchList[i] == pitch)
					{
						// pitchと合致するものがあった場合、
						// 該当するデータを取り除いて検索を終了する
						pitchList.erase(pitchList.begin() + i);
						//currentPitch = pitch;
					}
				}

				if (pitchList.empty())
				{
					FreqContour.noteOff(decay);
					VolumeContour.noteOff(decay);
				}

			}
		}
	}
}