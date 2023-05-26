/*
  ==============================================================================

    DTHelperFunctions.h
    Created: 19 Apr 2019 12:57:08pm
    Author:  Sam Ferguson

 helper functions for the GUI
 
  ==============================================================================
*/

#pragma once
#include "DTInterfaceDefines.h"

/**returns an array of rectangles that represend a matrix of subdivitions of the provided rectangle*/
inline Array<Rectangle<int>> DTsetSubBox(Rectangle<int> _inBounds, int _xDiv, int _yDiv)
{
    Array<Rectangle<int>> subBoxs;
    subBoxs.ensureStorageAllocated(_xDiv*_yDiv);
    int xDiff = _inBounds.getWidth()/_xDiv;
    int yDiff = _inBounds.getHeight()/ _yDiv;
    int x = _inBounds.getX();
    int y = _inBounds.getY();

    for(int i = 0 ; i < _yDiv; i++){
        for(int j = 0 ; j < _xDiv; j++){
            Rectangle<int> box(x + xDiff * j,
                               y + yDiff * i,
                               xDiff,
                               yDiff);

            subBoxs.add(box);
        }
    }

    return subBoxs;
};

/**returns the provided _inBoxes rectangle array with the _appendBoxes added below*/
inline Array<Rectangle<int>> DTAppendBoxesBelow(Array<Rectangle<int>> _inBoxes, Array<Rectangle<int>> _appendBoxes)
{
    Array<Rectangle<int>> subBoxs(_inBoxes);
    Array<Rectangle<int>> newBoxs(_appendBoxes);
    
    int deltaY = subBoxs[subBoxs.size() - 1].getBottom();
    
    for(int i = 0 ; i < newBoxs.size(); i++){
        newBoxs[i].translate(0, deltaY);
        subBoxs.add(newBoxs[i]);
    }
    return subBoxs;
};

/**returns the _leftBox rectangle extended to the right edge of the _rightBox rectangle
    *the returned rectangle will have the hight and top position of the _leftBox rectangle*/
inline Rectangle<int> DTextendRecto(Rectangle<int> _leftBox, Rectangle<int> _rightBox)
{
    Rectangle<int> expandedBox = _leftBox;
    expandedBox.setRight(_rightBox.getRight());
    return expandedBox;
}

/**returns the TopBox rectangle extended to the bottom edge of the BottomBox rectangle
    *the returned rectangle will have the Width and top position of the TopBox rectangle*/
inline Rectangle<int> DTAddBoxBelow(Rectangle<int> _topBox, Rectangle<int> _bottomBox)
{
    Rectangle<int> expandedBox = _topBox;
    expandedBox.setBottom(_bottomBox.getBottom());
    return expandedBox;
}

inline Rectangle<int> DTRemoveLabelBumper(Rectangle<int> _inputBox)
{
    return _inputBox.removeFromBottom(_inputBox.getHeight() - LABEL_BUMPER);
    
}

/**Spin Lock that waits for the processor to be in a state where we can safley pause it's updates to input a new value*/
inline void DTWaitForUpdate(std::atomic_bool& _pause, std::atomic_bool& _update)
{
    bool test(!_update.load(std::memory_order_acquire));
        while (!_pause.compare_exchange_weak(test, true, std::memory_order_release, std::memory_order_acquire)) {
            test = !_update.load(std::memory_order_acquire);
            DBG("Waiting for Update");
        }
}


inline float DTValueToPixels(float _value, NormalisableRange<float> _range, float _hightinPx)
{
    float ofset = log(_range.start);
    float mult = (log(_value)-ofset)/(log(_range.end)-ofset);
    mult = 1.f - mult;
    return trunc(mult * _hightinPx);
}
