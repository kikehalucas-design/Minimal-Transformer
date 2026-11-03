#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"
#include <cmath>

namespace
{
    // Limiteur doux : transparent sous 0.8, arrondit au-dessus
    inline float softClip (float v) noexcept
    {
        const float t = 0.8f;
        const float a = std::abs (v);
        if (a <= t)
            return v;
        const float s = v < 0.0f ? -1.0f : 1.0f;
        return s * (t + (1.0f - t) * std::tanh ((a - t) / (1.0f - t)));
    }

    // Cumul des modulations envoyées par les séquenceurs
    struct Mods
    {
        float oct = 0.0f;
        float reso = 0.0f;
        float morph = 0.0f;
        float drive = 0.0f;
        float dly = 0.0f;
        float crush = 0.0f;
        float gate = 1.0f;
    };

    // dest : 0 Off, 1 Cutoff, 2 Resonance, 3 Morph, 4 Drive, 5 Delay Send, 6 Bitcrush, 7 Gate
    inline void applyDest (Mods& m, int dest, float v, float depth) noexcept
    {
        const float a = v * depth;
        switch (dest)
        {
            case 1: m.oct += a * 4.0f; break;
            case 2: m.reso += a; break;
            case 3: m.morph += a; break;
            case 4: m.drive += a; break;
            case 5: m.dly += a; break;
            case 6: m.crush += a; break;
            case 7:
            {
                const float d = std::abs (depth);
                const float t = depth >= 0.0f ? v : 1.0f - v;
                m.gate *= (1.0f - d) + d * t;
                break;
            }
            default: break;
        }
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MinimalTransformerAudioProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    // ---- Filtre
    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "cutoff", 1 }, "Cutoff",
        NormalisableRange<float> (20.0f, 20000.0f, 0.0f, 0.3f), 1000.0f,
        AudioParameterFloatAttributes().withLabel ("Hz")));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "reso", 1 }, "Resonance",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.3f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "morph", 1 }, "Morph (LP-BP-HP)",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "drive", 1 }, "Drive",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.2f));

    // ---- Enveloppe
    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "envAmt", 1 }, "Env Amount",
        NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.0f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "envAtk", 1 }, "Env Attack",
        NormalisableRange<float> (1.0f, 200.0f, 0.1f, 0.5f), 5.0f,
        AudioParameterFloatAttributes().withLabel ("ms")));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "envRel", 1 }, "Env Release",
        NormalisableRange<float> (10.0f, 1000.0f, 1.0f, 0.5f), 150.0f,
        AudioParameterFloatAttributes().withLabel ("ms")));

    // ---- LFO
    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "lfoDepth", 1 }, "LFO Depth",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "lfoRate", 1 }, "LFO Rate",
        NormalisableRange<float> (0.05f, 20.0f, 0.01f, 0.4f), 1.0f,
        AudioParameterFloatAttributes().withLabel ("Hz")));

    p.push_back (std::make_unique<AudioParameterChoice> (
        ParameterID { "lfoShape", 1 }, "LFO Shape",
        StringArray { "Sine", "Triangle", "Saw", "Square", "S&H" }, 0));

    p.push_back (std::make_unique<AudioParameterChoice> (
        ParameterID { "lfoSync", 1 }, "LFO Sync",
        StringArray { "Free", "1 bar", "1/2", "1/4", "1/8", "1/16" }, 0));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "stereo", 1 }, "Stereo Spread",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    // ---- Effets
    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "crush", 1 }, "Bitcrush",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    p.push_back (std::make_unique<AudioParameterChoice> (
        ParameterID { "dlyTime", 1 }, "Delay Time",
        StringArray { "1/16", "1/8", "1/8.", "1/4", "1/4.", "1/2" }, 2));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "dlyFb", 1 }, "Delay Feedback",
        NormalisableRange<float> (0.0f, 0.95f, 0.001f), 0.4f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "dlySend", 1 }, "Delay Send",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    // ---- Sortie
    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "mix", 1 }, "Mix",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    p.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "output", 1 }, "Output",
        NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f,
        AudioParameterFloatAttributes().withLabel ("dB")));

    // ---- Deux séquenceurs (couches A et B)
    const StringArray rates { "1/4", "1/8", "1/16", "1/32", "1/8T", "1/16T" };
    const StringArray dests { "Off", "Cutoff", "Resonance", "Morph", "Drive", "Delay Send", "Bitcrush", "Gate" };

    for (int l = 0; l < 2; ++l)
    {
        const String pre = (l == 0) ? "a" : "b";
        const String name = (l == 0) ? "Seq A " : "Seq B ";
        const String stepPre = (l == 0) ? "stepA" : "stepB";

        p.push_back (std::make_unique<AudioParameterChoice> (
            ParameterID { pre + "Rate", 1 }, name + "Rate", rates, 2));

        p.push_back (std::make_unique<AudioParameterInt> (
            ParameterID { pre + "Len", 1 }, name + "Length", 1, 16, 16));

        p.push_back (std::make_unique<AudioParameterFloat> (
            ParameterID { pre + "Glide", 1 }, name + "Glide",
            NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

        p.push_back (std::make_unique<AudioParameterChoice> (
            ParameterID { pre + "Dest", 1 }, name + "Destination", dests, 0));

        p.push_back (std::make_unique<AudioParameterFloat> (
            ParameterID { pre + "Depth", 1 }, name + "Depth",
            NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.5f));

        for (int i = 1; i <= 16; ++i)
            p.push_back (std::make_unique<AudioParameterFloat> (
                ParameterID { stepPre + String (i), 1 }, name + "Step " + String (i),
                NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));
    }

    return { p.begin(), p.end() };
}

//==============================================================================
MinimalTransformerAudioProcessor::MinimalTransformerAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    pCutoff   = apvts.getRawParameterValue ("cutoff");
    pReso     = apvts.getRawParameterValue ("reso");
    pMorph    = apvts.getRawParameterValue ("morph");
    pDrive    = apvts.getRawParameterValue ("drive");
    pEnvAmt   = apvts.getRawParameterValue ("envAmt");
    pEnvAtk   = apvts.getRawParameterValue ("envAtk");
    pEnvRel   = apvts.getRawParameterValue ("envRel");
    pLfoDepth = apvts.getRawParameterValue ("lfoDepth");
    pLfoRate  = apvts.getRawParameterValue ("lfoRate");
    pLfoShape = apvts.getRawParameterValue ("lfoShape");
    pLfoSync  = apvts.getRawParameterValue ("lfoSync");
    pStereo   = apvts.getRawParameterValue ("stereo");
    pCrush    = apvts.getRawParameterValue ("crush");
    pDlyTime  = apvts.getRawParameterValue ("dlyTime");
    pDlyFb    = apvts.getRawParameterValue ("dlyFb");
    pDlySend  = apvts.getRawParameterValue ("dlySend");
    pMix      = apvts.getRawParameterValue ("mix");
    pOut      = apvts.getRawParameterValue ("output");

    for (int l = 0; l < 2; ++l)
    {
        const juce::String pre = (l == 0) ? "a" : "b";
        const juce::String stepPre = (l == 0) ? "stepA" : "stepB";

        seqP[l].rate  = apvts.getRawParameterValue (pre + "Rate");
        seqP[l].len   = apvts.getRawParameterValue (pre + "Len");
        seqP[l].glide = apvts.getRawParameterValue (pre + "Glide");
        seqP[l].dest  = apvts.getRawParameterValue (pre + "Dest");
        seqP[l].depth = apvts.getRawParameterValue (pre + "Depth");

        for (int i = 0; i < 16; ++i)
            seqP[l].steps[i] = apvts.getRawParameterValue (stepPre + juce::String (i + 1));
    }
}

