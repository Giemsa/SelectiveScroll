//
//  SelectiveScroll.cpp
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/07.
//
//

#include "SelectiveScroll.h"
#include <cmath>

USING_NS_CC;
using namespace std;


// MEMO:
// Define Layer order.
typedef enum {
    LayerOrder_BGLayer,
    LayerOrder_ScrollLayer,
} LayerOrder;


SelectiveScroll::SelectiveScroll()
:   _scrollLayerColor(ccc4BFromccc4F(ccc4FFromccc3B(ccWHITE)))
,   _bgColor(ccc4BFromccc4F(ccc4FFromccc3B(ccGRAY)))
,   _clipToBounds(true)
,   _scrollSize(CCSizeMake(600, 600))
{}

SelectiveScroll::~SelectiveScroll() {}


#pragma mark - Init

bool SelectiveScroll::init()
{
    if (!CCLayer::init()) {
        return false;
    }
    
    // this layer
    // (enable single touch)
    this->setTouchEnabled(true);
    this->setTouchMode(kCCTouchesOneByOne);
    // (anchor point)
    this->setAnchorPoint(CCPointMake(0.5, 0.5));
    this->ignoreAnchorPointForPosition(false);
    
    // bgLayer
    CCSize size = this->getContentSize();
    _bgLayer = CCLayerColor::create(_bgColor, size.width, size.height);
    _bgLayer->setAnchorPoint(CCPointZero);
    _bgLayer->retain();
    this->addChild(_bgLayer, LayerOrder_BGLayer);
    
    // scrollLayer
    _scrollLayer = CCLayerColor::create(_scrollLayerColor, _scrollSize.width, _scrollSize.height);
    _scrollLayer->retain();
    _scrollLayer->setAnchorPoint(CCPointMake(1.0, 1.0));
    _scrollLayer->setPosition(CCPointZero);
    this->addChild(_scrollLayer, LayerOrder_ScrollLayer);
    
    return true;
}

void SelectiveScroll::draw()
{
    // resize (if size was changed.)
    CCSize size = this->getContentSize();
    
    if (_bgLayer && !_bgLayer->getContentSize().equals(size)) {
        _bgLayer->setContentSize(size);
    }
    if (_scrollLayer && !_scrollLayer->getContentSize().equals(_scrollSize)) {
        _scrollLayer->setContentSize(_scrollSize);
    }
}

// MEMO:
// Clip the Layer by using OpenGL functions.
void SelectiveScroll::visit()
{
    if (_clipToBounds) {
        glEnable(GL_SCISSOR_TEST);
        CCRect rect = this->boundingBox();
        CCEGLView::sharedOpenGLView()->setScissorInPoints(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        CCLayer::visit();
        glDisable(GL_SCISSOR_TEST);
    }
}


#pragma mark - UI
#pragma mark Touch

// helper
void* easeAction(CCMoveTo* moveTo, BoundingEffect effect)
{
    void* action;
    switch ((int)effect) {
        case BoundingEffectNormal:  action = CCEaseExponentialOut::create(moveTo);  break;
        case BoundingEffectBack:    action = CCEaseBackOut::create(moveTo);         break;
        case BoundingEffectBounce:  action = CCEaseBounceOut::create(moveTo);       break;
        case BoundingEffectElastic: action = CCEaseElasticOut::create(moveTo);      break;
    }
    return action;
}

// began
bool SelectiveScroll::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint p = pTouch->getLocationInView();
    
    // is outside of this layer, ignore touch event.
    if (_clipToBounds && !this->boundingBox().containsPoint(p)) {
        return false;
    }
    
    // stop scrolling animation,
    _scrollLayer->stopAllActions();
    
    // save points for calc.
    _lastTouchPoint = p;
    _beganTouchPoint = p;
    _beganScrollPosition = _scrollLayer->getPosition();
    
//    this->detectSelectedItem(p);
    
    return true;
}

// moved
void SelectiveScroll::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    // scroll
    CCPoint p = pTouch->getLocationInView();
    _scrollLayer->setPositionY(_beganScrollPosition.y + _beganTouchPoint.y - p.y);
    
//    this->detectSelectedItem(p);
    
    // save last (technically it is previous location)
    _lastTouchPoint = pTouch->getPreviousLocationInView();
}

// ended
void SelectiveScroll::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint p = pTouch->getLocationInView();
    CCPoint delta = CCPointMake(0, _lastTouchPoint.y - p.y);
    
    // speed scroll correction
    delta.y *= (14.0 <= abs(delta.y) ? 12.0 : 0.0);
    
    // scroll
    _scrollLayer->setPositionY(_beganScrollPosition.y + _beganTouchPoint.y - p.y);
    
    float topY = this->getContentSize().height - _scrollLayer->getContentSize().height;
    float offsetY = _scrollLayer->getPositionY() + delta.y;
    
    // animation
    // (bounce top)
    if (offsetY<= topY) {
        CCMoveTo* moveTo = CCMoveTo::create(1.0, CCPointMake(0, topY));
        _scrollLayer->runAction((CCAction*)easeAction(moveTo, _topBoundingEffect));
    }
    // (bounce bottom)
    else if (0 <= offsetY) {
        CCMoveTo* moveTo = CCMoveTo::create(1.0, CCPointMake(0, 0));
        _scrollLayer->runAction((CCAction*)easeAction(moveTo, _bottomBoundingEffect));
    }
    // (momentum scrolling)
    else {
        CCMoveBy* moveBy = CCMoveBy::create(0.87, CCPointMake(0, delta.y));
        _scrollLayer->runAction(CCEaseSineOut::create(moveBy));
    }
}


#pragma mark - Setter/Getter

// clipToBounds
bool SelectiveScroll::clipToBounds() { return _clipToBounds; }
void SelectiveScroll::clipToBounds(bool clip) { _clipToBounds = clip; }

// bounce effect kind
void SelectiveScroll::setBoundingEffectKind(BoundingEffect effect)
{
    _topBoundingEffect = effect;
    _bottomBoundingEffect = effect;
}
void SelectiveScroll::setBoundingEffectKind(BoundingEffect top, BoundingEffect bottom)
{
    _topBoundingEffect = top;
    _bottomBoundingEffect = bottom;
}


