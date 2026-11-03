#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <vector>

class MinimalTransformerAudioProcessor : public juce::AudioProcessor
{
public:
    MinimalTransformerAudioProcessor();
    ~MinimalTransformerAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    // Presets (exposés aussi à l'hôte)
    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Pas en cours de lecture (pour l'affichage)
    std::atomic<int> stepIndexA { 0 };
    std::atomic<int> stepIndexB { 0 };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    struct SeqParams
    {
        std::atomic<float>* rate = nullptr;
        std::atomic<float>* len = nullptr;
        std::atomic<float>* glide = nullptr;
        std::atomic<float>* dest = nullptr;
        std::atomic<float>* depth = nullptr;
        std::atomic<float>* steps[16] = {};
    };

    // Paramètres
    std::atomic<float>* pCutoff   = nullptr;
    std::atomic<float>* pReso     = nullptr;
    std::atomic<float>* pMorph    = nullptr;
    std::atomic<float>* pDrive    = nullptr;
    std::atomic<float>* pEnvAmt   = nullptr;
    std::atomic<float>* pEnvAtk   = nullptr;
    std::atomic<float>* pEnvRel   = nullptr;
    std::atomic<float>* pLfoDepth = nullptr;
    std::atomic<float>* pLfoRate  = nullptr;
    std::atomic<float>* pLfoShape = nullptr;
    std::atomic<float>* pLfoSync  = nullptr;
    std::atomic<float>* pStereo   = nullptr;
    std::atomic<float>* pCrush    = nullptr;
    std::atomic<float>* pDlyTime  = nullptr;
    std::atomic<float>* pDlyFb    = nullptr;
    std::atomic<float>* pDlySend  = nullptr;
    std::atomic<float>* pMix      = nullptr;
    std::atomic<float>* pOut      = nullptr;
    SeqParams seqP[2];

    // Lissage des paramètres
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> smCutoff;
    juce::SmoothedValue<float> smReso, smMorph, smDrive, smCrush, smDlySend, smMix, smOut;

    // État du filtre / enveloppe / LFO (2 canaux max)
    float s1[2] = { 0.0f, 0.0f };
    float s2[2] = { 0.0f, 0.0f };
    float env[2] = { 0.0f, 0.0f };
    float modSm[2] = { 0.0f, 0.0f };
    float shValue[2] = { 0.0f, 0.0f };
    float prevPhase[2] = { 0.0f, 0.0f };
    float lfoPhase = 0.0f;

    // Séquenceurs
    float seqSmooth[2] = { 0.0f, 0.0f };
    double internalBeat = 0.0;

    // Bitcrusher
    float crushHold[2] = { 0.0f, 0.0f };
    int crushCount[2] = { 0, 0 };

    // Delay
    std::vector<float> dlyBuf[2];
    int dlyWritePos[2] = { 0, 0 };
    float dlyFbLp[2] = { 0.0f, 0.0f };
    float dlySamplesSm = 1000.0f;
    int dlySize = 0;

    int currentProgram = 0;
    double sr = 44100.0;
    juce::Random rng;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinimalTransformerAudioProcessor)
};
