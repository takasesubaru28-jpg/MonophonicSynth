// 自作VST用のインクルードファイル
#include "parameter.h"
#include "fuid.h"
#include "controller.h"
#include "pluginterfaces/base/ustring.h"

#include "public.sdk/source/vst/vstparameters.h"

#include "vstgui/plugin-bindings/vst3editor.h"
#include <string_view> // これを冒頭に追加
#include <cstring>           // strcmp を使うために必要
#include "vstgui/lib/vstguibase.h"
#include <windows.h> 

namespace Steinberg {
	namespace Vst {
		namespace miniMoog_typeT {
			// クラスを初期化する関数
			tresult PLUGIN_API Controller::initialize(FUnknown* context)
			{
				// まず継承元クラスの初期化
				tresult result = EditController::initialize(context);
				if (result == kResultTrue)
				{
					//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					// 範囲パラメーターを作成
					// tuneノブ
					RangeParameter* tune = new RangeParameter(STR16("tune"), TUNE_Osc1, STR16("semitone"), -1.0f, 1.0f, 0.0f);
					tune->setPrecision(2); // 小数第何位まで表示するか
					// 範囲パラメーターをコントローラーに追加
					parameters.addParameter(tune);

					// range_osc1
					auto* range_osc1 = new StringListParameter(USTRING("range_osc1"), RANGE_Osc1);
					range_osc1->appendString(USTRING("LO"));
					range_osc1->appendString(USTRING("32'"));
					range_osc1->appendString(USTRING("16'"));
					range_osc1->appendString(USTRING("8'"));
					range_osc1->appendString(USTRING("4'"));
					range_osc1->appendString(USTRING("2'"));
					range_osc1->setNormalized(3.0f / 5.0f);
					parameters.addParameter(range_osc1);

					// osc1の波形選択
					auto* osc1Param = new StringListParameter(USTRING("waveform_osc1"), WAVEFORM_Osc1);
					osc1Param->appendString(USTRING("tri"));
					osc1Param->appendString(USTRING("tri_saw"));
					osc1Param->appendString(USTRING("saw"));
					osc1Param->appendString(USTRING("pulse"));
					osc1Param->appendString(USTRING("pulse_wide"));
					osc1Param->appendString(USTRING("pulse_narrow"));
					parameters.addParameter(osc1Param);

					//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

					// freq_osc2ノブ
					RangeParameter* freq_osc2 = new RangeParameter(STR16("freq_osc2"), FREQUENCY_Osc2, STR16("semitone"), -7.0f, 7.0f, 0.0f);
					freq_osc2->setPrecision(2); // 小数第何位まで表示するか
					// 範囲パラメーターをコントローラーに追加
					parameters.addParameter(freq_osc2);

					// range_osc2
					auto* range_osc2 = new StringListParameter(USTRING("range_osc2"), RANGE_Osc2);
					range_osc2->appendString(USTRING("LO"));
					range_osc2->appendString(USTRING("32'"));
					range_osc2->appendString(USTRING("16'"));
					range_osc2->appendString(USTRING("8'"));
					range_osc2->appendString(USTRING("4'"));
					range_osc2->appendString(USTRING("2'"));
					range_osc2->setNormalized(3.0f / 5.0f);
					parameters.addParameter(range_osc2);

					// osc2の波形選択
					auto* osc2Param = new StringListParameter(USTRING("waveform_osc2"), WAVEFORM_Osc2);
					osc2Param->appendString(USTRING("tri"));
					osc2Param->appendString(USTRING("tri_saw"));
					osc2Param->appendString(USTRING("saw"));
					osc2Param->appendString(USTRING("pulse"));
					osc2Param->appendString(USTRING("pulse_wide"));
					osc2Param->appendString(USTRING("pulse_narrow"));
					parameters.addParameter(osc2Param);

					//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

					// freq_osc3ノブ
					RangeParameter* freq_osc3 = new RangeParameter(STR16("freq_oscs3"), FREQUENCY_Osc3, STR16("semitone"), -7.0f, 7.0f, 0.0f);
					freq_osc3->setPrecision(2); // 小数第何位まで表示するか
					// 範囲パラメーターをコントローラーに追加
					parameters.addParameter(freq_osc3);

					// range_osc3
					auto* range_osc3 = new StringListParameter(USTRING("range_osc3"), RANGE_Osc3);
					range_osc3->appendString(USTRING("LO"));
					range_osc3->appendString(USTRING("32'"));
					range_osc3->appendString(USTRING("16'"));
					range_osc3->appendString(USTRING("8'"));
					range_osc3->appendString(USTRING("4'"));
					range_osc3->appendString(USTRING("2'"));
					range_osc3->setNormalized(3.0f / 5.0f);
					parameters.addParameter(range_osc3);

					// osc3の波形選択
					auto* osc3Param = new StringListParameter(USTRING("waveform_osc3"), WAVEFORM_Osc3);
					osc3Param->appendString(USTRING("tri"));
					osc3Param->appendString(USTRING("rev_saw"));
					osc3Param->appendString(USTRING("saw"));
					osc3Param->appendString(USTRING("pulse"));
					osc3Param->appendString(USTRING("pulse_wide"));
					osc3Param->appendString(USTRING("pulse_narrow"));
					parameters.addParameter(osc3Param);

					// osc3コントロールノブ
					auto* osc3Ctrl = new RangeParameter(STR16("control_osc3"), CONTROL_Osc3, STR16(""), 0.f, 1.f, 1.f,1);
					osc3Ctrl->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(osc3Ctrl);

					// オシレータ1,2をオシレータ3，ノイズ，フィルター，コンター，外部モジュレーションソース，LFOでモジュレーションさせることができる
					auto* mod_osc = new RangeParameter(STR16("modulator_osc"), OSILLATORMODULATION, STR16(""), 0.f, 1.f, 1.f, 1);
					mod_osc->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(mod_osc);

					//--------------------------------------------------------------------controllers----------------------------------------------------------------------------------------------------------------------------------

					auto* osc3_filter = new RangeParameter(STR16("osc3_filterEG"), OSC3_FILTER, STR16(""), 0.f, 1.f, 1.f, 1);
					osc3_filter->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(osc3_filter);

					auto* noise_lfo = new RangeParameter(STR16("noise_lfo"), NOISE_LFO, STR16(""), 0.f, 1.f, 1.f, 1);
					noise_lfo->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(noise_lfo);

					auto* modulation_mix = new RangeParameter(STR16("modulation_mix"), MODULATIONMIX, STR16(""), 0.0f, 10.0f, 5.0f);
					modulation_mix ->setPrecision(2);
					parameters.addParameter(modulation_mix);

					parameters.addParameter(STR16("glide"), nullptr, 0, 0.5, ParameterInfo::kCanAutomate, GLIDE_VAL);

					//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					parameters.addParameter(STR16("volume_osc1"), nullptr, 0, 1, ParameterInfo::kCanAutomate, VOLUME_OSC1);
					parameters.addParameter(STR16("volume_osc2"), nullptr, 0, 1, ParameterInfo::kCanAutomate, VOLUME_OSC2);
					parameters.addParameter(STR16("volume_osc3"), nullptr, 0, 1, ParameterInfo::kCanAutomate, VOLUME_OSC3);

					parameters.addParameter(STR16("noise_volume"), nullptr, 0, 0, ParameterInfo::kCanAutomate, NOISEVOLUME);

					// ノイズの種類選択
					auto* noise = new StringListParameter(USTRING("noise"), WHITEPINK);
					noise->appendString(USTRING("white"));
					noise->appendString(USTRING("pink"));
					noise->setNormalized(0.0);
					parameters.addParameter(noise);
					//------------------------------------------------------------modifiere------------------------------------------------------------------------------------------------------------------------------------------

					auto* emphasis = new RangeParameter(STR16("emphasis"), EMPHASIS, STR16(""), 0.0f, 10.0f, 3.0f);
					emphasis->setPrecision(2);
					parameters.addParameter(emphasis);

					logParameter* cut_freq = new logParameter(STR16("cutoff_freq"), CUTOFFFREQUENCY, STR16("Hz"), 10.0f, 32000.0f, 1000.0f);
					cut_freq->setPrecision(2);
					parameters.addParameter(cut_freq);

					auto* amount_contour = new RangeParameter(STR16("amount of contour"), AMOUNTOFCONTOUR, STR16(""), 0.0f, 10.0f, 5.0f);
					amount_contour->setPrecision(2);
					parameters.addParameter(amount_contour);

					expParameter* attack_time_freq = new expParameter(STR16("attack time_freq"), ATTACKTIME_FREQ, STR16("msec"), 1.0f, 10000.0f, 500.0f);
					attack_time_freq->setPrecision(4);
					parameters.addParameter(attack_time_freq);

					expParameter* decay_time_freq = new expParameter(STR16("decay time_freq"), DECAYTIME_FREQ, STR16("msec"), 1.0f, 10000.0f, 500.0f);
					decay_time_freq->setPrecision(4);
					parameters.addParameter(decay_time_freq);

					auto* sustain_level_freq = new RangeParameter(STR16("sustain level_freq"), SUSTAINLEVEL_FREQ, STR16(""), 0.0f, 10.0f, 5.0f);
					sustain_level_freq->setPrecision(2);
					parameters.addParameter(sustain_level_freq);

					expParameter* attack_time_vol = new expParameter(STR16("attack time_vol"), ATTACKTIME_VOL, STR16("msec"), 1.0f, 10000.0f, 500.0f);
					attack_time_vol->setPrecision(4);
					parameters.addParameter(attack_time_vol);

					expParameter* decay_time_vol = new expParameter(STR16("decay time_vol"), DECAYTIME_VOL, STR16("msec"), 1.0f, 10000.0f, 500.0f);
					decay_time_vol->setPrecision(4);
					parameters.addParameter(decay_time_vol);

					auto* sustain_level_vol = new RangeParameter(STR16("sustain level_vol"), SUSTAINLEVEL_VOL, STR16(""), 0.0f, 10.0f, 5.0f);
					sustain_level_vol->setPrecision(2);
					parameters.addParameter(sustain_level_vol);

					auto*  filter_modifier = new RangeParameter(STR16("filter_modulation"), FILTERMODULATION, STR16(""), 0.f, 1.f, 1.f, 1);
					filter_modifier->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(filter_modifier);
					
					// ----------------------------------パラメータを追加------------------------------------------------------------------------------------------------------------------------------------------------
					// ボリューム
					parameters.addParameter(STR16("volume"), nullptr, 0, 0.5, ParameterInfo::kCanAutomate, VOLUME);

					//-------------------------------------------------leftHand------------------------------------------------------------------------------------------------------------------------------------------
					logParameter* lfo_freq = new logParameter(STR16("lfo_freq"), LFORATE, STR16("Hz"), 0.1f, 500.0f, 10.0f);
					lfo_freq->setPrecision(2);
					parameters.addParameter(lfo_freq);

					auto* glide_param = new RangeParameter(STR16("glide"), GLIDE, STR16(""), 0.f, 1.f, 1.f, 1);
					glide_param->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(glide_param);

					auto* decay_param = new RangeParameter(STR16("decay_param"), DECAY, STR16(""), 0.f, 1.f, 1.f, 1);
					decay_param->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(decay_param);

					auto* pushpull_param = new RangeParameter(STR16("lfo_wavetype"), PUSHPULL, STR16(""), 0.f, 1.f, 1.f, 1);
					pushpull_param->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(pushpull_param);

					auto* key_ctrl1 = new RangeParameter(STR16("key_ctrl1"), KYBD_TRACK_1, STR16(""), 0.f, 1.f, 1.f, 1);
					key_ctrl1->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(key_ctrl1);

					auto* key_ctrl2 = new RangeParameter(STR16("key_ctrl2"), KYBD_TRACK_2, STR16(""), 0.f, 1.f, 1.f, 1);
					key_ctrl2->setPrecision(0); // 0 or 1 の値だけになる
					parameters.addParameter(key_ctrl2);
					//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


				}
				result = kResultTrue;
				return result;
			}

