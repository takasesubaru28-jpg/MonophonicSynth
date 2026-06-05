#pragma once
// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vsteditcontroller.h"


// 自作VST用のインクルードファイル
#include "pluginterfaces/vst/ivstmidicontrollers.h" // MIDIのPitchBendやControlChangeを扱うために必要
#include "parameter.h"
#include "pluginterfaces/base/ftypes.h"
#include "pluginterfaces/base/ustring.h"
#include <cmath>

namespace Steinberg {
	namespace Vst {
		namespace miniMoog_typeT {

			// パラメータを操作するためのControllerクラス
			class Controller : public EditController
				, public IMidiMapping
			{
			public:
				// クラスを初期化する関数
				tresult PLUGIN_API initialize(FUnknown* context);

				// 自作VST Controllerクラスのインスタンスを作成するための関数
				static FUnknown* createInstance(void*) { return (IEditController*) new Controller(); }

				IPlugView* PLUGIN_API createView(const char* name);

				// MIDIやPutchBendなどのパラメータを関連付ける
				virtual tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id);

				// VST基本インターフェースをオーバーライドした場合に必要な宣言
				OBJ_METHODS(Controller, EditController)
					DEFINE_INTERFACES
					DEF_INTERFACE(IMidiMapping)
					END_DEFINE_INTERFACES(EditController)
					REFCOUNT_METHODS(EditController)
			};

			class logParameter : public Parameter
			{
			private:
				ParamValue minFreq;
				ParamValue maxFreq;

			public:
				// コンストラクタ（継承元メンバなどの初期化）
				logParameter(const TChar *title, ParamID tag, const TChar *units = 0, ParamValue minPlain = 0.0, ParamValue maxPlain = 1.0, ParamValue defaultValuePlain = 0.0,int32 flags = ParameterInfo::kCanAutomate, UnitID unitID = kRootUnitId);

				// 正規化された値を表示値の文字列にする
				virtual void toString(ParamValue valueNormalized, String128 string) const;

				// 表示されている文字列から正規化された値を取得する関数
				virtual bool fromString(const TChar* string, ParamValue& valueNormalized) const;

				// 正規化された値から表示されている値に変換する関数
				virtual ParamValue toPlain(ParamValue valueNormalized) const;


				// 表示されている値から正規化された値にする関数
				virtual ParamValue toNormalized(ParamValue plainValue) const;

				// Parameterクラスの各種クラスを自作パラメータクラスに置き換えるマクロ
				OBJ_METHODS(logParameter, Parameter)
			};

			class expParameter : public Parameter
			{
			private:
				ParamValue minTime;
				ParamValue maxTime;

			public:
				// コンストラクタ（継承元メンバなどの初期化）
				expParameter(const TChar* title, ParamID tag, const TChar* units = 0, ParamValue minPlain = 0.0, ParamValue maxPlain = 1.0, ParamValue defaultValuePlain = 0.0, int32 flags = ParameterInfo::kCanAutomate, UnitID unitID = kRootUnitId);

				// 正規化された値を表示値の文字列にする
				virtual void toString(ParamValue valueNormalized, String128 string) const;

				// 表示されている文字列から正規化された値を取得する関数
				virtual bool fromString(const TChar* string, ParamValue& valueNormalized) const;

				// 正規化された値から表示されている値に変換する関数
				virtual ParamValue toPlain(ParamValue valueNormalized) const;


				// 表示されている値から正規化された値にする関数
				virtual ParamValue toNormalized(ParamValue plainValue) const;

				// Parameterクラスの各種クラスを自作パラメータクラスに置き換えるマクロ
				OBJ_METHODS(expParameter, Parameter)
			};
		}
	}
}