#pragma once

#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

// フィルター実装の参考（ラダーフィルタ）
// https://qiita.com/Aogiri-m2d/items/d691eb801071f4402f6a

namespace Steinberg {
    namespace Vst {
        namespace miniMoog_typeT {

            class Filter
            {
            public:
                 inline void setFreq(float frequency);
                 inline void setFs(float F);
                 inline void setEmphasis(int emp);
                 inline void setFilterModulation(bool fm);
                 inline float getFreq() { return freq; }

                 inline float process(float sample);

                Filter()
                {
                    freq = 1000.0f;
                    Fs = 44100.0f;
                    k = 3.0f;
                    omega_c = 2 * M_PI * freq;

                    b0 = omega_c / (omega_c + 1.3f * Fs);
                    b1 = (0.3f * omega_c) / (omega_c + 1.3f * Fs);
                    a1 = (0.3f * omega_c - 1.3f * Fs) / (omega_c + 1.3f * Fs);

                    x0_n0 = 0.0f;
                    x1_n0 = 0.0f;
                    x2_n0 = 0.0f;
                    x3_n0 = 0.0f;

                    x0_n1 = 0.0f;
                    x1_n1 = 0.0f;
                    x2_n1 = 0.0f;
                    x3_n1 = 0.0f;

                    y_n1 = 0.0f;
                    y_n0 = 0.0f;
                }

            protected:
                float freq;
                float Fs;
                int k;
                float omega_c;

                float x0_n0;
                float x1_n0;
                float x2_n0;
                float x3_n0;

                float x0_n1;
                float x1_n1;
                float x2_n1;
                float x3_n1;

                float y_n1;
                float y_n0;

                float b0;
                float b1;
                float a1;
            };

            //------------------定義-------------------------------------

             inline void Filter::setFreq(float frequency)
            {
                freq = frequency;

                omega_c = 2 * M_PI * freq;

                b0 = omega_c / (omega_c + 1.3f * Fs);
                b1 = (0.3f * omega_c) / (omega_c + 1.3f * Fs);
                a1 = (0.3f * omega_c - 1.3f * Fs) / (omega_c + 1.3f * Fs);
            }

             inline void Filter::setFs(float F)
            {
                Fs = F;

                omega_c = 2 * M_PI * freq;

                b0 = omega_c / (omega_c + 1.3f * Fs);
                b1 = (0.3f * omega_c) / (omega_c + 1.3f * Fs);
                a1 = (0.3f * omega_c - 1.3f * Fs) / (omega_c + 1.3f * Fs);
            }

             inline void Filter::setEmphasis(int emp)
            {
                k = emp;
            }

             inline float Filter::process(float sample)
            {

                x0_n0 = sample - k * y_n1;
                x1_n0 = tanhf(b0 * x0_n0 + b1 * x1_n1 - a1 * x1_n1);
                x2_n0 = tanhf(b0 * x1_n0 + b1 * x2_n1 - a1 * x2_n1);
                x3_n0 = tanhf(b0 * x2_n0 + b1 * x3_n1 - a1 * x3_n1);
                y_n0 = tanhf(b0 * x3_n0 + b1 * x3_n1 - a1 * y_n1);

                x0_n1 = x0_n0;
                x1_n1 = x1_n0;
                x2_n1 = x2_n0;
                x3_n1 = x3_n0;
                y_n1 = y_n0;

                return y_n0;
            }

        } // namespace miniMoog_typeT
    } // namespace Vst
} // namespace Steinberg
#pragma once

