//
//  SelectiveScroll.h
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/07.
//
//

#ifndef __SelectiveScroll__SelectiveScroll__
#define __SelectiveScroll__SelectiveScroll__

#include "cocos2d.h"
#include "SelectiveScrollDelegateProtocol.h"

// Top / Bottom bounding effect kind.
typedef enum {
    BoundingEffectNormal,
    BoundingEffectBack,
    BoundingEffectBounce,
    BoundingEffectElastic,
    BoundingEffect_Count
} BoundingEffect;

USING_NS_CC;

class SelectiveScroll : public CCLayer {
private:
    SelectiveScroll();
    virtual ~SelectiveScroll();
    virtual bool init();
    virtual void draw();
    virtual void visit();
    virtual void onEnterTransitionDidFinish();
    
    bool _clipToBounds;
    bool _clipScrollInteraction;
    bool _enableToScroll;
    
    // color
    ccColor4B _scrollLayerColor;
    ccColor4B _bgColor;
    
    // effect
    BoundingEffect _topBoundingEffect;
    BoundingEffect _bottomBoundingEffect;
    
    // scroll calc
    CCPoint _beganTouchPoint;
    CCPoint _lastTouchPoint;
    CCPoint _beganScrollPosition;
    
    // running animation
    void* _runningAction;
    
    // touch helper
    void* _selectedItem;
    void detectSelectedItem(CCPoint p);
    
    
    // SelectiveScroll (Delegate)
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
public:
    CREATE_FUNC(SelectiveScroll);
    
    // delegate
    CC_SYNTHESIZE(SelectiveScrollDelegate*, _delegate, Delegate);
    
    // layer
    CC_SYNTHESIZE(CCSize, _scrollSize, ScrollSize);
    CC_SYNTHESIZE(CCLayerColor*, _bgLayer, BGLayer);
    CC_SYNTHESIZE(CCLayerColor*, _scrollLayer, ScrollLayer);
    
    // Effect kind can set individually.
    void setBoundingEffectKind(BoundingEffect effect); // both
    void setBoundingEffectKind(BoundingEffect top, BoundingEffect bottom); // individual
    
    // clipToBounds Setter/Getter
    bool clipToBounds();
    void clipToBounds(bool clip);
    
    // scrollTo
    void scrollToPoint(CCPoint p);
    void scrollToPointWithAnimation(CCPoint p);
    
    // enable/disable scroll
    bool enableToScroll();
    void enableToScroll(bool enable);
};

#endif /* defined(__SelectiveScroll__SelectiveScroll__) */