			tresult PLUGIN_API Controller::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id)
			{
				// midiControllerNumberに関係づけたいパラメータIDをidに代入することでMIDIコントロールチェンジとパラメータを関連付ける
				switch (midiControllerNumber)
				{
				case kCtrlVolume:
					id = VOLUME;
					return kResultTrue;
				}

				return kResultFalse;
			}

			//--------------------------------logParameterの定義----------------------------------------------

			logParameter::logParameter(const TChar* title, ParamID tag, const TChar* units, ParamValue minPlain, ParamValue maxPlain, ParamValue defaultValuePlain, int32 flags, UnitID unitID)
				:Parameter(title, tag, units, 0.0f, 0, flags, unitID)
			{
				minFreq = minPlain;
				maxFreq = maxPlain;

				valueNormalized = toNormalized(defaultValuePlain);
			}

			void logParameter::toString(ParamValue valueNormalized, String128 string) const
			{
				UString128 wrapper;

				wrapper.printFloat(toPlain(valueNormalized), precision);

				wrapper.copyTo(string, 128);
			}

			bool logParameter::fromString(const TChar* string, ParamValue& valueNormalized) const
			{
				UString wrapper((TChar*)string, strlen16(string));

				ParamValue plainvalue;
				if (wrapper.scanFloat(plainvalue))
				{
					valueNormalized = toNormalized(valueNormalized);

					return true;
				}
				return false;
			}

