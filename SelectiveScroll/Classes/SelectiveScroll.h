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
    namespace BoundingEffect
    {
        enum Type
        {
            Normal,
            Back,
            Bounce,
            Elastic,
            Count
        };
    }

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
        
        // effect
        BoundingEffect::Type _topBoundingEffect;
        BoundingEffect::Type _bottomBoundingEffect;
        
        // scroll calc
        CCPoint _beganTouchPoint;
        CCPoint _lastTouchPoint;
        CCPoint _beganScrollPosition;
        
        // running animation
        CCAction* _runningAction;
        CCNode* _container;
        CCNode* _background;
        
        // touch helper
        CCNode* _selectedItem;
        void detectSelectedItem(const CCPoint &p);
        bool canScrollVertical() const;
        bool canScrollHorizontal() const;
        void stopScrollingAnimation();
        CCAction* fitToAction(const CCPoint &toPoint);
        CCRect absoluteBoundingBox();
        
        CCAction* getEaseAction(CCMoveTo* moveTo, const BoundingEffect::Type effect);
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
        
        // Effect kind can set individually.
        void setBoundingEffectKind(const BoundingEffect::Type effect); // both
        void setBoundingEffectKind(const BoundingEffect::Type top, const BoundingEffect::Type bottom); // individual
        
        // clipToBounds Setter/Getter
        bool getClipToBounds() const;
        void setClipToBounds(const bool clip);
        
        // scrollTo
        void scrollToTop();
        void scrollToPoint(const CCPoint &p);
        void scrollToPointWithAnimation(const CCPoint &p);
        
        // enable/disable scroll
        bool getEnableToScroll() const;
        void setEnableToScroll(const bool enable);

        void setBackground(CCNode* bg);
        CCNode* getBackground() const;

        void setContainer(CCNode* node);
        CCNode* getContainer() const;
    };
}

#endif /* defined(__SelectiveScroll__SelectiveScroll__) */


