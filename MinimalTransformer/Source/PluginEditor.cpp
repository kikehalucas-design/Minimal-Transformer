#include "PluginEditor.h"
#include <cmath>

namespace
{
    // Cadre chromé + ombre portée autour d'un rectangle
    void drawRings (juce::Graphics& g, juce::Rectangle<float> r, float radius)
    {
        g.setColour (juce::Colour (0x40283c37));
        g.fillRoundedRectangle (r.translated (0.0f, 5.0f).expanded (5.0f), radius + 5.0f);
        g.setColour (juce::Colour (0xff8e949a));
        g.fillRoundedRectangle (r.expanded (5.0f), radius + 5.0f);
        g.setColour (juce::Colour (0xffd9dde0));
        g.fillRoundedRectangle (r.expanded (3.0f), radius + 3.0f);
    }

    float spacedWidth (const juce::Font& f, const juce::String& s, float spacing)
    {
        float w = 0.0f;
        for (int i = 0; i < s.length(); ++i)
            w += juce::GlyphArrangement::getStringWidth (f, juce::String::charToString (s[i])) + spacing;
        return w - spacing;
    }

    void drawSpaced (juce::Graphics& g, const juce::String& s, const juce::Font& f,
                     float x, float y, float h, float spacing)
    {
        g.setFont (f);
        for (int i = 0; i < s.length(); ++i)
        {
            const auto ch = juce::String::charToString (s[i]);
            const float cw = juce::GlyphArrangement::getStringWidth (f, ch);
            g.drawText (ch, juce::Rectangle<float> (x, y, cw + 2.0f, h), juce::Justification::centredLeft, false);
            x += cw + spacing;
        }
    }
}

//==============================================================================
// Look and feel "diner"
//==============================================================================
DinerLookAndFeel::DinerLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1d1a24));
    setColour (juce::PopupMenu::textColourId, juce::Colour (0xff7ff5d6));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xffff5fa2));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colour (0xff1d1a24));
    setColour (juce::TextButton::textColourOffId, juce::Colour (0xff2a2326));
    setColour (juce::TextButton::textColourOnId, juce::Colour (0xff2a2326));
}

void DinerLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float pos, float startA, float endA, juce::Slider& s)
{
    const float d = (float) juce::jmin (width, height) - 4.0f;
    const auto c = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).getCentre();
    const juce::Rectangle<float> r (c.x - d * 0.5f, c.y - d * 0.5f, d, d);

    // ombre
    g.setColour (juce::Colour (0x66281e1e));
    g.fillEllipse (r.translated (0.0f, 4.0f));

    // disque chromé
    juce::ColourGradient chrome (juce::Colour (0xfffafafa), r.getX(), r.getY(),
                                 juce::Colour (0xff80868c), r.getRight(), r.getBottom(), false);
    chrome.addColour (0.30, juce::Colour (0xff9aa0a6));
    chrome.addColour (0.50, juce::Colours::white);
    chrome.addColour (0.75, juce::Colour (0xff80868c));
    g.setGradientFill (chrome);
    g.fillEllipse (r);

    g.setColour (juce::Colour (0xff6d7378));
    g.drawEllipse (r, 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.drawEllipse (r.reduced (3.0f), 1.5f);

    // capuchon coloré
    const auto accent = s.findColour (juce::Slider::rotarySliderFillColourId);
    const auto cap = r.reduced (d * 0.27f);
    juce::ColourGradient capG (juce::Colours::white, cap.getX() + cap.getWidth() * 0.35f,
                               cap.getY() + cap.getHeight() * 0.30f,
                               accent, cap.getRight(), cap.getBottom(), true);
    g.setGradientFill (capG);
    g.fillEllipse (cap);
    g.setColour (juce::Colour (0xff6d7378));
    g.drawEllipse (cap, 2.0f);

    // repère rose foncé
    const float angle = startA + pos * (endA - startA);
    const float rad = d * 0.5f;
    juce::Path p;
    p.addRoundedRectangle (-2.5f, -rad + 3.0f, 5.0f, rad * 0.45f, 2.5f);
    g.setColour (juce::Colour (0xffb0245a));
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
}

void DinerLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int,
                                     juce::ComboBox& box)
{
    const bool card = (bool) box.getProperties()["card"];
    const auto r = juce::Rectangle<float> (0.0f, 0.0f, (float) w, (float) h).reduced (2.0f);

    g.setColour (juce::Colour (0xffc4c9cd));
    g.fillRoundedRectangle (r.expanded (1.5f), 7.0f);
    g.setColour (card ? juce::Colour (0xfffbf1d3) : juce::Colour (0xff1d1a24));
    g.fillRoundedRectangle (r, card ? 4.0f : 6.0f);

    const float cx = (float) w - 14.0f, cy = (float) h * 0.5f;
    juce::Path arrow;
    arrow.addTriangle (cx - 4.0f, cy - 2.0f, cx + 4.0f, cy - 2.0f, cx, cy + 3.0f);
    g.setColour (card ? juce::Colour (0xffb0245a) : juce::Colour (0xff7ff5d6));
    g.fillPath (arrow);
}

juce::Font DinerLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return DinerFonts::mono (14.5f);
}

void DinerLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (8, 1, box.getWidth() - 26, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
}

void DinerLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&,
                                             bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (3.0f);
    const float d = juce::jmin (r.getWidth(), r.getHeight());
    r = juce::Rectangle<float> (d, d).withCentre (r.getCentre());

    g.setColour (juce::Colour (0xff6d7378));
    g.fillEllipse (r.translated (0.0f, down ? 1.0f : 3.0f));

    juce::ColourGradient grad (juce::Colours::white, r.getX() + d * 0.35f, r.getY() + d * 0.30f,
                               juce::Colour (0xff8e949a), r.getRight(), r.getBottom(), true);
    grad.addColour (0.70, juce::Colour (0xffb9bec3));
    g.setGradientFill (grad);
    g.fillEllipse (down ? r.translated (0.0f, 2.0f) : r);

    if (highlighted)
    {
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.fillEllipse (r);
    }
}

juce::Font DinerLookAndFeel::getTextButtonFont (juce::TextButton&, int)
{
    return DinerFonts::label (17.0f);
}

juce::Font DinerLookAndFeel::getPopupMenuFont()
{
    return DinerFonts::mono (15.0f);
}

//==============================================================================
// Colonnes d'ampoules (séquenceur)
//==============================================================================
StepBulbs::StepBulbs (juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, juce::Colour c)
    : colour (c)
{
    for (int i = 0; i < 16; ++i)
    {
        params[(size_t) i] = apvts.getParameter (prefix + juce::String (i + 1));
        values[(size_t) i] = params[(size_t) i] != nullptr ? params[(size_t) i]->getValue() : 0.0f;
        touched[(size_t) i] = false;
    }
}

void StepBulbs::setActiveStep (int s)
{
    if (s != active)
    {
        active = s;
        repaint();
    }
}

void StepBulbs::refresh()
{
    bool changed = false;
    for (size_t i = 0; i < 16; ++i)
    {
        if (params[i] == nullptr)
            continue;

        const float v = params[i]->getValue();
        if (std::abs (v - values[i]) > 0.0001f)
        {
            values[i] = v;
            changed = true;
        }
    }
    if (changed)
        repaint();
}

