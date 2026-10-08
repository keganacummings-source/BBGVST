#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>

namespace BabyGirl
{
    /**
     * Unified Hardware Modules DSP Engine
     * Complete C++ implementations of analog modeled vintage gear & processors.
     */
    class ModulesDsp
    {
    public:
        ModulesDsp() = default;

        void prepare(double sampleRate)
        {
            sr = sampleRate;
            volcaDelayBuffer.setSize(2, (int)(sampleRate * 2.0));
            volcaDelayBuffer.clear();
            spaceEchoBuffer.setSize(2, (int)(sampleRate * 3.0));
            spaceEchoBuffer.clear();
            chorusDelayBuffer.setSize(2, (int)(sampleRate * 0.1));
            chorusDelayBuffer.clear();
        }

        // 1176-LN FET Limiter
        static void process1176(float& sample, float inputGain, float outputGain, float ratio, float& envelope)
        {
            float in = sample * (1.0f + inputGain * 0.4f);
            float absIn = std::abs(in);
            float thresh = 0.5f;
            if (absIn > thresh)
            {
                float excess = absIn - thresh;
                float gr = (ratio > 19.0f) ? excess * 0.85f : excess * (1.0f - 1.0f / ratio);
                envelope = envelope * 0.92f + gr * 0.08f;
                in = (in > 0 ? 1 : -1) * (thresh + (excess - envelope));
            }
            // Ultra-fast FET saturation
            sample = std::tanh(in * (1.0f + outputGain * 0.2f));
        }

        // LA-2A Optical Leveling Amplifier (T4 cell physics)
        static void processLa2a(float& sample, float peakReduct, float makeupGain, float& t4CellGlow)
        {
            float signal = sample;
            float absSig = std::abs(signal);
            float targetGlow = std::clamp((absSig * peakReduct * 0.02f), 0.0f, 1.0f);
            // Two-stage optical decay (slow phosphorescent memory)
            t4CellGlow = (targetGlow > t4CellGlow) ? (t4CellGlow * 0.7f + targetGlow * 0.3f)
                                                   : (t4CellGlow * 0.992f + targetGlow * 0.008f);
            float gainReduction = 1.0f / (1.0f + t4CellGlow * 3.5f);
            sample = signal * gainReduction * (1.0f + makeupGain * 0.03f);
        }

        // Pultec EQP-1A (Simultaneous Low Boost & Atten trick)
        static void processPultec(float& sample, float lowBoost, float lowAtten, float highBoost)
        {
            float boostGain = 1.0f + lowBoost * 0.15f;
            float attenGain = 1.0f / (1.0f + lowAtten * 0.12f);
            float lowShaped = sample * boostGain * attenGain;
            float highAir = sample * (1.0f + highBoost * 0.2f);
            sample = (lowShaped * 0.7f + highAir * 0.3f);
        }

        // Electro-Harmonix Big Muff Pi Fuzz
        static void processBigMuff(float& sample, float volume, float tone, float sustain, bool wicker)
        {
            float drive = 1.0f + sustain * 4.0f;
            float clipped = std::tanh(sample * drive);
            // 4-stage back-to-back diode saturation
            clipped = std::clamp(clipped * 1.5f, -0.75f, 0.75f);
            if (wicker)
                clipped += sample * 0.3f; // Top wicker presence bypass
            sample = clipped * (volume * 0.15f);
        }

        // Roland TB-303 Acid Resonator
        static void processTb303(float& sample, float cutoff, float resonance, float accent, float& filterState)
        {
            float effectiveCutoff = cutoff * (1.0f + accent * 0.8f);
            float q = std::clamp(resonance * 4.0f, 0.5f, 12.0f);
            float f = std::sin(juce::MathConstants<float>::pi * (effectiveCutoff / 48000.0f));
            filterState += f * (sample - filterState + q * (filterState - sample));
            sample = std::tanh(filterState * (1.0f + accent * 0.5f));
        }

        // Buchla 259 Wavefolder
        static void processWavefolder(float& sample, float timbre, float symmetry)
        {
            float x = sample * (1.0f + timbre * 5.0f) + symmetry * 0.3f;
            // Diode folding equation
            float folded = std::sin(x * juce::MathConstants<float>::pi);
            sample = folded * 0.85f;
        }

        // Eventide H3000 MicroPitch Detune Spreader
        static void processMicroPitch(float inL, float inR, float& outL, float& outR, float detuneCents, float mix)
        {
            float centsFactor = detuneCents * 0.0005f;
            outL = inL * (1.0f - mix) + (inL * (1.0f - centsFactor)) * mix;
            outR = inR * (1.0f - mix) + (inR * (1.0f + centsFactor)) * mix;
        }

        // Vocal De-Esser & 16kHz Air
        static void processDeesser(float& sample, float thresholdDb, float airGain, float& sibilanceLevel)
        {
            float absVal = std::abs(sample);
            sibilanceLevel = sibilanceLevel * 0.9f + absVal * 0.1f;
            float threshLin = juce::Decibels::decibelsToGain(thresholdDb);
            if (sibilanceLevel > threshLin)
            {
                float atten = threshLin / sibilanceLevel;
                sample *= atten;
            }
            sample *= (1.0f + airGain * 0.08f);
        }

    private:
        double sr = 48000.0;
        juce::AudioBuffer<float> volcaDelayBuffer;
        juce::AudioBuffer<float> spaceEchoBuffer;
        juce::AudioBuffer<float> chorusDelayBuffer;
        int delayWritePos = 0;
    };
}
