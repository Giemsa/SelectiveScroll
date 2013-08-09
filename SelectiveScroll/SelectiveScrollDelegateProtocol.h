//
//  SelectiveScrollDelegateProtocol.h
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/08.
//
//

#ifndef __SelectiveScroll__SelectiveScrollDelegateProtocol__
#define __SelectiveScroll__SelectiveScrollDelegateProtocol__

#include "cocos2d.h"

USING_NS_CC;

class SelectiveScrollDelegate {
    
public:
    virtual bool isLayerSelected(CCLayer* layer) {CC_UNUSED_PARAM(layer); return false;}
    virtual void selectiveScrollHighlightLayer(bool hi, CCLayer* layer) {CC_UNUSED_PARAM(hi), CC_UNUSED_PARAM(layer);}
    virtual void selectiveScrollDidSelectLayer(CCLayer* layer) {CC_UNUSED_PARAM(layer);}
    
//    // optional
//    virtual void selectiveScrollBeginTouchOnLayer(CCLayer* layer) {CC_UNUSED_PARAM(layer);}
//    virtual void selectiveScrollMoves(CCLayer* layer) {CC_UNUSED_PARAM(layer);}
//    virtual void selectiveScrollCanceled(CCLayer* layer) {CC_UNUSED_PARAM(layer);}
};


#endif /* defined(__SelectiveScroll__SelectiveScrollDelegateProtocol__) */
