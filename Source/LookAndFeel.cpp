/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 18 Jun 2024 4:08:15pm
    Author:  MacJay

  ==============================================================================
*/

#include "LookAndFeel.h"

LookAndFeel::LookAndFeel() 
{
//    Using font from BinaryData
//    It constructs a new juce::Typeface object from the pointer to the
//    font data, and then makes it the standard font.
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                    BinaryData::LatoMedium_ttf, BinaryData::LatoMedium_ttfSize);
    setDefaultSansSerifTypeface(typeface);
    
//    Slider
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(65, 70, 80));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0, 0, 0));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    setColour(juce::Slider::thumbColourId, juce::Colours::orange);
    
//    Button
    setColour(juce::TextButton::buttonColourId, juce::Colour(15, 30, 45));
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::orange);
    setColour(juce::TextButton::textColourOffId, juce::Colour(180, 180, 180));
    setColour(juce::TextButton::textColourOnId, juce::Colour(255, 255, 255));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(180, 180, 180));
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                      float sliderPos, float rotaryStartAngle,
                      float rotaryEndAngle, juce::Slider& slider) 
{
    auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin (8.0f, radius * 0.1f);
    auto arcRadius = radius - lineW * 0.5f;
    

//    Created Dial path background.
    juce::Path backgroundArc;
//    Adding centered arc.
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);
//    Set color.
    g.setColour (outline);
//    Drawing the arc.
    g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
//        Create Dial fill path.
        juce::Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);
//        Color
        g.setColour (fill);
//        Drawing
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
//    Create Thumb of the dial.
    juce::Point<float> thumbPoint (bounds.getCentreX() + (arcRadius - 10.0f) * std::cos (toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + (arcRadius- 10.0f) * std::sin (toAngle - juce::MathConstants<float>::halfPi));
//    Color of the point.
    g.setColour (slider.findColour (juce::Slider::thumbColourId));
//    Drawing a circle:
//    Drawing a pointer from the center of the arc to the thumb
    g.drawLine(backgroundArc.getBounds().getCentreX(), backgroundArc.getBounds().getCentreY(), thumbPoint.getX(), thumbPoint.getY(), lineW);
}