void StepBulbs::paint (juce::Graphics& g)
{
    const auto b = getLocalBounds().toFloat();

    g.setColour (juce::Colour (0xffc4c9cd));
    g.fillRoundedRectangle (b, 10.0f);
    g.setColour (juce::Colour (0xff1d1a24));
    g.fillRoundedRectangle (b.reduced (3.0f), 8.0f);

    const float colW = (b.getWidth() - 2.0f * padX) / 16.0f;
    const float top = padTop + 3.0f;
    const float bottom = top + (float) rows * cell;

    // séparateurs tous les 4 pas
    g.setColour (juce::Colour (0xff4a4254));
    for (int k = 1; k < 4; ++k)
        g.fillRect (padX + (float) (k * 4) * colW - 0.5f, top, 1.0f, (float) rows * cell);

    const auto numFont = DinerFonts::mono (11.0f);

    for (int c = 0; c < 16; ++c)
    {
        const float cx = padX + ((float) c + 0.5f) * colW;
        const int lit = juce::jlimit (0, rows, (int) std::lround (values[(size_t) c] * (float) rows));
        const auto lc = (c == active) ? juce::Colours::white : colour;

        for (int r = 0; r < rows; ++r)
        {
            const float cy = bottom - ((float) r + 0.5f) * cell;
            const juce::Rectangle<float> bulb (cx - 5.5f, cy - 5.5f, 11.0f, 11.0f);

            if (r < lit)
            {
                g.setColour (lc.withAlpha (0.22f));
                g.fillEllipse (bulb.expanded (3.0f));
                g.setColour (lc);
                g.fillEllipse (bulb);
                g.setColour (juce::Colours::white.withAlpha (0.9f));
                g.fillEllipse (bulb.reduced (4.0f));
            }
            else
            {
                g.setColour (juce::Colour (0xff3b3544));
                g.fillEllipse (bulb);
            }
        }

        g.setColour (juce::Colour (0xffb9b0c4));
        g.setFont (numFont);
        g.drawText (juce::String (c + 1), juce::Rectangle<float> (cx - colW * 0.5f, bottom + 3.0f, colW, 12.0f),
                    juce::Justification::centred, false);
    }
}

int StepBulbs::columnAt (float x) const
{
    const float colW = ((float) getWidth() - 2.0f * padX) / 16.0f;
    if (colW <= 0.0f)
        return -1;
    return juce::jlimit (0, 15, (int) std::floor ((x - padX) / colW));
}

void StepBulbs::setFromMouse (const juce::MouseEvent& e)
{
    const int c = columnAt ((float) e.x);
    if (c < 0)
        return;

    const float bottom = padTop + 3.0f + (float) rows * cell;
    int n = 0;
    if ((float) e.y < bottom)
        n = juce::jlimit (0, rows, (int) std::ceil ((bottom - (float) e.y) / cell));

    setStep (c, (float) n / (float) rows);
}

void StepBulbs::setStep (int c, float v)
{
    auto* p = params[(size_t) c];
    if (p == nullptr)
        return;

    if (! touched[(size_t) c])
    {
        p->beginChangeGesture();
        touched[(size_t) c] = true;
    }

    p->setValueNotifyingHost (p->convertTo0to1 (v));
    values[(size_t) c] = v;
    repaint();
}

void StepBulbs::mouseDown (const juce::MouseEvent& e) { setFromMouse (e); }
void StepBulbs::mouseDrag (const juce::MouseEvent& e) { setFromMouse (e); }

void StepBulbs::mouseUp (const juce::MouseEvent&)
{
    for (size_t i = 0; i < 16; ++i)
        if (touched[i])
        {
            if (params[i] != nullptr)
                params[i]->endChangeGesture();
            touched[i] = false;
        }
}

