/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 18 Jun 2024 2:25:40pm
    Author:  MacJay

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RotaryKnob.h"

static constexpr int labelHeight = 15;
static constexpr int textBoxHeight = 20;
//==============================================================================
RotaryKnob::RotaryKnob()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, textBoxHeight);
    addAndMakeVisible(slider);
    
//    Initial size which can be overridden by the layout code
//    in its parent component.
    setBounds(0, 0, 100, 120);

}

RotaryKnob::~RotaryKnob()
{
}

void RotaryKnob::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//   clear the background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    auto bounds = getLocalBounds();
//    This uses getLocalBounds to find out how large the component is, in particular how
//   wide, and then it draws the text from the label variable centered at the top.
    g.drawText (label, juce::Rectangle<int>{ 0, 0, bounds.getWidth(), labelHeight },
                juce::Justification::centred);
    
//    Drawing an outline around the entire component.
    g.setColour(juce::Colours::red);
    g.drawRect(getLocalBounds(), 1);
    
    /*
    g.setColour(juce::Colours::yellow);
    g.drawRect(0, labelHeight, bounds.getWidth(),
               bounds.getHeight() - labelHeight - textBoxHeight, 1);
    g.setColour(juce::Colours::green);
    g.drawRect(0, 0, bounds.getWidth(), labelHeight, 1);
     */
}

void RotaryKnob::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
//    Getting a juce::Rectangle object with the current width and
//    height of the RotaryKnob component.
    auto bounds = getLocalBounds();
//    Extra space at the top in order to set the label.
    slider.setBounds(0, labelHeight, bounds.getWidth(), bounds.getHeight() - labelHeight);

}
