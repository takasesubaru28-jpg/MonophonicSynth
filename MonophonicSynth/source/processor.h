// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "oscillator.h"
#include "noise.h"
#include "contour.h"
#include "filter.h"
#include <cmath>
#include <vector> 

using namespace std;

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {
		namespace miniMoog_typeT {

			// ===================================================================================
			// 音声信号を処理するProcessorクラス
			// ===================================================================================
			class Processor : public AudioEffect
			{
			protected:
				// 押されたキーの音程(周波数)を保存する可変長配列
				vector<float> pitchList; 
				float normalizedVelocity;

				float mixed;
				float filter_mod;
				float currentPitch;

				// オシレータ
				Oscillator<float> osc1;
				Oscillator<float> osc2;
				Oscillator<float> osc3;

				// オシレータ用パラメータ
				ParamValue tune;
				ParamValue tune_Osc1;
				ParamValue range_Osc1;
				WaveType waveform_Osc1;

				ParamValue frequency_Osc2;
				ParamValue range_Osc2;
				WaveType waveform_Osc2;

				ParamValue frequency_Osc3;
				ParamValue range_Osc3;
				WaveType waveform_Osc3;
				bool controll_Osc3;

				ParamValue osillatorModulation;

				// Controllers
				bool osc3Filter;
				bool noiseLfo;
				ParamValue modulationMix;
				float glide_val;
				float targetPitch;    // 次に到達すべきピッチ
				float phase_glide;    // グライドの進行度、または現在の計算用ピッチ
				float glideRate;

				// MIxer&Noize
				ParamValue volume1;
				ParamValue volume2;
				ParamValue volume3;
				ParamValue volume_noise;
				bool white;
				Noise<float> noise;

				// Modifires
				Filter filter;
				float emphasis;
				float cut_freq;

				ParamValue amountContour_freq;
				ParamValue attackTime_freq;
				ParamValue decayTime_freq;
				ParamValue sustainLevel_freq;

				ParamValue attackTime_vol;
				ParamValue decayTime_vol;
				ParamValue sustainLevel_vol;

				bool filterModulation;

				bool kybdTrack1;
				bool kybdTrack2;


				float loud_mod;

				Contour VolumeContour;
				Contour FreqContour;

				// アウトプットセクションパラメータ
				ParamValue volume;
				ParamValue mainOutput;

				// LeftHandKeboardPanel
				ParamValue lfoRate;
				bool glide;
				bool decay;
				WaveType push_pull;
				Oscillator<float> lfo;

				float modWheel;
				float pitchBend;

			public:
				// コンストラクタ
				Processor();

				// クラスを初期化する関数(必須)
				tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;

				// バス構成を設定する関数。
				tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;

				// midiノートオンオフイベント
				virtual void onNoteOn(int channel, int note, float velocity);
				virtual void onNoteOff(int channel, int note, float velocity);

				// 音声信号を処理する関数(必須)
				tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;

				// 自作VST Processorクラスのインスタンスを作成するための関数(必須)
				static FUnknown* createInstance(void*) { return (IAudioProcessor*)new Processor(); }
			};


		}
	}
} // namespace SteinbergとVstの終わり