//==============================================================================
// Editeur
//==============================================================================
MinimalTransformerAudioProcessorEditor::MinimalTransformerAudioProcessorEditor (MinimalTransformerAudioProcessor& p)
    : juce::AudioProcessorEditor (&p),
      proc (p),
      bulbsA (p.apvts, "stepA", juce::Colour (0xffff5fa2)),
      bulbsB (p.apvts, "stepB", juce::Colour (0xff5fe3c3))
{
    setLookAndFeel (&laf);

    // petite texture de points (Formica)
    dotTile = juce::Image (juce::Image::ARGB, 14, 14, true);
    {
        juce::Graphics tg (dotTile);
        tg.setColour (juce::Colours::white.withAlpha (0.28f));
        tg.fillEllipse (6.0f, 6.0f, 2.0f, 2.0f);
    }

    // plaques
    const char* titles[7] = { "FILTER", "ENVELOPE", "OUTPUT", "LFO", "BITCRUSH + DELAY", "SEQUENCER A", "SEQUENCER B" };
    const juce::uint32 cols[7] = { 0xfff58fb0, 0xfff6d56b, 0xff8fe0c8, 0xfff7a98b, 0xfff6d56b, 0xffff5fa2, 0xff5fe3c3 };
    for (size_t i = 0; i < 7; ++i)
    {
        plates[i].title = titles[i];
        plates[i].colour = juce::Colour (cols[i]);
    }

    // ---- Barre de presets (étiquette de jukebox)
    for (int i = 0; i < proc.getNumPrograms(); ++i)
    {
        const juce::String code = juce::String::charToString ((juce::juce_wchar) ('A' + i / 10))
                                  + "-" + juce::String (i % 10 + 1);
        presetBox.addItem (code + "  " + proc.getProgramName (i).toUpperCase(), i + 1);
    }
    presetBox.getProperties().set ("card", true);
    presetBox.setColour (juce::ComboBox::textColourId, juce::Colour (0xff2a2326));
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);

    prevButton.onClick = [this] { changePreset (-1); };
    nextButton.onClick = [this] { changePreset (1); };
    addAndMakeVisible (prevButton);
    addAndMakeVisible (nextButton);

    // ---- Boutons
    const auto cFilter = juce::Colour (0xfff58fb0), cEnv = juce::Colour (0xfff6d56b), cOut = juce::Colour (0xff8fe0c8),
               cLfo = juce::Colour (0xfff7a98b), cFx = juce::Colour (0xfff6d56b),
               cSeqA = juce::Colour (0xffff5fa2), cSeqB = juce::Colour (0xff5fe3c3);

    setupKnob (knobs[0],  "cutoff",   "CUTOFF",    cFilter);
    setupKnob (knobs[1],  "reso",     "RESONANCE", cFilter);
    setupKnob (knobs[2],  "morph",    "MORPH",     cFilter);
    setupKnob (knobs[3],  "drive",    "DRIVE",     cFilter);
    setupKnob (knobs[4],  "envAmt",   "AMOUNT",    cEnv);
    setupKnob (knobs[5],  "envAtk",   "ATTACK",    cEnv);
    setupKnob (knobs[6],  "envRel",   "RELEASE",   cEnv);
    setupKnob (knobs[7],  "mix",      "MIX",       cOut);
    setupKnob (knobs[8],  "output",   "LEVEL",     cOut);
    setupKnob (knobs[9],  "lfoDepth", "DEPTH",     cLfo);
    setupKnob (knobs[10], "lfoRate",  "RATE",      cLfo);
    setupKnob (knobs[11], "stereo",   "STEREO",    cLfo);
    setupKnob (knobs[12], "crush",    "CRUSH",     cFx);
    setupKnob (knobs[13], "dlyFb",    "FEEDBACK",  cFx);
    setupKnob (knobs[14], "dlySend",  "SEND",      cFx);

    setupKnob (knobs[15], "aLen",   "LENGTH", cSeqA);
    setupKnob (knobs[16], "aGlide", "GLIDE",  cSeqA);
    setupKnob (knobs[17], "aDepth", "DEPTH",  cSeqA);
    setupKnob (knobs[18], "bLen",   "LENGTH", cSeqB);
    setupKnob (knobs[19], "bGlide", "GLIDE",  cSeqB);
    setupKnob (knobs[20], "bDepth", "DEPTH",  cSeqB);

    // ---- Menus déroulants
    setupCombo (combos[0], "lfoShape", "SHAPE");
    setupCombo (combos[1], "lfoSync",  "SYNC");
    setupCombo (combos[2], "dlyTime",  "TIME");
    setupCombo (combos[3], "aRate",    "RATE");
    setupCombo (combos[4], "aDest",    "DESTINATION");
    setupCombo (combos[5], "bRate",    "RATE");
    setupCombo (combos[6], "bDest",    "DESTINATION");

    addAndMakeVisible (bulbsA);
    addAndMakeVisible (bulbsB);

    setSize (1000, 680);
    startTimerHz (30);
}