bool MinimalTransformerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono())
        return false;

    return in == out;
}

void MinimalTransformerAudioProcessor::prepareToPlay (double sampleRate, int)
{
    sr = sampleRate;

    smCutoff.reset (sr, 0.02);
    smReso.reset (sr, 0.02);
    smMorph.reset (sr, 0.02);
    smDrive.reset (sr, 0.02);
    smCrush.reset (sr, 0.02);
    smDlySend.reset (sr, 0.02);
    smMix.reset (sr, 0.02);
    smOut.reset (sr, 0.02);

    smCutoff.setCurrentAndTargetValue (pCutoff->load());
    smReso.setCurrentAndTargetValue (pReso->load());
    smMorph.setCurrentAndTargetValue (pMorph->load());
    smDrive.setCurrentAndTargetValue (pDrive->load());
    smCrush.setCurrentAndTargetValue (pCrush->load());
    smDlySend.setCurrentAndTargetValue (pDlySend->load());
    smMix.setCurrentAndTargetValue (pMix->load());
    smOut.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (pOut->load()));

    dlySize = (int) (sr * 4.5) + 8;
    for (int ch = 0; ch < 2; ++ch)
    {
        dlyBuf[ch].assign ((size_t) dlySize, 0.0f);
        dlyWritePos[ch] = 0;
        dlyFbLp[ch] = 0.0f;
        crushHold[ch] = 0.0f;
        crushCount[ch] = 0;

        s1[ch] = s2[ch] = 0.0f;
        env[ch] = 0.0f;
        modSm[ch] = 0.0f;
        shValue[ch] = 0.0f;
        prevPhase[ch] = 0.0f;
        seqSmooth[ch] = 0.0f;
    }
    dlySamplesSm = (float) (sr * 0.3);
    lfoPhase = 0.0f;
    internalBeat = 0.0;
}

