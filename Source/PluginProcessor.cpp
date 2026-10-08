#include "PluginProcessor.h"
#include "PluginEditor.h"

BabyGirlAudioProcessor::BabyGirlAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BabyGirlAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Hush Parameters ---
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TUBE_DRIVE", "Tube Drive", juce::NormalisableRange<float>(1.0f, 10.0f, 0.1f), 3.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TUBE_BIAS", "Tube Bias", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.42f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "TUBE_PENTODE", "Pentode Mode", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FILTER_CUTOFF", "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 1850.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FILTER_RESO", "Filter Resonance", juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.48f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "FILTER_MODE", "Filter Mode", juce::StringArray{"Lowpass 24dB", "Bandpass", "Highpass"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TAPE_TIME", "Tape Delay Time", juce::NormalisableRange<float>(0.05f, 1.2f, 0.01f), 0.36f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TAPE_FEEDBACK", "Tape Feedback", juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.46f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TAPE_FLUTTER", "Tape Wow & Flutter", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.32f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TAPE_MIX", "Tape Wet Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.38f));

    // --- KyotoSpxrit Parameters ---
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "VINYL_CRACKLE", "Vinyl Crackle & Dust", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "TAPE_WARP", "Tape Warp Drift", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.28f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "LOFI_BITS", "Lofi Bits Resolution", juce::NormalisableRange<float>(4.0f, 16.0f, 0.5f), 16.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "SHIMMER_MIX", "Kyoto Shrine Shimmer", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.35f));

    // --- Master Parameters ---
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "STEREO_WIDTH", "Stereo Width", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.25f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "IRON_DRIVE", "Transformer Iron Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.45f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "CEILING_DB", "True-Peak Ceiling", juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f), -0.2f));

    // --- Modeled Modules Toggles & Parameters ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("FET_1176_ON", "1176 Limiter Enable", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FET_1176_IN", "1176 Input", 0.0f, 10.0f, 6.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FET_1176_OUT", "1176 Output", 0.0f, 10.0f, 4.8f));

    params.push_back(std::make_unique<juce::AudioParameterBool>("PULTEC_ON", "Pultec EQ Enable", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PULTEC_BOOST", "Pultec 60Hz Boost", 0.0f, 10.0f, 4.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PULTEC_ATTEN", "Pultec 60Hz Atten", 0.0f, 10.0f, 3.8f));

    return { params.begin(), params.end() };
}

void BabyGirlAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    hushEngine.prepare(spec);
    kyotoEngine.prepare(spec);
    masterEngine.prepare(spec);
    modulesEngine.prepare(sampleRate);
}

void BabyGirlAudioProcessor::releaseResources() {}

bool BabyGirlAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    return true;
}

void BabyGirlAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    // Fetch live parameters from APVTS
    float tubeDrive = apvts.getRawParameterValue("TUBE_DRIVE")->load();
    float tubeBias = apvts.getRawParameterValue("TUBE_BIAS")->load();
    bool pentode = apvts.getRawParameterValue("TUBE_PENTODE")->load() > 0.5f;

    float cutoff = apvts.getRawParameterValue("FILTER_CUTOFF")->load();
    float reso = apvts.getRawParameterValue("FILTER_RESO")->load();
    int fMode = (int)apvts.getRawParameterValue("FILTER_MODE")->load();
    hushEngine.updateFilter(cutoff, reso, fMode);

    float tapeTime = apvts.getRawParameterValue("TAPE_TIME")->load();
    float tapeFb = apvts.getRawParameterValue("TAPE_FEEDBACK")->load();
    float tapeFlutter = apvts.getRawParameterValue("TAPE_FLUTTER")->load();
    float tapeMix = apvts.getRawParameterValue("TAPE_MIX")->load();

    float vinyl = apvts.getRawParameterValue("VINYL_CRACKLE")->load();
    float warp = apvts.getRawParameterValue("TAPE_WARP")->load();
    float lofiBits = apvts.getRawParameterValue("LOFI_BITS")->load();
    float shimmer = apvts.getRawParameterValue("SHIMMER_MIX")->load();

    float stereoWidth = apvts.getRawParameterValue("STEREO_WIDTH")->load();
    float ironDrive = apvts.getRawParameterValue("IRON_DRIVE")->load();
    float ceiling = apvts.getRawParameterValue("CEILING_DB")->load();

    // 1. Process Hush Stage (Vacuum Tube Preamp, 4-Pole Ladder Filter, Tape Delay)
    hushEngine.process(buffer, tubeDrive, tubeBias, pentode, tapeTime, tapeFb, tapeFlutter, tapeMix);

    // 2. Process KyotoSpxrit Stage (Vinyl Dust, Tape Warp, Lofi Crunch, Shimmer)
    kyotoEngine.process(buffer, vinyl, warp, lofiBits, shimmer);

    // 3. Process Active Chained Hardware Modules (1176 & Pultec)
    bool use1176 = apvts.getRawParameterValue("FET_1176_ON")->load() > 0.5f;
    if (use1176)
    {
        float inGain = apvts.getRawParameterValue("FET_1176_IN")->load();
        float outGain = apvts.getRawParameterValue("FET_1176_OUT")->load();
        auto* l = buffer.getWritePointer(0);
        auto* r = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : l;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            modulesEngine.process1176(l[i], inGain, outGain, 4.0f, fet1176Env);
            if (l != r)
                modulesEngine.process1176(r[i], inGain, outGain, 4.0f, fet1176Env);
        }
    }

    bool usePultec = apvts.getRawParameterValue("PULTEC_ON")->load() > 0.5f;
    if (usePultec)
    {
        float pBoost = apvts.getRawParameterValue("PULTEC_BOOST")->load();
        float pAtten = apvts.getRawParameterValue("PULTEC_ATTEN")->load();
        auto* l = buffer.getWritePointer(0);
        auto* r = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : l;
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            modulesEngine.processPultec(l[i], pBoost, pAtten, 5.0f);
            if (l != r)
                modulesEngine.processPultec(r[i], pBoost, pAtten, 5.0f);
        }
    }

    // 4. Process Master Stage (Stereo Width, Transformer Iron, Brickwall Limiter, Telemetry)
    masterEngine.process(buffer, stereoWidth, ironDrive, ceiling, -18.0f, currentLufs, currentPeak);
}

juce::AudioProcessorEditor* BabyGirlAudioProcessor::createEditor()
{
    return new BabyGirlAudioProcessorEditor(*this);
}

void BabyGirlAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BabyGirlAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BabyGirlAudioProcessor();
}