			ParamValue logParameter::toPlain(ParamValue valueNormalized) const
			{
				return minFreq * pow(maxFreq / minFreq, valueNormalized);
			}

			ParamValue logParameter::toNormalized(ParamValue plainValue) const
			{
				return log(plainValue / minFreq) / log(maxFreq / minFreq);
			}

			//--------------------------------expParameterの定義----------------------------------------------

			expParameter::expParameter(const TChar* title, ParamID tag, const TChar* units, ParamValue minPlain, ParamValue maxPlain, ParamValue defaultValuePlain, int32 flags, UnitID unitID)
				:Parameter(title, tag, units, 0.0f, 0, flags, unitID)
			{
				minTime = minPlain;
				maxTime = maxPlain;

				valueNormalized = toNormalized(defaultValuePlain);
			}

			void expParameter::toString(ParamValue valueNormalized, String128 string) const
			{
				UString128 wrapper;

				wrapper.printFloat(toPlain(valueNormalized), precision);

				wrapper.copyTo(string, 128);
			}

			bool expParameter::fromString(const TChar* string, ParamValue& valueNormalized) const
			{
				UString wrapper((TChar*)string, strlen16(string));

				ParamValue plainvalue;
				if (wrapper.scanFloat(plainvalue))
				{
					valueNormalized = toNormalized(valueNormalized);

					return true;
				}
				return false;
			}

			ParamValue expParameter::toPlain(ParamValue valueNormalized) const
			{
				return maxTime * powf(minTime / maxTime, 1 - valueNormalized);
			}

			ParamValue expParameter::toNormalized(ParamValue plainValue) const
			{
				return 1.0f - logf(plainValue / maxTime) / logf(minTime / maxTime);
			}

			IPlugView* PLUGIN_API Controller::createView(const char* name)
			{
				// strcmp を使うために <cstring> が必要
				if (strcmp(name, ViewType::kEditor) == 0)
				{
					// "view" は design.uidesc 内のテンプレート名
					// "design.uidesc" は .rc ファイルで定義した名前
					return new VSTGUI::VST3Editor(this, "view", "design.uidesc");
				}
				return nullptr;
			}


		}
	}
}