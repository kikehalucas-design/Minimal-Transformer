#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

struct PresetValue
{
    const char* id;
    float value;
};

struct Preset
{
    const char* name;
    std::vector<PresetValue> values;  // seuls les réglages différents des valeurs par défaut
    const char* stepsA;               // 16 chiffres de 0 à 9 (hauteur de chaque pas)
    const char* stepsB;
};

const std::vector<Preset>& getPresets();

// Remet tous les paramètres à leur valeur par défaut, puis applique le preset
void applyPreset (juce::AudioProcessorValueTreeState& apvts, const Preset& preset);