MinimalTransformerAudioProcessorEditor::~MinimalTransformerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void MinimalTransformerAudioProcessorEditor::setupKnob (Knob& k, const juce::String& paramId,
                                                        const juce::String& text, juce::Colour accent)
{
    k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    k.slider.setRotaryParameters (juce::MathConstants<float>::pi * -0.75f,
                                  juce::MathConstants<float>::pi * 0.75f, true);
    k.slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    k.slider.setPopupDisplayEnabled (true, true, this);
    addAndMakeVisible (k.slider);

    k.label.setText (text, juce::dontSendNotification);
    k.label.setFont (DinerFonts::label (13.0f));
    k.label.setColour (juce::Label::textColourId, juce::Colour (0xff3a2a30));
    k.label.setJustificationType (juce::Justification::centred);
    k.label.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (k.label);

    k.attachment = std::make_unique<SliderAttachment> (proc.apvts, paramId, k.slider);
}

void MinimalTransformerAudioProcessorEditor::setupCombo (Combo& c, const juce::String& paramId,
                                                         const juce::String& text)
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*> (proc.apvts.getParameter (paramId)))
        c.box.addItemList (choiceParam->choices, 1);

    c.box.setColour (juce::ComboBox::textColourId, juce::Colour (0xff7ff5d6));
    addAndMakeVisible (c.box);

    c.label.setText (text, juce::dontSendNotification);
    c.label.setFont (DinerFonts::label (13.0f));
    c.label.setColour (juce::Label::textColourId, juce::Colour (0xff3a2a30));
    c.label.setJustificationType (juce::Justification::centred);
    c.label.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (c.label);

    c.attachment = std::make_unique<ComboBoxAttachment> (proc.apvts, paramId, c.box);
}

void MinimalTransformerAudioProcessorEditor::changePreset (int delta)
{
    const int n = presetBox.getNumItems();
    if (n <= 0)
        return;

    int id = presetBox.getSelectedId();
    if (id <= 0)
        id = 1;

    id = ((id - 1 + delta + n) % n) + 1;
    presetBox.setSelectedId (id); // déclenche onChange
}

void MinimalTransformerAudioProcessorEditor::timerCallback()
{
    bulbsA.refresh();
    bulbsB.refresh();
    bulbsA.setActiveStep (proc.stepIndexA.load());
    bulbsB.setActiveStep (proc.stepIndexB.load());
}

//==============================================================================
// Halo néon du titre, de la ligne d'accroche et du voyant (mis en cache)
void MinimalTransformerAudioProcessorEditor::buildNeon()
{
    const juce::String title ("Minimal Transformer");
    const juce::String tag ("HIM IS BACK NO PANIC");

    const auto titleFont = DinerFonts::script (38.0f);
    const auto tagFont = DinerFonts::label (15.0f);

    const float titleX = 50.0f;
    const float titleW = juce::GlyphArrangement::getStringWidth (titleFont, title);

    titlePath.clear();
    {
        juce::GlyphArrangement ga;
        ga.addLineOfText (titleFont, title, titleX, 58.0f);
        ga.createPath (titlePath);
    }

    // ligne d'accroche : lettres espacées, alignée sur le bord droit du titre
    const float spacing = 4.5f;
    float total = 0.0f;
    for (int i = 0; i < tag.length(); ++i)
        total += juce::GlyphArrangement::getStringWidth (tagFont, juce::String::charToString (tag[i])) + spacing;

    float x = titleX + titleW + 4.0f - (total - spacing);
    tagPath.clear();
    {
        juce::GlyphArrangement ga;
        for (int i = 0; i < tag.length(); ++i)
        {
            const auto ch = juce::String::charToString (tag[i]);
            ga.addLineOfText (tagFont, ch, x, 80.0f);
            x += juce::GlyphArrangement::getStringWidth (tagFont, ch) + spacing;
        }
        ga.createPath (tagPath);
    }

    lampPath.clear();
    lampPath.addEllipse (lampRect.toFloat());

    glowImage = juce::Image (juce::Image::ARGB, juce::jmax (1, getWidth()), juce::jmax (1, getHeight()), true);
    juce::Graphics gi (glowImage);

    const juce::Point<int> noOffset;
    for (int rad : { 4, 10, 22 })
        juce::DropShadow (juce::Colour (0xffff5fa2), rad, noOffset).drawForPath (gi, titlePath);
    for (int rad : { 3, 8, 16 })
        juce::DropShadow (juce::Colour (0xff5ff0cf), rad, noOffset).drawForPath (gi, tagPath);
    for (int rad : { 6, 14 })
        juce::DropShadow (juce::Colour (0xffff5fa2), rad, noOffset).drawForPath (gi, lampPath);
}

