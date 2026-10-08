#pragma once
#include "DreamShare.h"
#include <vector>

namespace BabyGirl
{
    inline std::vector<DreamPreset> getFactoryPresets()
    {
        return {
            {
                "babygirl-main",
                "BabyGirl Signature",
                "Kegana Cummings",
                "Producer Lead",
                "Warm, Tape, Tube, Master",
                4.2f, 1850.0f, 0.48f, 0.36f, 0.32f, 0.25f, 0.28f, 0.35f, 1.25f, 0.45f
            },
            {
                "kyoto-shrine-ambient",
                "Kyoto Shrine Ghost",
                "KyotoSpxrit",
                "Lofi Ambient",
                "Vinyl, Cassette, Shimmer",
                2.5f, 1400.0f, 0.35f, 0.52f, 0.65f, 0.55f, 0.60f, 0.75f, 1.45f, 0.28f
            },
            {
                "hush-acid-furnace",
                "Hush Acid Furnace",
                "Hush Core",
                "Bass / Acid",
                "Pentode, Resonant, Driven",
                8.2f, 950.0f, 0.84f, 0.16f, 0.10f, 0.05f, 0.08f, 0.15f, 1.05f, 0.70f
            },
            {
                "master-final-polish",
                "Final Polish Mastering Suite",
                "Master Labs",
                "Mastering",
                "Multiband, TruePeak, Iron",
                1.8f, 14000.0f, 0.15f, 0.10f, 0.08f, 0.00f, 0.00f, 0.12f, 1.35f, 0.55f
            },
            {
                "dreamshare-celestial",
                "DreamShare Celestial Cloud",
                "DreamShare Collective",
                "Ether Pad",
                "Vast, Modulated, Shimmer",
                2.5f, 2800.0f, 0.52f, 0.48f, 0.45f, 0.30f, 0.35f, 0.75f, 1.60f, 0.35f
            }
        };
    }
}
