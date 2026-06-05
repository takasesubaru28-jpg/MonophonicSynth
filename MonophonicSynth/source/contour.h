#pragma once

#include <cmath>
#include <algorithm>

// フィルター実装の参考（ラダーフィルタ）
// https://qiita.com/Aogiri-m2d/items/d691eb801071f4402f6a

namespace Steinberg {
    namespace Vst {
        namespace miniMoog_typeT {

            class Contour
            {
            public:
                inline float process(bool vol_);
                inline void setAttack(float time);
                inline void setDecay(float time);
                inline void setAmount(float amount);
                inline void setSustain(float sustain);
                inline void setFs(float samplingRate);
                inline void setOrg(float param);
                inline void noteOn();
                inline void noteOff(bool decayEnabled);
                inline float getCurrentValue();

                Contour()
                {
                    state = State::Idle;

                    attackTime = 5.0f;
                    decayTime = 5.0f;
                    sustainLevel = 1.0f;
                    amountContour = 20000.0f;
                    Fs = 44100.0f;
                    tmp = 0.0f;
                }

            protected:
                enum class State { Idle, Attack, Decay, Sustain, Release };

                State state;
                float attackTime;
                float decayTime;
                float sustainLevel;
                float amountContour;
                float Fs;
                float tmp;
                float org_param;
            };

            //------------------定義-------------------------------------

            inline void Contour::setAttack(float time)
            {
                attackTime = time;
            }

            inline void Contour::setDecay(float time)
            {
                decayTime = time;
            }

            inline void Contour::setAmount(float amount)
            {
                amountContour = amount;
            }

            inline void Contour::setSustain(float sustain)
            {
                sustainLevel = sustain;
            }

            inline void Contour::setFs(float samplingRate)
            {
                Fs = samplingRate;
            }

            inline void Contour::setOrg(float param)
            {
                org_param = param;
            }

            inline void Contour::noteOn()
            {
                state = State::Attack;
                //tmp = 0.0f;
            }

            inline void Contour::noteOff(bool decayEnabled)
            {
                // スイッチがONなら、現在の音量から徐々に減衰（Release）へ
                if (decayEnabled && tmp > 0.0f)
                {
                    state = State::Release;
                }
                // スイッチがOFFなら、即座に消音
                else
                {
                    state = State::Idle;
                    tmp = 0.0f;
                }
            }

            inline float Contour::getCurrentValue()
            {
                // 現在の tmp (0.0 〜 1.0) の状態をそのまま返す
                // ※ フィルター用なら tmp * amountContour + org_param ですが、
                // モジュレーション源として使う場合は 0.0〜1.0 の raw 値が扱いやすいです
                return tmp;
            }


            inline float Contour::process(bool vol_)
            {
                switch (state)
                {
                case State::Attack:
                    tmp += 1.0f / (attackTime * Fs);
                    if (tmp >= 1.0f)
                    {
                        tmp = 1.0f;
                        state = State::Decay;
                    }
                    break;

                case State::Decay:
                    tmp -= (1.0f - sustainLevel) / (decayTime * Fs);
                    if (tmp <= sustainLevel)
                    {
                        tmp = sustainLevel;
                        state = State::Sustain;
                    }
                    break;

                case State::Sustain:
                    tmp = sustainLevel; 
                    break;

                case State::Release:
                    tmp -= tmp / (decayTime * Fs);
                    if (tmp <= 0.0f)
                    {
                        tmp = 0.0f;
                        state = State::Idle;
                    }
                    break;

                case State::Idle:
                    tmp = 0.0f;
                    return 0.0f;
                }
                if (vol_)
                {
                    return tmp * org_param;
                }
                else
                {
                    return tmp * amountContour + org_param;
                }
            }

        } // namespace miniMoog_typeT
    } // namespace Vst
} // namespace Steinberg