//==============================================================================
void MinimalTransformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff26252b));

    // cadre chromé
    {
        juce::ColourGradient chrome (juce::Colours::white, 6.0f, 6.0f, juce::Colour (0xffe8eaec), 994.0f, 674.0f, false);
        chrome.addColour (0.30, juce::Colour (0xffa9aeb3));
        chrome.addColour (0.50, juce::Colour (0xfff4f5f6));
        chrome.addColour (0.75, juce::Colour (0xff8e949a));
        g.setGradientFill (chrome);
        g.fillRoundedRectangle (6.0f, 6.0f, 988.0f, 668.0f, 22.0f);
    }

    // fond turquoise pastel façon Formica
    const juce::Rectangle<float> mint (12.0f, 12.0f, 976.0f, 656.0f);
    g.setColour (juce::Colour (0xff86d6c6));
    g.fillRoundedRectangle (mint, 17.0f);
    g.setTiledImageFill (dotTile, 12, 12, 1.0f);
    g.fillRoundedRectangle (mint, 17.0f);
    g.setColour (juce::Colour (0xff5fb3a2));
    g.drawRoundedRectangle (mint.reduced (1.5f), 16.0f, 3.0f);

    // ---- Enseigne néon
    drawRings (g, headerRect.toFloat(), 14.0f);
    g.setColour (juce::Colour (0xff1d1a24));
    g.fillRoundedRectangle (headerRect.toFloat(), 14.0f);

    g.saveState();
    {
        juce::Path clip;
        clip.addRoundedRectangle (headerRect.toFloat(), 14.0f);
        g.reduceClipRegion (clip, juce::AffineTransform());
        g.drawImageAt (glowImage, 0, 0);
    }
    g.restoreState();

    g.setColour (juce::Colour (0xffffe3ee));
    g.fillPath (titlePath);
    g.setColour (juce::Colour (0xffe6fff9));
    g.fillPath (tagPath);

    // voyant rose
    {
        const auto lamp = lampRect.toFloat();
        g.setColour (juce::Colour (0xff8e949a));
        g.fillEllipse (lamp.expanded (3.0f));
        juce::ColourGradient lg (juce::Colours::white, lamp.getX() + 5.0f, lamp.getY() + 4.0f,
                                 juce::Colour (0xffc2327a), lamp.getRight(), lamp.getBottom(), true);
        lg.addColour (0.55, juce::Colour (0xffff7db4));
        g.setGradientFill (lg);
        g.fillEllipse (lamp);
    }

    // ---- Plaques crème bordées de chrome
    const auto tabFont = DinerFonts::label (13.0f);
    for (const auto& p : plates)
    {
        const auto r = p.r.toFloat();
        drawRings (g, r, 14.0f);
        g.setColour (juce::Colour (0xfffbf3dc));
        g.fillRoundedRectangle (r, 14.0f);

        // languette de couleur
        const float tw = spacedWidth (tabFont, p.title, 2.0f);
        const juce::Rectangle<float> tab (r.getX() + 16.0f, r.getY() - 2.0f, tw + 32.0f, 22.0f);
        juce::Path tp;
        tp.addRoundedRectangle (tab.getX(), tab.getY(), tab.getWidth(), tab.getHeight(),
                                12.0f, 12.0f, false, false, true, true);
        g.setColour (p.colour);
        g.fillPath (tp);
        g.setColour (juce::Colour (0xff34172a));
        drawSpaced (g, p.title, tabFont, tab.getX() + 16.0f, tab.getY() + 1.0f, 20.0f, 2.0f);
    }

    // ---- Damier noir et crème en bas
    g.setColour (juce::Colour (0xff5fb3a2));
    g.fillRoundedRectangle (checkerRect.toFloat().expanded (2.0f), 4.0f);
    const int right = checkerRect.getRight();
    for (int cx = 0; cx * 8 < checkerRect.getWidth(); ++cx)
        for (int cy = 0; cy < 2; ++cy)
        {
            const int x = checkerRect.getX() + cx * 8;
            g.setColour (((cx + cy) % 2 == 0) ? juce::Colour (0xfff3ead2) : juce::Colour (0xff222222));
            g.fillRect (x, checkerRect.getY() + cy * 8, juce::jmin (8, right - x), 8);
        }
}

