/*
  ==============================================================================

    DTLookAndFeel.h
    Created: 19 Apr 2019 12:59:27pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DTInterfaceDefines.h"

class DTLookAndFeel
: public LookAndFeel_V4
{
public:
    DTLookAndFeel():
    mBackgroundImageA(ImageCache::getFromMemory(BinaryData::TabBarA_png, BinaryData::TabBarA_pngSize)),
    mBackgroundImageB(ImageCache::getFromMemory(BinaryData::TabBarB_png, BinaryData::TabBarB_pngSize)),
    mNeedleImage(ImageCache::getFromMemory(BinaryData::fader_png, BinaryData::fader_pngSize)),
    mTabbedNeedleImage(ImageCache::getFromMemory(BinaryData::TabBarBNeedle_png, BinaryData::TabBarBNeedle_pngSize))
    {
       // mSliderImage = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::background_pngSize);

        setColour(PopupMenu::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
        setColour(PopupMenu::ColourIds::textColourId, DTColourIds::PopUpText);
    };
    
    virtual ~DTLookAndFeel(){};
    
    /** SLIDERS **/
    //==============================================================================
    
    /** Draws a rotary slider thumbpoint to g */
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        
//        const int numFrames = mGearImage.getHeight() / mGearImage.getWidth();
//        const int frameIndex = static_cast<int>(std::ceil(sliderPos * (numFrames - 1)));
//
//        const float radius = jmin(width * 0.5f, height * 0.5f);
//        const float centreX = x + width * 0.5f;
//        const float centreY = y + height * 0.5f;
//        const float rx = centreX - radius;
//        const float ry = centreY - radius;
//
//        //Draws the image file at the current frameIndex
//        g.drawImage(mGearImage,
//                    rx,
//                    ry,
//                    2 * radius,
//                    2 * radius,
//                    0,
//                    frameIndex * mGearImage.getWidth(),
//                    mGearImage.getWidth(),
//                    mGearImage.getWidth());
        
        
        //==================
        
        auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (5);

        auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin (8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;


        //auto thumbWidth = lineW * 2.0f;
        Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                                 bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

        //g.setColour (slider.findColour (Slider::thumbColourId));
        //g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
    }
    /**draws a liniar slider  thumb point to g*/
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const Slider::SliderStyle style, Slider& slider) override
    {


        Point<float> maxPoint;


        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float) width * 0.5f)-2.f;
        auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

        maxPoint = { kx, ky };

        auto thumbWidth = getSliderThumbRadius (slider);

        if(slider.isHorizontal()){
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (maxPoint));
        }
        else{
            g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
            g.drawImage(mNeedleImage, Rectangle<int>(mNeedleImage.getWidth(), mNeedleImage.getHeight()).toFloat().withCentre(maxPoint));
            
        }
        
    }


    void drawTabbedButtonBarBackground (TabbedButtonBar& _b, Graphics& _g) override{
        if(_b.getNumTabs() > 3)
            _g.drawImage(mBackgroundImageA, _b.getLocalBounds().toFloat());
        else
            _g.drawImage(mBackgroundImageB, _b.getLocalBounds().toFloat());
    }
    
//    int getTabButtonOverlap (int tabDepth) override
//    {
//        return 1 + tabDepth / 3;
//    }

//    int getTabButtonSpaceAroundImage( )override
//    {
//        return 0;
//    }

    int getTabButtonBestWidth (TabBarButton& button, int tabDepth) override
    {
        int width = Font ((float) tabDepth * 0.6f).getStringWidth (button.getButtonText())
                       + getTabButtonOverlap (tabDepth) * 2;

//        if (auto* extraComponent = button.getExtraComponent())
//            width += button.getTabbedButtonBar().isVertical() ? extraComponent->getHeight()
//                                                              : extraComponent->getWidth();

        return width+10;
    }
    
