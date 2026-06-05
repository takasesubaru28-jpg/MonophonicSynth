# Monophonic Synth🎹
モノフォニックシンセサイザーをVSTとして実装する．

## 概要
本プロジェクトは，単純な信号処理を用いてシンセサイザーを実装しています．
実装にあたり以下のサイトを参考にしました．<br>
うつぼっくす（C++でVST作り）https://www.utsbox.com/?page_id=1316 <br>
KORG社MiniMoogマニュアル https://www.korg-kid.com/moog/wp-content/uploads/2022/12/Minimoog-Model-D-Manual_JP_web.pdf

## 環境
Windows11，Cubase13Proで動作確認済みです．

## ディレクトリ構成
```text
MonophonicSynth/           # VST用ソースコード
├── vst3sdk/              # vst3sdkを置いておく
├── source/               # VST用のソースコードが入っています
├── README.md
└── .gitignore
MonophonicSynth.vst3/contents/  # VST本体
```

## プラグイン本体
NeuralAmpTorch.vst3を自身のvst3ファイルが認識されるフォルダに丸ごと移動して下さい

# 実行手順 (Usage)

本リポジトリでは、GitHubの容量制限（100MB）を回避しつつ、効率的に学習を行うためのワークフローを採用しています。


## VSTの作成（NeuralAmpTorch）
### 0．ライブラリ配置
#### 外部ライブラリの配置
本リポジトリにはライブラリ本体は含まれていません。以下の構成になるように各自で配置してください。

1. **VST3 SDK**:
   - [Steinberg公式サイト](https://www.steinberg.net/developers/)からダウンロードし、`NeuralAmpTorch/vst3sdk/` に配置します。


### 1．環境構築
```bash
# VisualStudioで（NeuralAmpTorchフォルダを開いて以下のコマンドを打ってください
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
```

### 2．ビルド
Releaseでビルドをしてください．.vst3ファイルが生成されます．