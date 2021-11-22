/*
  ==============================================================================

    Components.h
    Created: 4 Feb 2021 1:34:43pm
    Author:  Daniel Faronbi
    Use:    GUI components for synthesizer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "sliders.h"

//Gui for nav bar
class Nav_Bar : public juce::AnimatedAppComponent, public juce::ChangeBroadcaster, public juce::Button::Listener
{
public:
    //==============================================================================
    Nav_Bar()
    {
        //create images for oscillator button
        const juce::Image osc_img = juce::ImageCache::getFromMemory(BinaryData::osc_png,BinaryData::osc_pngSize);
        const juce::Image osc_hover_img = juce::ImageCache::getFromMemory(BinaryData::osc_hover_png,BinaryData::osc_hover_pngSize);
        const juce::Image osc_clicked_img = juce::ImageCache::getFromMemory(BinaryData::osc_clicked_png,BinaryData::osc_clicked_pngSize);
        
        //setup ocs select button
        select_osc.setImages(false, true, false, osc_img, 1.0f, juce::Colours::transparentBlack, osc_hover_img, 1.0f, juce::Colours::transparentBlack, osc_clicked_img, 1.0f, juce::Colours::black);
        
        //create images for oscillator button
        const juce::Image filt_img = juce::ImageCache::getFromMemory(BinaryData::filt_png,BinaryData::filt_pngSize);
        const juce::Image filt_hover_img = juce::ImageCache::getFromMemory(BinaryData::filt_hover_png,BinaryData::filt_hover_pngSize);
        const juce::Image filt_clicked_img = juce::ImageCache::getFromMemory(BinaryData::filt_clicked_png,BinaryData::filt_clicked_pngSize);
        
        //setup filter select button
        select_filt.setImages(false, true,false, filt_img, 1.0f, juce::Colours::transparentBlack, filt_hover_img, 1.0f, juce::Colours::transparentBlack, filt_clicked_img, 1.0f, juce::Colours::black);
        
       //create images for amplifier button
        const juce::Image amp_img = juce::ImageCache::getFromMemory(BinaryData::amp_png,BinaryData::amp_pngSize);
        const juce::Image amp_hover_img = juce::ImageCache::getFromMemory(BinaryData::amp_hover_png,BinaryData::amp_hover_pngSize);
        const juce::Image amp_clicked_img = juce::ImageCache::getFromMemory(BinaryData::amp_clicked_png,BinaryData::amp_clicked_pngSize);

        //setup amplifier select button
        select_amp.setImages(false, true, false, amp_img, 1.0f, juce::Colours::transparentBlack, amp_hover_img, 1.0f, juce::Colours::transparentBlack, amp_clicked_img, 1.0f, juce::Colours::black);
        
        //setup 
        addAndMakeVisible(select_osc);
        addAndMakeVisible(select_filt);
        addAndMakeVisible(select_amp);
        
        //add button listners
        select_osc.addListener(this);
        select_filt.addListener(this);
        select_amp.addListener(this);
        
    }
    
    ~Nav_Bar(){
        //remove button listners
        select_osc.removeListener(this);
        select_filt.removeListener(this);
        select_amp.removeListener(this);
    }
    
    

    void update() override
    {
        // This function is called at the frequency specified by the setFramesPerSecond() call
        // in the constructor. You can use it to update counters, animate values, etc.
    }

    void paint (juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (juce::Colours::darkcyan);
        g.setColour (juce::Colours::peachpuff);
    }

    void resized() override
    {
        
        //get total area of nav bar
        auto area = getLocalBounds();
        
        //each button takes one third of nav area
        select_osc.setBounds(area.removeFromTop(getHeight()/3));
        select_filt.setBounds(area.removeFromTop(getHeight()/3));
        select_amp.setBounds(area.removeFromTop(getHeight()/3));
        
        
    }
    juce::ImageButton select_osc;
    juce::ImageButton select_filt;
    juce::ImageButton select_amp;
    
    void buttonClicked (juce::Button* button) override
    {
        if (button == &select_osc)                                                      // [3]
        {
            selected_screen = 0;
        }
        
        if (button == &select_filt)                                                      // [3]
        {
            selected_screen = 1;
        }
        
        if (button == &select_amp)                                                      // [3]
        {
            selected_screen = 2;
        }
        sendChangeMessage();
    }
    
    int get_button_click()
    {
        return selected_screen;
    }

private:
    int selected_screen = 0;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Nav_Bar)
};

//gui for oscillator menu
class Oscillator_Menu : public juce::AnimatedAppComponent, public juce::ChangeBroadcaster, public juce::Slider::Listener
{
public:
    //create components for oscillator 1
    juce::Label osc1_wav_shape_label;
    juce::ComboBox osc1_wav_shape;
    juce::Slider osc1_frequency;
    gain_slider osc1_gain;
    
    //create components for oscillator 2
    juce::Label osc2_wav_shape_label;
    juce::ComboBox osc2_wav_shape;
    juce::Slider osc2_frequency;
    gain_slider  osc2_gain;
    
    //create components for oscillator 3
    juce::Label osc3_wav_shape_label;
    juce::ComboBox osc3_wav_shape;
    juce::Slider osc3_frequency;
    gain_slider  osc3_gain;
    
    //==============================================================================
    Oscillator_Menu()
    {
        //set labels text
        osc1_wav_shape_label.setText("Osc 1 Wave Shape", juce::dontSendNotification);
        osc2_wav_shape_label.setText("Osc 2 Wave Shape", juce::dontSendNotification);
        osc3_wav_shape_label.setText("Osc 3 Wave Shape", juce::dontSendNotification);
        
        //set combo box items
        osc1_wav_shape.addItem("Sine",1);
        osc1_wav_shape.addItem("Saw",2);
        osc1_wav_shape.addItem("Square",3);
        osc1_wav_shape.addItem("Triangle",4);
        osc1_wav_shape.addItem("White Noise",5);
        osc1_wav_shape.setSelectedId(1);
        
        osc2_wav_shape.addItem("Sine",1);
        osc2_wav_shape.addItem("Saw",2);
        osc2_wav_shape.addItem("Square",3);
        osc2_wav_shape.addItem("Triangle",4);
        osc2_wav_shape.addItem("White Noise",5);
        osc2_wav_shape.setSelectedId(1);
        
        osc3_wav_shape.addItem("Sine",1);
        osc3_wav_shape.addItem("Saw",2);
        osc3_wav_shape.addItem("Square",3);
        osc3_wav_shape.addItem("Triangle",4);
        osc3_wav_shape.addItem("White Noise",5);
        osc3_wav_shape.setSelectedId(1);
        
        //set freq slider settings
        osc1_frequency.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        osc1_frequency.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc1_frequency.setTextValueSuffix (" Frequency (Hz)");
        osc1_frequency.setRange(20, 20000);
        
        osc2_frequency.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        osc2_frequency.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc2_frequency.setTextValueSuffix (" Frequency (Hz)");
        osc2_frequency.setRange(20, 20000);
        
        osc3_frequency.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        osc3_frequency.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc3_frequency.setTextValueSuffix (" Frequency (Hz)");
        osc3_frequency.setRange(20, 20000);
        
        //set gain slider settings
        osc1_gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        osc1_gain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc1_gain.setTextValueSuffix (" gain (dB)");
        osc1_gain.setRange(-100, 50);
        
        osc2_gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        osc2_gain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc2_gain.setTextValueSuffix (" gain (dB)");
        osc2_gain.setRange(-100, 50);
        
        osc3_gain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        osc3_gain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        osc3_gain.setTextValueSuffix (" gain (dB)");
        osc3_gain.setRange(-100, 50);
        
        
        //add child components
        addAndMakeVisible(osc1_wav_shape_label);
        addAndMakeVisible(osc1_wav_shape);
        addAndMakeVisible(osc1_frequency);
        addAndMakeVisible(osc1_gain);
        
        addAndMakeVisible(osc2_wav_shape_label);
        addAndMakeVisible(osc2_wav_shape);
        addAndMakeVisible(osc2_frequency);
        addAndMakeVisible(osc2_gain);
        
        addAndMakeVisible(osc3_wav_shape_label);
        addAndMakeVisible(osc3_wav_shape);
        addAndMakeVisible(osc3_frequency);
        addAndMakeVisible(osc3_gain);
        
        //add slider listeners to object
        osc1_frequency.addListener(this);
        osc2_frequency.addListener(this);
        osc3_frequency.addListener(this);
        
    }
    
    ~Oscillator_Menu(){
        //remove sliderlisteneres
        osc1_frequency.removeListener(this);
        osc2_frequency.removeListener(this);
        osc3_frequency.removeListener(this);
    }

    void update() override
    {
        // This function is called at the frequency specified by the setFramesPerSecond() call
        // in the constructor. You can use it to update counters, animate values, etc.
    }

    void paint (juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::peachpuff);
    }

    void resized() override
    {
        //get total area of nav bar
        auto area = getLocalBounds();
        
        //flexboxes to organize items
        juce::FlexBox fb;
        juce::FlexBox row1;
        juce::FlexBox row2;
        juce::FlexBox row3;
        
        
        //set rows on vertical axis direction
        fb.flexDirection = juce::FlexBox::Direction::row;
        row1.flexDirection = juce::FlexBox::Direction::column;
        row2.flexDirection = juce::FlexBox::Direction::column;
        row3.flexDirection = juce::FlexBox::Direction::column;
        
        //set wrapping of flex boxes
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        row1.flexWrap = juce::FlexBox::Wrap::noWrap;
        row2.flexWrap = juce::FlexBox::Wrap::noWrap;
        row3.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        //set content justification (main axis)
        fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row1.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row2.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row3.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        
        //set content alignment (cross axis)
        fb.alignContent = juce::FlexBox::AlignContent::flexStart;
        row1.alignContent = juce::FlexBox::AlignContent::flexStart;
        row2.alignContent = juce::FlexBox::AlignContent::flexStart;
        row3.alignContent = juce::FlexBox::AlignContent::flexStart;
        
        auto osc_label_height = area.getHeight()/10;
        auto osc_type_height = area.getHeight()/10;
        auto osc_slider_height = 5*area.getHeight()/10;
        auto osc_knob_height = 3*area.getHeight()/10;
        
        //add items to row1
        row1.items.add(juce::FlexItem(osc1_wav_shape_label).withMinWidth(area.getWidth()/3).withMinHeight(osc_label_height));
        row1.items.add(juce::FlexItem(osc1_wav_shape).withMinWidth(area.getWidth()/3).withMinHeight(osc_type_height));
        row1.items.add(juce::FlexItem(osc1_frequency).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_slider_height));
        row1.items.add(juce::FlexItem(osc1_gain).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_knob_height));
        
        //add items to row2
        row2.items.add(juce::FlexItem(osc2_wav_shape_label).withMinWidth(area.getWidth()/3).withMinHeight(osc_label_height));
        row2.items.add(juce::FlexItem(osc2_wav_shape).withMinWidth(area.getWidth()/3).withMinHeight(osc_type_height));
        row2.items.add(juce::FlexItem(osc2_frequency).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_slider_height));
        row2.items.add(juce::FlexItem(osc2_gain).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_knob_height));
        
        //add items to rown3
        row3.items.add(juce::FlexItem(osc3_wav_shape_label).withMinWidth(area.getWidth()/3).withMinHeight(osc_label_height));
        row3.items.add(juce::FlexItem(osc3_wav_shape).withMinWidth(area.getWidth()/3).withMinHeight(osc_type_height));
        row3.items.add(juce::FlexItem(osc3_frequency).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_slider_height));
        row3.items.add(juce::FlexItem(osc3_gain).withMinWidth(area.getWidth()/3 ).withMinHeight(osc_knob_height));
        
        
        //add items to main flex box
        fb.items.add(juce::FlexItem(row1).withFlex(1.0));
        fb.items.add(juce::FlexItem(row2).withFlex(1.0));
        fb.items.add(juce::FlexItem(row3).withFlex(1.0));
        
        fb.performLayout (getLocalBounds().reduced(10).toFloat());

    }
    
    void sliderValueChanged (juce::Slider* slider) override {
        
        //send a message  that slider value is changed
        sendChangeMessage();
        
    }
    
    

private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator_Menu)
};

//gui for filter menu
class Filter_Menu : public juce::AnimatedAppComponent, public juce::ChangeBroadcaster, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:
    
    //create components for filter 1
    juce::Label filter1_label;
    juce::ComboBox filter1_type;
    juce::Slider filter1_cuttoff_freq;
    juce::Slider filter1_resonance;
    
    //create components for filter 2
    juce::Label filter2_label;
    juce::ComboBox filter2_type;
    juce::Slider filter2_cuttoff_freq;
    juce::Slider filter2_resonance;
    
    //create components for  asdr
    juce::Slider filter_attack;
    juce::Slider filter_decay;
    juce::Slider filter_sustain;
    juce::Slider filter_release;
    
    //==============================================================================
    Filter_Menu()
    {
        //set labels text
        filter1_label.setText("Filter 1 Type", juce::dontSendNotification);
        filter2_label.setText("Filter 2 Type", juce::dontSendNotification);
        
        //set combo box items
        filter1_type.addItem("Low Pass",1);
        filter1_type.addItem("High Pass",2);
        filter1_type.addItem("Band Pass",3);
        filter1_type.addItem("Band Reject",4);
        filter1_type.setSelectedId(1);
        
        filter2_type.addItem("Low Pass",1);
        filter2_type.addItem("High Pass",2);
        filter2_type.addItem("Band Pass",3);
        filter2_type.addItem("Band Reject",4);
        filter2_type.setSelectedId(1);
        
        //set cuttoff freq slider settings
        filter1_cuttoff_freq.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filter1_cuttoff_freq.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter1_cuttoff_freq.setTextValueSuffix (" Cutoff Freq (Hz)");
        filter1_cuttoff_freq.setRange(20, 20000);
        
        filter2_cuttoff_freq.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filter2_cuttoff_freq.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter2_cuttoff_freq.setTextValueSuffix (" Cutoff Freq (Hz)");
        filter2_cuttoff_freq.setRange(20, 20000);
        
        //set cuttoff freq slider settings
        filter1_resonance.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filter1_resonance.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter1_resonance.setTextValueSuffix (" Resonance");
        filter1_resonance.setRange(0, 50);
        
        filter2_resonance.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filter2_resonance.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter2_resonance.setTextValueSuffix (" Resonance");
        filter2_resonance.setRange(0, 50);
        
        //set asdr slider settings
        filter_attack.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        filter_attack.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter_attack.setTextValueSuffix (" Attack Time (s)");
        filter_attack.setRange(0, 0.5);
        
        filter_decay.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        filter_decay.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter_decay.setTextValueSuffix (" Decay Time (s)");
        filter_decay.setRange(0, 0.5);
        
        filter_sustain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        filter_sustain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter_sustain.setTextValueSuffix (" Sustain Level (fraction)");
        filter_sustain.setRange(0, 1);
        
        
        filter_release.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        filter_release.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        filter_release.setTextValueSuffix (" Release Time (s)");
        filter_release.setRange(0, 1);
        
        //set sustain level to be 1
        filter_sustain.setValue(1);
        
        //set filter values
        filter1_cuttoff_freq.setValue(20000);
        filter2_cuttoff_freq.setValue(20000);
        
        
        
        //add child components
        addAndMakeVisible(filter1_label);
        addAndMakeVisible(filter1_type);
        addAndMakeVisible(filter1_cuttoff_freq);
        addAndMakeVisible(filter1_resonance);
        
        addAndMakeVisible(filter2_label);
        addAndMakeVisible(filter2_type);
        addAndMakeVisible(filter2_cuttoff_freq);
        addAndMakeVisible(filter2_resonance);
        
        addAndMakeVisible(filter_attack);
        addAndMakeVisible(filter_decay);
        addAndMakeVisible(filter_sustain);
        addAndMakeVisible(filter_release);
        
        //add slider listeners
        filter1_cuttoff_freq.addListener(this);
        filter2_cuttoff_freq.addListener(this);
        filter1_type.addListener(this);
        filter2_type.addListener(this);
        filter1_resonance.addListener(this);
        filter2_resonance.addListener(this);
        filter_attack.addListener(this);
        filter_decay.addListener(this);
        filter_sustain.addListener(this);
        filter_release.addListener(this);
    }
    
    ~Filter_Menu(){
        //remove slider listeners
        filter1_cuttoff_freq.removeListener(this);
        filter2_cuttoff_freq.removeListener(this);
        filter1_resonance.removeListener(this);
        filter2_resonance.removeListener(this);
        filter_attack.removeListener(this);
        filter_decay.removeListener(this);
        filter_sustain.removeListener(this);
        filter_release.removeListener(this);
        
    }

    void update() override
    {
        // This function is called at the frequency specified by the setFramesPerSecond() call
        // in the constructor. You can use it to update counters, animate values, etc.
    }

    void paint (juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::peachpuff);
    }

    void resized() override
    {
        //get total area of nav bar
        auto area = getLocalBounds();
        
        //flexboxes to organize items
        juce::FlexBox fb;
        juce::FlexBox row1;
        juce::FlexBox row2;
        juce::FlexBox asdr;
        
        
        //set rows on vertical axis direction
        fb.flexDirection = juce::FlexBox::Direction::row;
        row1.flexDirection = juce::FlexBox::Direction::column;
        row2.flexDirection = juce::FlexBox::Direction::column;
        asdr.flexDirection = juce::FlexBox::Direction::column;
        
        //set wrapping of flex boxes
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        row1.flexWrap = juce::FlexBox::Wrap::noWrap;
        row2.flexWrap = juce::FlexBox::Wrap::noWrap;
        asdr.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        //set content justification (main axis)
        fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row1.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row2.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        asdr.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        
        //set content alignment (cross axis)
        fb.alignContent = juce::FlexBox::AlignContent::flexStart;
        row1.alignContent = juce::FlexBox::AlignContent::flexStart;
        row2.alignContent = juce::FlexBox::AlignContent::flexStart;
        asdr.alignContent = juce::FlexBox::AlignContent::flexStart;
        
        auto filter_label_height = area.getHeight()/10;
        auto filter_type_height = area.getHeight()/10;
        auto filter_knob_height = 3*area.getHeight()/10;
        auto filter_slider_height = area.getHeight()/5;
        
        //add items to row1
        row1.items.add(juce::FlexItem(filter1_label).withMinWidth(area.getWidth()/3).withMinHeight(filter_label_height));
        row1.items.add(juce::FlexItem(filter1_type).withMinWidth(area.getWidth()/3).withMinHeight(filter_type_height));
        row1.items.add(juce::FlexItem(filter1_cuttoff_freq).withMinWidth(area.getWidth()/3 ).withMinHeight(filter_knob_height));
        row1.items.add(juce::FlexItem(filter1_resonance).withMinWidth(area.getWidth()/3 ).withMinHeight(filter_knob_height));
        
        //add items to row2
        row2.items.add(juce::FlexItem(filter2_label).withMinWidth(area.getWidth()/3).withMinHeight(filter_label_height));
        row2.items.add(juce::FlexItem(filter2_type).withMinWidth(area.getWidth()/3).withMinHeight(filter_type_height));
        row2.items.add(juce::FlexItem(filter2_cuttoff_freq).withMinWidth(area.getWidth()/3 ).withMinHeight(filter_knob_height));
        row2.items.add(juce::FlexItem(filter2_resonance).withMinWidth(area.getWidth()/3 ).withMinHeight(filter_knob_height));
        
        //add items to asr
        asdr.items.add(juce::FlexItem(filter_attack).withMinWidth(area.getWidth()/3).withMinHeight(filter_slider_height));
        asdr.items.add(juce::FlexItem(filter_decay).withMinWidth(area.getWidth()/3).withMinHeight(filter_slider_height));
        asdr.items.add(juce::FlexItem(filter_sustain).withMinWidth(area.getWidth()/3).withMinHeight(filter_slider_height));
        asdr.items.add(juce::FlexItem(filter_release).withMinWidth(area.getWidth()/3).withMinHeight(filter_slider_height));
        
        
        //add items to main flex box
        fb.items.add(juce::FlexItem(row1).withFlex(1.0));
        fb.items.add(juce::FlexItem(row2).withFlex(1.0));
        fb.items.add(juce::FlexItem(asdr).withFlex(1.0));
        
        fb.performLayout (getLocalBounds().reduced(10).toFloat());
    }
    
    void sliderValueChanged (juce::Slider* slider) override {
        //send a message  that slider value is changed
        sendChangeMessage();
    }
    
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override{
        //send a message that combobox has changed
        sendChangeMessage();
    }
    

private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Filter_Menu)
};

//gui for amplifier menu
class amplifier_Menu : public juce::AnimatedAppComponent, public juce::ChangeBroadcaster, public juce::Slider::Listener
{
public:
    //create components for amplifier
    gain_slider  amp_total_gain;
    
    //create components for  asdr
    juce::Slider amp_attack;
    juce::Slider amp_decay;
    juce::Slider amp_sustain;
    juce::Slider amp_release;
    
    
    //==============================================================================
    amplifier_Menu()
    {
    
        //main gain silder settings
        amp_total_gain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        amp_total_gain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        amp_total_gain.setTextValueSuffix (" Total Gain (dB)");
        amp_total_gain.setRange(-100, 50);
        
        //set asdr slider settings
        amp_attack.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        amp_attack.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        amp_attack.setTextValueSuffix (" Attack Time (s)");
        amp_attack.setRange(0, 0.5);
        
        amp_decay.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        amp_decay.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100,50);
        amp_decay.setTextValueSuffix (" Decay Time (s)");
        amp_decay.setRange(0, 0.5);
        
        amp_sustain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        amp_sustain.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        amp_sustain.setTextValueSuffix (" Sustain Level (fraction)");
        amp_sustain.setRange(0, 1);
        
        amp_release.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        amp_release.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 50);
        amp_release.setTextValueSuffix (" Release Time (s)");
        amp_release.setRange(0, 1);
        
        //set sustain level to be 1
        amp_sustain.setValue(1);
        
        
        
        //listen to slider change values
        amp_total_gain.addListener(this);
        amp_attack.addListener(this);
        amp_decay.addListener(this);
        amp_sustain.addListener(this);
        amp_release.addListener(this);
        
                                   
        //add child components
        addAndMakeVisible(amp_total_gain);

        addAndMakeVisible(amp_attack);
        addAndMakeVisible(amp_decay);
        addAndMakeVisible(amp_sustain);
        addAndMakeVisible(amp_release);
    }

    void update() override
    {
        // This function is called at the frequency specified by the setFramesPerSecond() call
        // in the constructor. You can use it to update counters, animate values, etc.
    }

    void paint (juce::Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::peachpuff);
    }

    void resized() override
    {
        //get total area of nav bar
        auto area = getLocalBounds();
        
        //flexboxes to organize items
        juce::FlexBox fb;
        juce::FlexBox row1;
        juce::FlexBox asdr;
        
        
        //set rows on vertical axis direction
        fb.flexDirection = juce::FlexBox::Direction::row;
        row1.flexDirection = juce::FlexBox::Direction::column;
        asdr.flexDirection = juce::FlexBox::Direction::column;
        
        //set wrapping of flex boxes
        fb.flexWrap = juce::FlexBox::Wrap::wrap;
        row1.flexWrap = juce::FlexBox::Wrap::noWrap;
        asdr.flexWrap = juce::FlexBox::Wrap::noWrap;
        
        //set content justification (main axis)
        fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        row1.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        asdr.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        
        //set content alignment (cross axis)
        fb.alignContent = juce::FlexBox::AlignContent::flexStart;
        row1.alignContent = juce::FlexBox::AlignContent::flexStart;
        asdr.alignContent = juce::FlexBox::AlignContent::flexStart;

        auto amp_big_slider_height = 4*area.getHeight()/5;
        auto amp_slider_height = area.getHeight()/5;
        
        //add items to row1
        row1.items.add(juce::FlexItem(amp_total_gain).withMinWidth(area.getWidth()/3).withMinHeight(amp_big_slider_height));
        
        //add items to asr
        asdr.items.add(juce::FlexItem(amp_attack).withMinWidth(area.getWidth()/3).withMinHeight(amp_slider_height));
        asdr.items.add(juce::FlexItem(amp_decay).withMinWidth(area.getWidth()/3).withMinHeight(amp_slider_height));
        asdr.items.add(juce::FlexItem(amp_sustain).withMinWidth(area.getWidth()/3).withMinHeight(amp_slider_height));
        asdr.items.add(juce::FlexItem(amp_release).withMinWidth(area.getWidth()/3).withMinHeight(amp_slider_height));
        
        
        //add items to main flex box
        fb.items.add(juce::FlexItem(row1).withFlex(1.0));
        fb.items.add(juce::FlexItem(asdr).withFlex(1.0));
        
        fb.performLayout (getLocalBounds().reduced(10).toFloat());

    }
    
    void sliderValueChanged (juce::Slider* slider) override {
        //send a message  that slider value is changed
        sendChangeMessage();
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (amplifier_Menu)
};