//    Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp) override
//    {
//        Rectangle<int> extraComp(0,0,0,0);
//
//        return extraComp;
//    }
    
    void drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        const Rectangle<int> activeArea (button.getActiveArea());

        const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();

        const Colour bkg (button.getTabBackgroundColour());

        if (button.getToggleState())
        {
            g.setColour (bkg);
        }
        else
        {
            Point<int> p1, p2;

            switch (o)
            {
                case TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
                case TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
                case TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
                case TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
                default:                              jassertfalse; break;
            }

            g.setGradientFill (ColourGradient (bkg.brighter (0.2f), p1.toFloat(),
                                               bkg.darker (0.1f),   p2.toFloat(), false));
        }

        //g.fillRect (activeArea);

        g.setColour (button.findColour (TabbedButtonBar::tabOutlineColourId));


        Rectangle<int> r (activeArea);

//        if (o != TabbedButtonBar::TabsAtBottom)   g.fillRect (r.removeFromTop (1));
//        if (o != TabbedButtonBar::TabsAtTop)      g.fillRect (r.removeFromBottom (1));
//        if (o != TabbedButtonBar::TabsAtRight)    g.fillRect (r.removeFromLeft (1));
//        if (o != TabbedButtonBar::TabsAtLeft)     g.fillRect (r.removeFromRight (1));
        

        const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

        Colour col (bkg.contrasting().withMultipliedAlpha (alpha));

        if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
        {
            TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId
                                                                   : TabbedButtonBar::tabTextColourId;

            if (bar->isColourSpecified (colID))
                col = bar->findColour (colID);
            else if (isColourSpecified (colID))
                col = findColour (colID);
        }

        const Rectangle<float> area (button.getTextArea().toFloat());

        float length = area.getWidth();
        float depth  = area.getHeight();

        if (button.getTabbedButtonBar().isVertical())
            std::swap (length, depth);

        TextLayout textLayout;
        createTabTextLayout (button, length, depth, col, textLayout);

        AffineTransform t;

        switch (o)
        {
            case TabbedButtonBar::TabsAtLeft:   t = t.rotated (MathConstants<float>::pi * -0.5f).translated (area.getX(), area.getBottom()); break;
            case TabbedButtonBar::TabsAtRight:  t = t.rotated (MathConstants<float>::pi *  0.5f).translated (area.getRight(), area.getY()); break;
            case TabbedButtonBar::TabsAtTop:
            case TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
            default:                            jassertfalse; break;
        }

        g.addTransform (t);
        
        Colour centreColour = Colours::white.withAlpha(0.65f);
        Colour edgeColour =Colours::white.withAlpha(0.f);
        Point<float> centrePoint = area.toFloat().getCentre();
        Point<float> edgePoint = centrePoint.withX(area.toFloat().getX() +10.f);
        
        g.setGradientFill(ColourGradient(centreColour, centrePoint, edgeColour, edgePoint, true));
        
        if (isMouseOver)
            g.fillRoundedRectangle(area.toFloat().reduced(1.f), 2.f);
      
        textLayout.draw (g, Rectangle<float> (length, depth));
        
        
        if(button.isFrontTab()){
            g.setColour(Colours::black);
            //g.drawEllipse(r.reduced(5).toFloat(), 1.f);
            
            Line<float> needle(r.toFloat().getRight() *0.9f, r.toFloat().getBottom() - 5.f, r.toFloat().getRight() *0.9f, r.toFloat().getCentreY());
            //g.drawArrow(needle, 3.f, 6.f, 6.f);

            g.drawImage(mTabbedNeedleImage, Rectangle<float>(r.toFloat().getRight()*0.9f,
                                                             r.toFloat().getHeight()-mTabbedNeedleImage.getBounds().toFloat().getHeight(),
                                                             mTabbedNeedleImage.getBounds().toFloat().getWidth(),
                                                             mTabbedNeedleImage.getBounds().toFloat().getHeight()));
            
        }
    }
    
private:
    const Image mBackgroundImageA;
    const Image mBackgroundImageB;
    const Image mNeedleImage;
    const Image mTabbedNeedleImage;
    //const Image mGearImage;
    
};
