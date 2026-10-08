#pragma once
#include <juce_core/juce_core.h>
#include "DreamShare.h"
#include <functional>

namespace BabyGirl
{
    /**
     * DreamApi Cloud Client
     * Connects with the BabyGirl Cloudflare Worker for cloud preset sharing
     */
    class DreamApi
    {
    public:
        DreamApi(const juce::String& endpoint = "https://babygirl-dream-api.workers.dev")
            : apiBaseUrl(endpoint) {}

        void setBaseUrl(const juce::String& url) { apiBaseUrl = url; }

        void fetchCloudPresets(const juce::String& query, const juce::String& tag,
                               std::function<void(std::vector<DreamPreset>, bool)> callback)
        {
            juce::URL url(apiBaseUrl + "/api/presets?query=" + juce::URL::escapeString(query) +
                          "&tag=" + juce::URL::escapeString(tag));
            juce::Thread::launch([url, callback]()
            {
                juce::String response = url.readEntireTextStream();
                if (response.isNotEmpty())
                {
                    auto json = juce::JSON::parse(response);
                    if (auto* obj = json.getDynamicObject())
                    {
                        if (auto* list = obj->getProperty("presets").getArray())
                        {
                            std::vector<DreamPreset> results;
                            for (auto& item : *list)
                            {
                                results.push_back(DreamPreset::fromJson(item));
                            }
                            juce::MessageManager::callAsync([callback, results]() {
                                callback(results, true);
                            });
                            return;
                        }
                    }
                }
                juce::MessageManager::callAsync([callback]() {
                    callback({}, false);
                });
            });
        }

        void publishPreset(const DreamPreset& preset, std::function<void(bool)> callback)
        {
            juce::URL url(apiBaseUrl + "/api/presets");
            juce::String jsonBody = juce::JSON::toString(preset.toJson());
            juce::Thread::launch([url, jsonBody, callback]()
            {
                auto postUrl = url.withPOSTData(jsonBody);
                int statusCode = 0;
                juce::String response = postUrl.readEntireTextStream(false, &statusCode);
                bool success = (statusCode >= 200 && statusCode < 300);
                juce::MessageManager::callAsync([callback, success]() {
                    callback(success);
                });
            });
        }

    private:
        juce::String apiBaseUrl;
    };
}