void MinimalTransformerAudioProcessorEditor::resized()
{
    const int L = 30, W = 940;

    headerRect = { L, 24, W, 64 };
    prevButton.setBounds (580, 39, 34, 34);
    presetBox.setBounds (622, 39, 250, 34);
    nextButton.setBounds (880, 39, 34, 34);
    lampRect = { 934, 48, 16, 16 };

    plates[0].r = { L, 101, 405, 110 };
    plates[1].r = { L + 405 + 14, 101, 304, 110 };
    plates[2].r = { L + 405 + 14 + 304 + 14, 101, 203, 110 };
    plates[3].r = { L, 220, 514, 110 };
    plates[4].r = { L + 514 + 14, 220, 412, 110 };
    plates[5].r = { L, 343, W, 140 };
    plates[6].r = { L, 492, W, 140 };
    checkerRect = { L, 641, W, 16 };

    auto placeKnob = [] (Knob& k, juce::Rectangle<int> cellR, int top, int size)
    {
        k.slider.setBounds (cellR.getCentreX() - size / 2, top, size, size);
        k.label.setBounds (cellR.getX(), top + size + 6, cellR.getWidth(), 16);
    };

    auto placeCombo = [] (Combo& c, juce::Rectangle<int> cellR, int top, int w, int h)
    {
        c.box.setBounds (cellR.getCentreX() - w / 2, top, w, h);
        c.label.setBounds (cellR.getX(), top + h + 5, cellR.getWidth(), 16);
    };

    // plaques de réglages : cellules de largeur égale
    auto layoutPlate = [&] (const Plate& p, std::initializer_list<std::pair<bool, int>> items)
    {
        const int n = (int) items.size();
        const int cellW = (p.r.getWidth() - 12) / n;
        int i = 0;
        for (const auto& it : items)
        {
            const juce::Rectangle<int> cellR (p.r.getX() + 6 + i * cellW, p.r.getY(), cellW, p.r.getHeight());
            if (it.first)
                placeCombo (combos[(size_t) it.second], cellR, p.r.getY() + 43, 96, 31);
            else
                placeKnob (knobs[(size_t) it.second], cellR, p.r.getY() + 25, 56);
            ++i;
        }
    };

    layoutPlate (plates[0], { { false, 0 }, { false, 1 }, { false, 2 }, { false, 3 } });
    layoutPlate (plates[1], { { false, 4 }, { false, 5 }, { false, 6 } });
    layoutPlate (plates[2], { { false, 7 }, { false, 8 } });
    layoutPlate (plates[3], { { false, 9 }, { false, 10 }, { false, 11 }, { true, 0 }, { true, 1 } });
    layoutPlate (plates[4], { { false, 12 }, { true, 2 }, { false, 13 }, { false, 14 } });

    // plaques des séquenceurs
    for (int l = 0; l < 2; ++l)
    {
        const auto p = plates[(size_t) (5 + l)].r;
        const int lx = p.getX() + 14;
        const int cw = (290 - 8) / 2;

        // vitesse + destination
        for (int k = 0; k < 2; ++k)
        {
            auto& c = combos[(size_t) (3 + 2 * l + k)];
            const int cx = lx + k * (cw + 8);
            c.box.setBounds (cx, p.getY() + 26, cw, 29);
            c.label.setBounds (cx, p.getY() + 26 + 29 + 4, cw, 16);
        }

        // longueur, glide, intensité
        for (int k = 0; k < 3; ++k)
        {
            const juce::Rectangle<int> cellR (lx + k * 96, p.getY(), 96, p.getHeight());
            placeKnob (knobs[(size_t) (15 + 3 * l + k)], cellR, p.getY() + 80, 34);
        }

        auto& bulbs = (l == 0) ? bulbsA : bulbsB;
        bulbs.setBounds (p.getX() + 322, p.getY() + 25, p.getWidth() - 322 - 14, 107);
    }

    buildNeon();
}
