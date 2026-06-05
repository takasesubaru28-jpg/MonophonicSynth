#pragma once

#include "pluginterfaces/base/ftypes.h"
#include <cstdlib>
#include <cmath>

namespace Steinberg {
    namespace Vst {
        namespace MonophonicSynth {

            template<class SamplePrecision>
            class Noise
            {
            public:
                Noise(bool white = true)
                    : white(white), b0(0), b1(0), b2(0) {}

                // ノイズタイプを切り替え
                void setWhite(bool white_) { white = white_; }

                // 毎サンプル呼び出してノイズ生成
                SamplePrecision process()
                {
                    if (white)
                        return whiteNoise();
                    else
                        return pinkNoise();
                }

            private:
                bool white;
                // ピンクノイズ用フィルタ係数
                SamplePrecision b0, b1, b2;

                // ホワイトノイズ生成
                SamplePrecision whiteNoise()
                {
                    return static_cast<SamplePrecision>(2.0f * rand() / RAND_MAX - 1.0f);
                }

                // ピンクノイズ（Paul Kellet法）
                SamplePrecision pinkNoise()
                {
                    SamplePrecision white = whiteNoise();

                    b0 = static_cast<SamplePrecision>(0.99765 * b0 + white * 0.0990460);
                    b1 = static_cast<SamplePrecision>(0.96300 * b1 + white * 0.2965164);
                    b2 = static_cast<SamplePrecision>(0.57000 * b2 + white * 1.0526913);

                    SamplePrecision pink = static_cast<SamplePrecision>(b0 + b1 + b2 + white * 0.1848);
                    return pink * static_cast<SamplePrecision>(0.05 / 0.3); // 音量調整
                }
            };

        } // namespace MonophonicSynth
    } // namespace Vst
} // namespace Steinberg
