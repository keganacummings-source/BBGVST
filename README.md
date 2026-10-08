# BabyGirl.vst — VST3 & Standalone Emulator

**BabyGirl** is a unified analog-modeled synthesizer and DSP console built with JUCE.  
Push this repo to GitHub and the Actions CI builds `BabyGirl.vst3` + `BabyGirl.exe/.app` automatically for Windows, macOS, and Linux.

---

## Engines

| Stage | What it does |
|---|---|
| **Hush** | Vacuum tube preamp (triode/pentode), 4-pole 24dB ladder filter, reel-to-reel tape delay with wow/flutter |
| **KyotoSpxrit** | Vinyl crackle & dust, lofi bitcrush, tape warp drift, Kyoto shrine shimmer reverb |
| **Master** | Mid/Side stereo imager, transformer iron saturation, brickwall true-peak limiter, LUFS metering |
| **Modules DSP** | 1176-LN FET limiter, LA-2A optical leveler, Pultec EQP-1A, Big Muff Pi, TB-303 resonator, Buchla wavefolder, de-esser |
| **DreamShare** | Cloud preset catalog via Cloudflare Worker (`worker/`) — fetch, publish, Discord webhook |

---

## Build on GitHub (Automatic — no local setup needed)

1. Create a new GitHub repository
2. Push this entire folder to the `main` branch
3. Go to **Actions** tab → the `Build BabyGirl VST3 & Standalone Emulator` workflow runs automatically
4. When it finishes, download your builds from the **Artifacts** section:
   - `BabyGirl-Windows-VST3-and-Exe` → `BabyGirl.vst3` + `BabyGirl.exe`
   - `BabyGirl-macOS-VST3-and-App` → `BabyGirl.vst3` + `BabyGirl.app`
   - `BabyGirl-Linux-VST3-and-Standalone` → `BabyGirl.vst3` + standalone binary

> **Tag a release** (`git tag v1.0.0 && git push --tags`) to also publish the Windows build as a GitHub Release with a public download link.

---

## Local Build (CMake)

**Prerequisites:** CMake 3.22+, a C++20 compiler, and on Linux the packages listed in the workflow.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

Outputs land in `build/BabyGirl_artefacts/Release/`.

---

## DAW Installation

| Platform | Copy `BabyGirl.vst3` to |
|---|---|
| Windows | `C:\Program Files\Common Files\VST3\` |
| macOS | `/Library/Audio/Plug-Ins/VST3/` |
| Linux | `~/.vst3/` or `/usr/lib/vst3/` |

Restart your DAW and scan for new plugins.

---

## DreamShare Cloud Presets (optional)

The `worker/` folder contains a Cloudflare Worker for the DreamAPI backend.

```bash
cd worker
npm install
npx wrangler deploy
```

Set the deployed Worker URL as `BABYGIRL_DREAM_API_URL` in your DAW environment or hardcode it in `Source/DreamApi.h`.  
Add `DISCORD_WEBHOOK_URL` as a Wrangler secret to get Discord notifications on new preset posts:

```bash
npx wrangler secret put DISCORD_WEBHOOK_URL
```

---

## Repo Structure

```
BabyGirl-VST3/
├── CMakeLists.txt                  ← JUCE cmake build
├── Source/
│   ├── PluginProcessor.h/.cpp      ← APVTS parameters, processBlock
│   ├── PluginEditor.h/.cpp         ← GUI: knobs, labels, DreamShare button
│   ├── HushDsp.h                   ← Tube preamp, ladder filter, tape delay
│   ├── KyotoDsp.h                  ← Vinyl, bitcrush, warp, shimmer reverb
│   ├── MasterDsp.h                 ← M/S imager, iron sat, limiter, metering
│   ├── ModulesDsp.h                ← 1176, LA-2A, Pultec, Big Muff, TB-303...
│   ├── DreamShare.h                ← DreamPreset data struct + JSON serialization
│   ├── DreamApi.h                  ← Cloud preset HTTP client (juce::URL)
│   ├── Presets.h                   ← Factory preset bank
│   └── CustomLookAndFeel.h         ← Analog machined knob renderer
├── .github/workflows/
│   └── build.yml                   ← CI: Windows + macOS + Linux matrix build
└── worker/
    ├── worker.js                   ← Cloudflare Worker: DreamAPI endpoints
    ├── wrangler.toml               ← Wrangler deploy config
    └── package.json
```

---

## Parameters

| ID | Name | Range | Default |
|---|---|---|---|
| `TUBE_DRIVE` | Tube Drive | 1–10 | 3.8 |
| `TUBE_BIAS` | Tube Bias | 0–1 | 0.42 |
| `TUBE_PENTODE` | Pentode Mode | bool | false |
| `FILTER_CUTOFF` | Filter Cutoff | 20–20000 Hz | 1850 |
| `FILTER_RESO` | Filter Resonance | 0–0.95 | 0.48 |
| `FILTER_MODE` | Filter Mode | LP/BP/HP | Lowpass 24dB |
| `TAPE_TIME` | Tape Delay Time | 0.05–1.2s | 0.36 |
| `TAPE_FEEDBACK` | Tape Feedback | 0–0.95 | 0.46 |
| `TAPE_FLUTTER` | Wow & Flutter | 0–1 | 0.32 |
| `TAPE_MIX` | Tape Wet Mix | 0–1 | 0.38 |
| `VINYL_CRACKLE` | Vinyl Crackle & Dust | 0–1 | 0.25 |
| `TAPE_WARP` | Tape Warp Drift | 0–1 | 0.28 |
| `LOFI_BITS` | Lofi Bit Depth | 4–16 | 16 |
| `SHIMMER_MIX` | Kyoto Shrine Shimmer | 0–1 | 0.35 |
| `STEREO_WIDTH` | Stereo Width | 0–2 | 1.25 |
| `IRON_DRIVE` | Transformer Iron Drive | 0–1 | 0.45 |
| `CEILING_DB` | True-Peak Ceiling | −6–0 dB | −0.2 |
| `FET_1176_ON` | 1176 Limiter Enable | bool | true |
| `FET_1176_IN` | 1176 Input Gain | 0–10 | 6.5 |
| `FET_1176_OUT` | 1176 Output Gain | 0–10 | 4.8 |
| `PULTEC_ON` | Pultec EQ Enable | bool | true |
| `PULTEC_BOOST` | Pultec 60Hz Boost | 0–10 | 4.5 |
| `PULTEC_ATTEN` | Pultec 60Hz Atten | 0–10 | 3.8 |

---

Made with JUCE · keganacummings-source/BabyGirl
