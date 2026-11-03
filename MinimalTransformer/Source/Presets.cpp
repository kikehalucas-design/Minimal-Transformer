#include "Presets.h"

namespace
{
    // Destinations des séquenceurs
    namespace Dest
    {
        constexpr float Off = 0.f, Cut = 1.f, Res = 2.f, Mor = 3.f, Drv = 4.f, Dly = 5.f, Cru = 6.f, Gate = 7.f;
    }
    // Vitesse des séquenceurs
    namespace Rate
    {
        constexpr float r4 = 0.f, r8 = 1.f, r16 = 2.f, r32 = 3.f, r8t = 4.f, r16t = 5.f;
    }
    // Temps du delay
    namespace DTime
    {
        constexpr float t16 = 0.f, t8 = 1.f, t8d = 2.f, t4 = 3.f, t4d = 4.f, t2 = 5.f;
    }
    // Formes du LFO
    namespace Shape
    {
        constexpr float Sine = 0.f, Tri = 1.f, Saw = 2.f, Sqr = 3.f, SH = 4.f;
    }
    // Synchro du LFO
    namespace Sync
    {
        constexpr float Free = 0.f, Bar = 1.f, N2 = 2.f, N4 = 3.f, N8 = 4.f, N16 = 5.f;
    }

    std::vector<Preset> buildPresets()
    {
        std::vector<Preset> p;

        // ---------------------------------------------------------------- Départ
        p.push_back ({ "Init", {}, "5555555555555555", "5555555555555555" });

        // ---------------------------------------------------------------- Filtres / basses
        p.push_back ({ "Minimal Transformer",
            { {"cutoff", 900.f}, {"reso", 0.5f}, {"morph", 0.15f}, {"drive", 0.35f},
              {"envAmt", 0.5f}, {"envAtk", 4.f}, {"envRel", 130.f},
              {"lfoDepth", 0.2f}, {"lfoRate", 0.5f},
              {"dlyTime", DTime::t8d}, {"dlyFb", 0.4f}, {"dlySend", 0.15f},
              {"aDest", Dest::Cut}, {"aDepth", 0.6f}, {"aRate", Rate::r16}, {"aLen", 16.f}, {"aGlide", 0.15f},
              {"bDest", Dest::Gate}, {"bDepth", 0.35f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "9005300790053007", "9959995999599959" });

        p.push_back ({ "Minimal Pulse",
            { {"cutoff", 700.f}, {"reso", 0.45f}, {"drive", 0.25f},
              {"envAmt", 0.4f}, {"envAtk", 3.f}, {"envRel", 120.f},
              {"aDest", Dest::Cut}, {"aDepth", 0.7f}, {"aRate", Rate::r16}, {"aLen", 16.f}, {"aGlide", 0.2f},
              {"bDest", Dest::Gate}, {"bDepth", 0.3f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "9004600590046005", "9959995999599959" });

        p.push_back ({ "Rubber Bass",
            { {"cutoff", 250.f}, {"reso", 0.6f}, {"drive", 0.5f},
              {"envAmt", 0.85f}, {"envAtk", 2.f}, {"envRel", 140.f}, {"output", -2.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Acid Squeeze",
            { {"cutoff", 500.f}, {"reso", 0.8f}, {"drive", 0.6f}, {"envAmt", 0.3f},
              {"aDest", Dest::Cut}, {"aDepth", 0.8f}, {"aRate", Rate::r16}, {"aLen", 16.f}, {"aGlide", 0.1f} },
            "9040705090407050", "5555555555555555" });

        p.push_back ({ "Wah Walker",
            { {"cutoff", 800.f}, {"reso", 0.55f}, {"morph", 0.5f}, {"drive", 0.3f},
              {"envAmt", 0.9f}, {"envAtk", 4.f}, {"envRel", 220.f},
              {"lfoDepth", 0.2f}, {"lfoRate", 0.6f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Pluck Maker",
            { {"cutoff", 280.f}, {"reso", 0.55f}, {"drive", 0.3f},
              {"envAmt", 1.f}, {"envAtk", 1.f}, {"envRel", 80.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Sub Tamer",
            { {"cutoff", 180.f}, {"reso", 0.2f}, {"drive", 0.35f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Dark Room",
            { {"cutoff", 400.f}, {"reso", 0.15f}, {"envAmt", 0.2f},
              {"lfoDepth", 0.35f}, {"lfoRate", 0.15f}, {"lfoShape", Shape::Tri}, {"output", 1.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Soft Focus",
            { {"cutoff", 2500.f}, {"reso", 0.1f}, {"drive", 0.05f}, {"mix", 0.6f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "HP Air Lift",
            { {"cutoff", 2500.f}, {"reso", 0.3f}, {"morph", 1.f}, {"drive", 0.15f}, {"envAmt", 0.3f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Chrome Bite",
            { {"cutoff", 2200.f}, {"reso", 0.9f}, {"morph", 0.5f}, {"drive", 0.7f},
              {"envAmt", 0.5f}, {"output", -4.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Radio Voice",
            { {"cutoff", 1600.f}, {"reso", 0.5f}, {"morph", 0.5f}, {"drive", 0.55f}, {"crush", 0.15f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Telephone",
            { {"cutoff", 1200.f}, {"reso", 0.3f}, {"morph", 0.5f}, {"drive", 0.4f}, {"crush", 0.1f} },
            "5555555555555555", "5555555555555555" });

        // ---------------------------------------------------------------- Rythmique (gate / séquences)
        p.push_back ({ "Trance Gate 16",
            { {"cutoff", 8000.f}, {"reso", 0.1f},
              {"bDest", Dest::Gate}, {"bDepth", 1.f}, {"bRate", Rate::r16}, {"bLen", 16.f}, {"bGlide", 0.05f} },
            "5555555555555555", "9090909090909090" });

        p.push_back ({ "Offbeat Chop",
            { {"cutoff", 12000.f},
              {"bDest", Dest::Gate}, {"bDepth", 0.9f}, {"bRate", Rate::r8}, {"bLen", 8.f}, {"bGlide", 0.05f} },
            "5555555555555555", "0909090909090909" });

        p.push_back ({ "Stutter Roll",
            { {"cutoff", 10000.f}, {"drive", 0.3f},
              {"bDest", Dest::Gate}, {"bDepth", 1.f}, {"bRate", Rate::r32}, {"bLen", 16.f} },
            "5555555555555555", "9090909090909090" });

        p.push_back ({ "Machine Gun",
            { {"cutoff", 5000.f}, {"reso", 0.4f}, {"drive", 0.5f},
              {"bDest", Dest::Gate}, {"bDepth", 1.f}, {"bRate", Rate::r32}, {"bLen", 16.f} },
            "5555555555555555", "9900990099009900" });

        p.push_back ({ "Tresillo Filter",
            { {"cutoff", 600.f}, {"reso", 0.5f},
              {"aDest", Dest::Cut}, {"aDepth", 0.75f}, {"aRate", Rate::r16}, {"aLen", 16.f}, {"aGlide", 0.1f} },
            "9009009009009090", "5555555555555555" });

        p.push_back ({ "Sidechain Pump",
            { {"cutoff", 20000.f},
              {"bDest", Dest::Gate}, {"bDepth", -0.8f}, {"bRate", Rate::r4}, {"bLen", 4.f}, {"bGlide", 0.35f} },
            "5555555555555555", "9000900090009000" });

        p.push_back ({ "Pumping Wobble",
            { {"cutoff", 400.f}, {"reso", 0.6f}, {"drive", 0.45f},
              {"lfoDepth", 0.8f}, {"lfoShape", Shape::Saw}, {"lfoSync", Sync::N8},
              {"bDest", Dest::Gate}, {"bDepth", -0.5f}, {"bRate", Rate::r4}, {"bLen", 4.f}, {"bGlide", 0.3f} },
            "5555555555555555", "9000900090009000" });

        p.push_back ({ "Bit Gate Machine",
            { {"cutoff", 6000.f}, {"reso", 0.3f}, {"crush", 0.35f},
              {"aDest", Dest::Cut}, {"aDepth", 0.6f}, {"aRate", Rate::r16}, {"aLen", 16.f},
              {"bDest", Dest::Gate}, {"bDepth", 0.8f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "9040904090409040", "9909090999090909" });

        // ---------------------------------------------------------------- Delay
        p.push_back ({ "Dub Throws",
            { {"cutoff", 3000.f}, {"reso", 0.2f},
              {"dlyTime", DTime::t4d}, {"dlyFb", 0.6f},
              {"bDest", Dest::Dly}, {"bDepth", 1.f}, {"bRate", Rate::r8}, {"bLen", 16.f} },
            "5555555555555555", "9000000090000000" });

        p.push_back ({ "Ping Spiral",
            { {"cutoff", 5000.f}, {"reso", 0.3f},
              {"dlyTime", DTime::t8d}, {"dlyFb", 0.72f}, {"dlySend", 0.45f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Echo Gate",
            { {"cutoff", 7000.f},
              {"dlyTime", DTime::t8}, {"dlyFb", 0.5f}, {"dlySend", 0.35f},
              {"bDest", Dest::Gate}, {"bDepth", 0.9f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "5555555555555555", "9909090999090909" });

        p.push_back ({ "Space Echo",
            { {"cutoff", 3500.f}, {"drive", 0.3f},
              {"dlyTime", DTime::t4d}, {"dlyFb", 0.62f}, {"dlySend", 0.5f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Rhythm Wash",
            { {"cutoff", 1000.f}, {"reso", 0.45f}, {"stereo", 0.6f},
              {"lfoDepth", 0.3f}, {"lfoSync", Sync::N2},
              {"dlyTime", DTime::t8}, {"dlyFb", 0.65f}, {"dlySend", 0.3f},
              {"aDest", Dest::Cut}, {"aDepth", 0.6f}, {"aRate", Rate::r8}, {"aLen", 16.f} },
            "9060906090609060", "5555555555555555" });

        // ---------------------------------------------------------------- Lo-fi
        p.push_back ({ "Tape Dust",
            { {"cutoff", 6000.f}, {"reso", 0.1f}, {"drive", 0.45f}, {"crush", 0.12f},
              {"lfoDepth", 0.1f}, {"lfoRate", 0.3f}, {"output", -1.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "8-Bit Squash",
            { {"cutoff", 9000.f}, {"reso", 0.2f}, {"drive", 0.5f}, {"crush", 0.7f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Crunch Steps",
            { {"cutoff", 7000.f},
              {"bDest", Dest::Cru}, {"bDepth", 0.9f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "5555555555555555", "9009000090090000" });

        // ---------------------------------------------------------------- Mouvements / effets
        p.push_back ({ "Breathing Pad",
            { {"cutoff", 1200.f}, {"reso", 0.2f}, {"drive", 0.1f},
              {"lfoDepth", 0.5f}, {"lfoShape", Shape::Sine}, {"lfoSync", Sync::Bar}, {"stereo", 0.5f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Half-Time Wobble",
            { {"cutoff", 300.f}, {"reso", 0.55f}, {"drive", 0.4f},
              {"lfoDepth", 0.7f}, {"lfoShape", Shape::Tri}, {"lfoSync", Sync::N2} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Stereo Swirl",
            { {"cutoff", 1800.f}, {"reso", 0.45f},
              {"lfoDepth", 0.6f}, {"lfoShape", Shape::Sine}, {"lfoSync", Sync::N4}, {"stereo", 1.f} },
            "5555555555555555", "5555555555555555" });

        p.push_back ({ "Riser Opener",
            { {"cutoff", 150.f}, {"reso", 0.5f},
              {"aDest", Dest::Cut}, {"aDepth", 1.f}, {"aRate", Rate::r4}, {"aLen", 16.f}, {"aGlide", 0.6f} },
            "0011223344556789", "5555555555555555" });

        p.push_back ({ "Falling Filter",
            { {"cutoff", 4000.f}, {"reso", 0.45f},
              {"aDest", Dest::Cut}, {"aDepth", -1.f}, {"aRate", Rate::r4}, {"aLen", 16.f}, {"aGlide", 0.6f} },
            "0011223344556789", "5555555555555555" });

        p.push_back ({ "Random Walk",
            { {"cutoff", 700.f}, {"reso", 0.5f},
              {"aDest", Dest::Cut}, {"aDepth", 0.8f}, {"aRate", Rate::r8}, {"aLen", 16.f}, {"aGlide", 0.4f} },
            "3907528164703958", "5555555555555555" });

        p.push_back ({ "Glitch Garden",
            { {"cutoff", 1500.f}, {"reso", 0.6f}, {"crush", 0.25f},
              {"dlyTime", DTime::t8}, {"dlyFb", 0.55f},
              {"aDest", Dest::Cut}, {"aDepth", 0.8f}, {"aRate", Rate::r32}, {"aLen", 16.f},
              {"bDest", Dest::Dly}, {"bDepth", 0.8f}, {"bRate", Rate::r16}, {"bLen", 16.f} },
            "9205814937052846", "9000900190000090" });

        return p;
    }
}

const std::vector<Preset>& getPresets()
{
    static const std::vector<Preset> presets = buildPresets();
    return presets;
}

void applyPreset (juce::AudioProcessorValueTreeState& apvts, const Preset& preset)
{
    // 1) tout remettre aux valeurs par défaut
    for (auto* param : apvts.processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
            ranged->setValueNotifyingHost (ranged->getDefaultValue());

    // 2) appliquer les réglages du preset
    for (const auto& pv : preset.values)
        if (auto* param = apvts.getParameter (pv.id))
            param->setValueNotifyingHost (param->convertTo0to1 (pv.value));

    // 3) motifs des deux séquenceurs
    auto setSteps = [&apvts] (const char* prefix, const char* pattern)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (pattern[i] == '\0')
                break;

            const float v = (float) (pattern[i] - '0') / 9.0f;

            if (auto* param = apvts.getParameter (juce::String (prefix) + juce::String (i + 1)))
                param->setValueNotifyingHost (param->convertTo0to1 (v));
        }
    };

    setSteps ("stepA", preset.stepsA);
    setSteps ("stepB", preset.stepsB);
}
