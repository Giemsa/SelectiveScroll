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

namespace cocos2d {
    // Top / Bottom bounding effect kind.
    typedef enum {
        BoundingEffectNormal,
        BoundingEffectBack,
        BoundingEffectBounce,
        BoundingEffectElastic,
        BoundingEffect_Count
    } BoundingEffect;

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
        CCAction* _runningAction;
        
        // touch helper
        CCNode* _selectedItem;
        void detectSelectedItem(const CCPoint &p);
        bool canScrollVertical() const;
        bool canScrollHorizontal() const;
        void stopScrollingAnimation();
        CCAction* fitToAction(const CCPoint &toPoint);
        CCRect absoluteBoundingBox();
        
        CCAction* getEaseAction(CCMoveTo* moveTo, const BoundingEffect effect);
        void pagingScrollDidEndCallback(CCNode* node);
        
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
        void setBoundingEffectKind(const BoundingEffect effect); // both
        void setBoundingEffectKind(const BoundingEffect top, const BoundingEffect bottom); // individual
        
        // clipToBounds Setter/Getter
        bool clipToBounds() const;
        void clipToBounds(const bool clip);
        
        // scrollTo
        void scrollToTop();
        void scrollToPoint(const CCPoint &p);
        void scrollToPointWithAnimation(const CCPoint &p);
        
        // enable/disable scroll
        bool enableToScroll() const;
        void enableToScroll(const bool enable);
    };
}

#endif /* defined(__SelectiveScroll__SelectiveScroll__) */


