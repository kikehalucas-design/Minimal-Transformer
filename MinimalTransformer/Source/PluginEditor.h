#pragma once

#include "PluginProcessor.h"
#include <array>
#include <memory>
#include <vector>

//==============================================================================
// Polices : on utilise Pacifico / Fredoka / Courier Prime si elles sont installées,
// sinon des polices système proches.
namespace DinerFonts
{
    inline juce::Font pickFont (std::initializer_list<const char*> names, float h, int style, bool& found)
    {
        static const juce::StringArray all = juce::Font::findAllTypefaceNames();
        for (auto* n : names)
            if (all.contains (n, true))
            {
                found = true;
                return juce::Font (juce::FontOptions (n, h, style));
            }
        found = false;
        return juce::Font (juce::FontOptions (h, style));
    }

    inline juce::Font script (float h)
    {
        bool found = false;
        auto f = pickFont ({ "Pacifico", "Segoe Script", "Brush Script MT", "Snell Roundhand",
                             "Lucida Handwriting", "Apple Chancery" }, h, juce::Font::plain, found);
        if (! found)
            return juce::Font (juce::FontOptions (h, juce::Font::bold | juce::Font::italic));
        return f;
    }

    inline juce::Font label (float h)
    {
        bool found = false;
        auto f = pickFont ({ "Fredoka", "Trebuchet MS", "Verdana", "Helvetica Neue" }, h, juce::Font::bold, found);
        if (! found)
            return juce::Font (juce::FontOptions (h, juce::Font::bold));
        return f;
    }

    inline juce::Font mono (float h)
    {
        bool found = false;
        auto f = pickFont ({ "Courier Prime", "Courier New", "Menlo", "Consolas" }, h, juce::Font::bold, found);
        if (! found)
            return juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), h, juce::Font::bold));
        return f;
    }
}

//==============================================================================
class DinerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DinerLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;
    juce::Font getPopupMenuFont() override;
};

//==============================================================================
// 16 colonnes de 6 ampoules : cliquer / glisser pour dessiner la séquence
class StepBulbs : public juce::Component
{
public:
    StepBulbs (juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, juce::Colour colour);

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

    void setActiveStep (int s);
    void refresh();

private:
    static constexpr float padX = 8.0f;
    static constexpr float padTop = 4.0f;
    static constexpr float cell = 14.0f;
    static constexpr int rows = 6;

    int columnAt (float x) const;
    void setFromMouse (const juce::MouseEvent&);
    void setStep (int column, float value);

    std::array<juce::RangedAudioParameter*, 16> params {};
    std::array<float, 16> values {};
    std::array<bool, 16> touched {};
    juce::Colour colour;
    int active = -1;
};

//==============================================================================
class MinimalTransformerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                               private juce::Timer
{
public:
    explicit MinimalTransformerAudioProcessorEditor (MinimalTransformerAudioProcessor&);
    ~MinimalTransformerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment   = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    struct Combo
    {
        juce::ComboBox box;
        juce::Label label;
        std::unique_ptr<ComboBoxAttachment> attachment;
    };

    struct Plate
    {
        juce::Rectangle<int> r;
        juce::String title;
        juce::Colour colour;
    };

    void setupKnob (Knob& k, const juce::String& paramId, const juce::String& text, juce::Colour accent);
    void setupCombo (Combo& c, const juce::String& paramId, const juce::String& text);
    void changePreset (int delta);
    void timerCallback() override;
    void buildNeon();

    MinimalTransformerAudioProcessor& proc;

    DinerLookAndFeel laf;

    // Barre de presets
    juce::ComboBox presetBox;
    juce::TextButton prevButton { "<" }, nextButton { ">" };

    // 0 cutoff, 1 reso, 2 morph, 3 drive, 4 envAmt, 5 envAtk, 6 envRel, 7 mix, 8 output,
    // 9 lfoDepth, 10 lfoRate, 11 stereo, 12 crush, 13 dlyFb, 14 dlySend,
    // 15-17 : seq A (len, glide, depth)   18-20 : seq B (len, glide, depth)
    std::array<Knob, 21> knobs;

    // 0 lfoShape, 1 lfoSync, 2 dlyTime, 3 aRate, 4 aDest, 5 bRate, 6 bDest
    std::array<Combo, 7> combos;

    StepBulbs bulbsA, bulbsB;

    // 0 filter, 1 envelope, 2 output, 3 lfo, 4 bitcrush+delay, 5 seq A, 6 seq B
    std::array<Plate, 7> plates;

    juce::Rectangle<int> headerRect, checkerRect, lampRect;
    juce::Path titlePath, tagPath, lampPath;
    juce::Image glowImage, dotTile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinimalTransformerAudioProcessorEditor)
};
