#pragma once
#include <juce_core/juce_core.h>

namespace BabyGirl
{
    /**
     * DreamShare Cloud & Preset Data Structure
     */
    struct DreamPreset
    {
        juce::String id;
        juce::String name;
        juce::String author;
        juce::String category;
        juce::String tags;
        float tubeDrive = 3.8f;
        float filterCutoff = 1850.0f;
        float filterReso = 0.48f;
        float tapeTime = 0.36f;
        float tapeFlutter = 0.32f;
        float vinylCrackle = 0.25f;
        float tapeWarp = 0.28f;
        float shimmerMix = 0.35f;
        float stereoWidth = 1.25f;
        float ironDrive = 0.45f;

        juce::var toJson() const
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("id", id);
            obj->setProperty("name", name);
            obj->setProperty("author", author);
            obj->setProperty("category", category);
            obj->setProperty("tags", tags);
            obj->setProperty("tubeDrive", tubeDrive);
            obj->setProperty("filterCutoff", filterCutoff);
            obj->setProperty("filterReso", filterReso);
            obj->setProperty("tapeTime", tapeTime);
            obj->setProperty("tapeFlutter", tapeFlutter);
            obj->setProperty("vinylCrackle", vinylCrackle);
            obj->setProperty("tapeWarp", tapeWarp);
            obj->setProperty("shimmerMix", shimmerMix);
            obj->setProperty("stereoWidth", stereoWidth);
            obj->setProperty("ironDrive", ironDrive);
            return juce::var(obj);
        }

        static DreamPreset fromJson(const juce::var& json)
        {
            DreamPreset p;
            if (auto* obj = json.getDynamicObject())
            {
                p.id = obj->getProperty("id").toString();
                p.name = obj->getProperty("name").toString();
                p.author = obj->getProperty("author").toString();
                p.category = obj->getProperty("category").toString();
                p.tags = obj->getProperty("tags").toString();
                p.tubeDrive = (float)obj->getProperty("tubeDrive");
                p.filterCutoff = (float)obj->getProperty("filterCutoff");
                p.filterReso = (float)obj->getProperty("filterReso");
                p.tapeTime = (float)obj->getProperty("tapeTime");
                p.tapeFlutter = (float)obj->getProperty("tapeFlutter");
                p.vinylCrackle = (float)obj->getProperty("vinylCrackle");
                p.tapeWarp = (float)obj->getProperty("tapeWarp");
                p.shimmerMix = (float)obj->getProperty("shimmerMix");
                p.stereoWidth = (float)obj->getProperty("stereoWidth");
                p.ironDrive = (float)obj->getProperty("ironDrive");
            }
            return p;
        }
    };
}