//==============================================================================
void MinimalTransformerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = juce::jmin (buffer.getNumChannels(), 2);
    const int numSamples = buffer.getNumSamples();
    const float fs = (float) sr;

    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    if (dlySize < 16)
        return;

    smCutoff.setTargetValue (pCutoff->load());
    smReso.setTargetValue (pReso->load());
    smMorph.setTargetValue (pMorph->load());
    smDrive.setTargetValue (pDrive->load());
    smCrush.setTargetValue (pCrush->load());
    smDlySend.setTargetValue (pDlySend->load());
    smMix.setTargetValue (pMix->load());
    smOut.setTargetValue (juce::Decibels::decibelsToGain (pOut->load()));

    const float envAmt   = pEnvAmt->load();
    const float lfoDepth = pLfoDepth->load();
    const float stereo   = pStereo->load();
    const int shape      = (int) pLfoShape->load();
    const int sync       = (int) pLfoSync->load();

    // ---- Infos de l'hôte : tempo et position
    double bpm = 120.0;
    double ppq = 0.0;
    bool playing = false;
    bool havePpq = false;

    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm())
                bpm = *b;
            playing = pos->getIsPlaying();
            if (auto pp = pos->getPpqPosition())
            {
                ppq = *pp;
                havePpq = true;
            }
        }

    bpm = juce::jlimit (20.0, 400.0, bpm);
    const bool synced = playing && havePpq;
    double beatPos = synced ? ppq : internalBeat;
    const double beatInc = bpm / 60.0 / sr;

    // ---- LFO : fréquence libre ou synchronisée
    float rateHz = pLfoRate->load();
    if (sync > 0)
    {
        static const float beats[] = { 0.0f, 4.0f, 2.0f, 1.0f, 0.5f, 0.25f };
        rateHz = (float) (bpm / 60.0 / (double) beats[juce::jlimit (1, 5, sync)]);
    }

    // ---- Séquenceurs : préparation du bloc
    static const double rateBeats[] = { 1.0, 0.5, 0.25, 0.125, 1.0 / 3.0, 1.0 / 6.0 };

    struct SeqBlock
    {
        float steps[16];
        int len;
        double stepBeats;
        int dest;
        float depth;
        float glideCoef;
    };
    SeqBlock sb[2];

    for (int l = 0; l < 2; ++l)
    {
        const auto& sp = seqP[l];
        sb[l].len = juce::jlimit (1, 16, (int) std::lround (sp.len->load()));
        sb[l].dest = (int) sp.dest->load();
        sb[l].depth = sp.depth->load();
        for (int i = 0; i < 16; ++i)
            sb[l].steps[i] = sp.steps[i]->load();

        const int ri = juce::jlimit (0, 5, (int) sp.rate->load());
        sb[l].stepBeats = rateBeats[ri];

        const float glide = sp.glide->load();
        const double stepSec = sb[l].stepBeats * 60.0 / bpm;
        sb[l].glideCoef = glide < 0.01f
                              ? 1.0f
                              : (float) (1.0 - std::exp (-1.0 / ((double) glide * 0.7 * stepSec * sr + 1.0)));
    }
    int lastStep[2] = { 0, 0 };

    // ---- Delay
    static const double dtBeats[] = { 0.25, 0.5, 0.75, 1.0, 1.5, 2.0 };
    const int dtIndex = juce::jlimit (0, 5, (int) pDlyTime->load());
    const float dlyTarget = (float) juce::jlimit (4.0, (double) (dlySize - 4), dtBeats[dtIndex] * 60.0 / bpm * sr);
    const float dlySmCoef = 1.0f - std::exp (-1.0f / (0.05f * fs));
    const float dlyFb = juce::jlimit (0.0f, 0.95f, pDlyFb->load());
    const float twoPi = juce::MathConstants<float>::twoPi;
    const float fbLpCoef = 1.0f - std::exp (-twoPi * 5000.0f / fs);

    // ---- Enveloppe, LFO, filtre
    const float atk = std::exp (-1.0f / (0.001f * pEnvAtk->load() * fs));
    const float rel = std::exp (-1.0f / (0.001f * pEnvRel->load() * fs));
    const float modCoef = 1.0f - std::exp (-1.0f / (0.003f * fs));
    const float maxFc = 0.45f * fs;
    const float pi = juce::MathConstants<float>::pi;

    for (int i = 0; i < numSamples; ++i)
    {
        // --- Séquenceurs
        Mods mods;
        for (int l = 0; l < 2; ++l)
        {
            const auto& s = sb[l];
            const double stepPos = beatPos / s.stepBeats;
            int idx = (int) std::floor (stepPos);
            idx = ((idx % s.len) + s.len) % s.len;

            seqSmooth[l] += s.glideCoef * (s.steps[idx] - seqSmooth[l]);
            lastStep[l] = idx;
            applyDest (mods, s.dest, seqSmooth[l], s.depth);
        }
        beatPos += beatInc;

        // --- Paramètres lissés + modulation des séquenceurs
        const float cutoffBase = smCutoff.getNextValue();
        const float reso  = juce::jlimit (0.0f, 1.0f, smReso.getNextValue()  + mods.reso);
        const float morph = juce::jlimit (0.0f, 1.0f, smMorph.getNextValue() + mods.morph);
        const float drive = juce::jlimit (0.0f, 1.0f, smDrive.getNextValue() + mods.drive);
        const float crush = juce::jlimit (0.0f, 1.0f, smCrush.getNextValue() + mods.crush);
        const float dlySend = juce::jlimit (0.0f, 1.0f, smDlySend.getNextValue() + mods.dly);
        const float mix  = smMix.getNextValue();
        const float outG = smOut.getNextValue();

        dlySamplesSm += dlySmCoef * (dlyTarget - dlySamplesSm);

        const float driveGain = 1.0f + drive * 24.0f;
        const float driveComp = 1.0f / std::pow (driveGain, 0.4f);
        const float q = 0.6f + reso * reso * 24.0f;
        const float R = 1.0f / (2.0f * q);

        const bool crushOn = crush > 0.002f;
        const float crushScale = 0.5f * std::pow (2.0f, 16.0f - crush * 13.0f);
        const int holdN = 1 + (int) (crush * crush * 24.0f);

        for (int ch = 0; ch < numCh; ++ch)
        {
            float* data = buffer.getWritePointer (ch);
            const float dry = data[i];

            // --- Suiveur d'enveloppe
            const float a = std::abs (dry);
            env[ch] = (a > env[ch]) ? atk * env[ch] + (1.0f - atk) * a
                                    : rel * env[ch] + (1.0f - rel) * a;
            const float envMod = juce::jlimit (0.0f, 1.0f, env[ch] * 3.0f);

            // --- LFO (avec décalage de phase stéréo)
            float ph = lfoPhase + (float) ch * stereo * 0.5f;
            ph -= std::floor (ph);

            float lfo = 0.0f;
            switch (shape)
            {
                case 0:  lfo = std::sin (twoPi * ph); break;
                case 1:  lfo = 4.0f * std::abs (ph - 0.5f) - 1.0f; break;
                case 2:  lfo = 2.0f * ph - 1.0f; break;
                case 3:  lfo = ph < 0.5f ? 1.0f : -1.0f; break;
                default:
                    if (ph < prevPhase[ch])
                        shValue[ch] = rng.nextFloat() * 2.0f - 1.0f;
                    lfo = shValue[ch];
                    break;
            }
            prevPhase[ch] = ph;

            // --- Modulation de la coupure (en octaves) : enveloppe + LFO + séquenceurs
            const float targetOct = envAmt * envMod * 4.0f + lfoDepth * lfo * 2.0f + mods.oct;
            modSm[ch] += modCoef * (targetOct - modSm[ch]);

            const float fc = juce::jlimit (20.0f, maxFc, cutoffBase * std::exp2 (modSm[ch]));
            const float g = std::tan (pi * fc / fs);

            // --- Drive (saturation avant le filtre)
            const float xin = std::tanh (dry * driveGain) * driveComp;

            // --- Filtre à variables d'état (TPT)
            const float a1 = 1.0f / (1.0f + 2.0f * R * g + g * g);
            const float hp = (xin - (2.0f * R + g) * s1[ch] - s2[ch]) * a1;
            const float bp = g * hp + s1[ch];
            const float lp = g * bp + s2[ch];
            s1[ch] = g * hp + bp;
            s2[ch] = g * bp + lp;

            const float bpN = bp * 2.0f * R; // gain unitaire au centre

            // --- Morph : LP -> BP -> HP
            float y;
            if (morph < 0.5f)
            {
                const float t = morph * 2.0f;
                y = lp * (1.0f - t) + bpN * t;
            }
            else
            {
                const float t = (morph - 0.5f) * 2.0f;
                y = bpN * (1.0f - t) + hp * t;
            }

            if (! std::isfinite (y))
            {
                s1[ch] = s2[ch] = 0.0f;
                y = 0.0f;
            }

            y = softClip (y);

            // --- Bitcrusher (réduction de bits + de fréquence d'échantillonnage)
            if (crushOn)
            {
                if (crushCount[ch] <= 0)
                {
                    crushHold[ch] = std::round (y * crushScale) / crushScale;
                    crushCount[ch] = holdN;
                }
                --crushCount[ch];
                y = crushHold[ch];
            }

            // --- Delay synchronisé au tempo (le séquenceur peut moduler l'envoi)
            {
                auto& buf = dlyBuf[ch];
                float rp = (float) dlyWritePos[ch] - dlySamplesSm;
                if (rp < 0.0f)
                    rp += (float) dlySize;

                const int i0 = juce::jmin ((int) rp, dlySize - 1);
                const float fr = rp - (float) i0;
                const int i1 = (i0 + 1) % dlySize;
                const float delayed = buf[(size_t) i0] * (1.0f - fr) + buf[(size_t) i1] * fr;

                dlyFbLp[ch] += fbLpCoef * (delayed - dlyFbLp[ch]);
                buf[(size_t) dlyWritePos[ch]] = softClip (y * dlySend + dlyFbLp[ch] * dlyFb);
                dlyWritePos[ch] = (dlyWritePos[ch] + 1) % dlySize;

                y += delayed;
            }

            // --- Gate rythmique
            y *= mods.gate;

            data[i] = (dry * (1.0f - mix) + y * mix) * outG;
        }

        lfoPhase += rateHz / fs;
        lfoPhase -= std::floor (lfoPhase);
    }

    internalBeat = beatPos;
    stepIndexA.store (lastStep[0]);
    stepIndexB.store (lastStep[1]);
}

//==============================================================================
int MinimalTransformerAudioProcessor::getNumPrograms()
{
    return (int) getPresets().size();
}

const juce::String MinimalTransformerAudioProcessor::getProgramName (int index)
{
    const auto& presets = getPresets();
    if (juce::isPositiveAndBelow (index, (int) presets.size()))
        return presets[(size_t) index].name;
    return {};
}

void MinimalTransformerAudioProcessor::setCurrentProgram (int index)
{
    const auto& presets = getPresets();
    if (! juce::isPositiveAndBelow (index, (int) presets.size()))
        return;

    currentProgram = index;
    applyPreset (apvts, presets[(size_t) index]);
}

juce::AudioProcessorEditor* MinimalTransformerAudioProcessor::createEditor()
{
    return new MinimalTransformerAudioProcessorEditor (*this);
}

void MinimalTransformerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("presetIndex", currentProgram, nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void MinimalTransformerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            currentProgram = juce::jlimit (0, juce::jmax (0, getNumPrograms() - 1),
                                           (int) tree.getProperty ("presetIndex", 0));
            apvts.replaceState (tree);
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MinimalTransformerAudioProcessor();
}
