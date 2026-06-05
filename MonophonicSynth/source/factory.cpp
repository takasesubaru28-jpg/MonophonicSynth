// VST3 SDKのインクルードファイル
#include "public.sdk/source/main/pluginfactory.h"

// 自作VSTのヘッダファイルをインクルード
#include "fuid.h"
#include "processor.h"
#include "controller.h"

// 製作者(製作会社)の名前。終端文字「\0」含めて64文字まで。
#define MYVST_VENDOR   "takaseS"

// 製作者(製作会社)のWebサイトのURL。終端文字「\0」含めて256文字まで。
#define MYVST_URL      ""

// 製作者(製作会社)の連絡先メールアドレス。終端文字「\0」含めて128文字まで。
#define MYVST_EMAIL    ""

// 自作するVSTの名前。終端文字「\0」含めて64文字まで。
#define MYVST_VSTNAME  "miniMoog_modelT"

// 自作するVSTのバージョン。終端文字「\0」含めて64文字まで。
#define MYVST_VERSION  "0" 

// 自作するVSTのカテゴリ。終端文字「\0」含めて64文字まで。
#define MYVST_SUBCATEGORIES Vst::PlugType::kInstrument


// ===================================================================================
// DLLファイルの初期化、終了処理関数
// ===================================================================================
// 基本的に何もする必要はない。(VST SDK 3.7.1より前のバージョンの場合はコメントを外すこと)
//bool InitModule() { return true; }
//bool DeinitModule() { return true; }


// ===================================================================================
// 自作VSTプラグインの生成
// ===================================================================================
BEGIN_FACTORY_DEF(MYVST_VENDOR, MYVST_URL, MYVST_EMAIL)

	// Processorクラスの作成を行う
	DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::Vst::miniMoog_typeT::ProcessorUID),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		MYVST_VSTNAME,
		Vst::kDistributable,
		MYVST_SUBCATEGORIES,
		MYVST_VERSION,
		kVstVersionString,
		Steinberg::Vst::miniMoog_typeT::Processor::createInstance)

	// Controllerクラスの作成を行う
	DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::Vst::miniMoog_typeT::ControllerUID),
		PClassInfo::kManyInstances,
		kVstComponentControllerClass,
		MYVST_VSTNAME " Controller",	// 自作VSTの名前に"Controller"を付与したもの。
		0,						    // 使わないので必ず0にする
		"",						    // 使わないので必ず""にする
		MYVST_VERSION,
		kVstVersionString,
		Steinberg::Vst::miniMoog_typeT::Controller::createInstance)

	END_FACTORY