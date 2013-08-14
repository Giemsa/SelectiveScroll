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

#define CANCEL_SELECTION_THRESHOLD 4.0

// MEMO:
// Define Layer order.
typedef enum {
    LayerOrder_BGLayer,
    LayerOrder_ScrollLayer,
} LayerOrder;


SelectiveScroll::SelectiveScroll()
:   _scrollLayerColor(ccc4(255.0, 0.0, 0.0, 255.0 * 0.5))
,   _bgColor(ccc4(0.0, 0.0, 255.0, 255.0 * 0.5))
,   _clipToBounds(true)
,   _clipScrollInteraction(true)
,   _enableToScroll(true)
,   _scrollSize(CCSizeMake(600, 600))
,   _runningAction(NULL)
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
    this->addChild(_bgLayer, LayerOrder_BGLayer);
    _bgLayer->setAnchorPoint(CCPointZero);  
    _bgLayer->retain();
    
    // scrollLayer
    _scrollLayer = CCLayerColor::create(_scrollLayerColor, _scrollSize.width, _scrollSize.height);
    this->addChild(_scrollLayer, LayerOrder_ScrollLayer);
    _scrollLayer->setAnchorPoint(CCPointMake(0.0, 0.0));
    _scrollLayer->retain();
    
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
    else {
        CCLayer::visit();
    }
}

void SelectiveScroll::onEnterTransitionDidFinish()
{
    this->scrollToTop();
}


#pragma mark - UI
#pragma mark Scroll

void SelectiveScroll::scrollToTop()
{
    this->scrollToPoint(CCPointMake(0.0, this->getContentSize().height - _scrollSize.height));
}

void SelectiveScroll::scrollToPoint(CCPoint p)
{
    _scrollLayer->setPosition(p);
}

void SelectiveScroll::scrollToPointWithAnimation(CCPoint p)
{
    this->stopAction((CCAction*)_runningAction);
    CCMoveTo* moveTo = CCMoveTo::create(1.0, p);
    _scrollLayer->runAction(CCEaseInOut::create(moveTo, 2.0));
}


#pragma mark Touch

// helper func
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

bool SelectiveScroll::isScrollVertical()
{
    return this->getContentSize().width < _scrollSize.width;
}

bool SelectiveScroll::isScrollHorizontal() 
{
    return this->getContentSize().height < _scrollSize.height;
}

CCAction* SelectiveScroll::fitToAction(CCPoint delta)
{
    CCPoint toPoint = CCPointMake(_scrollLayer->getPositionX() + delta.x, _scrollLayer->getPositionY() + delta.y);
    CCPoint fitPoint = CCPointZero;
    float top = this->getContentSize().height - _scrollSize.height;
    float right = this->getContentSize().width - _scrollSize.width;
    float bottom = 0.0;
    float left = 0.0;
    
    if (isScrollHorizontal() && toPoint.y < top) {
        // top left
        if (left < toPoint.x) {
            fitPoint = CCPointMake(left, top);
        }
        // top right
        else if (toPoint.x < right) {
            fitPoint = CCPointMake(right, top);
        }
        // top
        else {
            fitPoint = CCPointMake(toPoint.x, top);
        }
    }
    else if (isScrollHorizontal() && bottom < toPoint.y) {
        // bottom right
        if (toPoint.x < right) {
            fitPoint = CCPointMake(right, bottom);
        }
        // bottom left
        else if (left < toPoint.x) {
            fitPoint = CCPointMake(left, bottom);
        }
        // bottom
        else {
            fitPoint = CCPointMake(toPoint.x, bottom);
        }
    }
    // right
    else if (isScrollVertical() && toPoint.x < right) {
        fitPoint = CCPointMake(right, top);
    }
    // left
    else if (isScrollVertical() && left < toPoint.x) {
        fitPoint = CCPointMake(left, top);
    }
    else {
        CCPoint byPoint = CCPointMake((isScrollVertical() ? delta.x : 0.0), (isScrollHorizontal() ? delta.y : 0.0));
        CCMoveBy* moveBy = CCMoveBy::create(0.87, byPoint);
        return (CCAction*)CCEaseSineOut::create(moveBy);
    }
    CCMoveTo* moveTo = CCMoveTo::create(1.0, fitPoint);
    return (CCAction*)easeAction(moveTo, _topBoundingEffect);
}

void SelectiveScroll::detectSelectedItem(CCPoint p)
{
    bool cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.y - _lastTouchPoint.y));
    
    // convert "view touch point" to "layer touch point"
    CCRect rect = this->boundingBox();
    CCPoint pointOnThisLayer = ccp((p.x - rect.origin.x), (p.y - rect.origin.y));
    CCPoint pointOnScrollLayer = CCPointZero;
    pointOnScrollLayer.x = -_scrollLayer->getPositionX() + pointOnThisLayer.x;
    pointOnScrollLayer.y = rect.size.height - _scrollLayer->getPositionY() - pointOnThisLayer.y;
    
    // reset first.
    _selectedItem = NULL;
    
    // search selected item
    CCArray* sprites = _scrollLayer->getChildren();
    if (sprites == NULL) return;
    
    for (int i = 0; i < sprites->count(); i++) {
        CCLayer* layer = (CCLayer*)sprites->objectAtIndex(i);
        bool isSelected = !cancelSelection && (CCSprite*)layer->boundingBox().containsPoint(pointOnScrollLayer);
        if (isSelected) {
            _selectedItem = (CCSprite*)layer;
        }
        // delegate callback 
        SelectiveScrollDelegate* delegate = this->getDelegate();
        if (delegate) {
            // if selection state hasn't changed, change it.
            if (delegate->isLayerSelected(layer) != isSelected) {
                delegate->selectiveScrollHighlightLayer(isSelected, layer);
            }
        }
    }
}


// began
bool SelectiveScroll::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint p = pTouch->getLocationInView();
    
    // is outside of this layer, ignore touch event.
    if (_clipScrollInteraction && !this->boundingBox().containsPoint(p)) {
        return false;
    }
    // stop scrolling animation.
    if (_runningAction) {
        this->stopAction((CCAction*)_runningAction);
    }
    
    // save points for calc.
    _lastTouchPoint = p;
    _beganTouchPoint = p;
    _beganScrollPosition = _scrollLayer->getPosition();
    
    this->detectSelectedItem(p);
    
    return true;
}

// moved
void SelectiveScroll::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    // scroll
    CCPoint p = pTouch->getLocationInView();
    
    if (_enableToScroll) {
        if (isScrollVertical()) {
            _scrollLayer->setPositionX(_beganScrollPosition.x - _beganTouchPoint.x + p.x);
        }
        else if (isScrollHorizontal()) {
            _scrollLayer->setPositionY(_beganScrollPosition.y + _beganTouchPoint.y - p.y);
        }
    }
    
    // save last (technically it is previous location)
    _lastTouchPoint = pTouch->getPreviousLocationInView();
    
    this->detectSelectedItem(p);
}

// ended
void SelectiveScroll::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint p = pTouch->getLocationInView();
    
    if (_enableToScroll) {
        // speed scroll correction
        CCPoint delta = CCPointMake(_lastTouchPoint.x - p.x, _lastTouchPoint.y - p.y);
        delta.x *= -(14.0 <= abs(delta.x) ? 14.0 : 0.0);
        delta.y *= (14.0 <= abs(delta.y) ? 12.0 : 0.0);
        
        // fit
        _runningAction = this->fitToAction(delta);
        _scrollLayer->runAction((CCAction*)_runningAction);
        
        // has selected item.
        if (_selectedItem != NULL) {
            SelectiveScrollDelegate* delegate = this->getDelegate();
            if (delegate) {
                delegate->selectiveScrollHighlightLayer(false, (CCLayer*)_selectedItem);
                delegate->selectiveScrollDidSelectLayer((CCLayer*)_selectedItem);
            }
        }
    }
}


#pragma mark - Setter/Getter

// clipToBounds
bool SelectiveScroll::clipToBounds() { return _clipToBounds; }
void SelectiveScroll::clipToBounds(bool clip) { _clipToBounds = clip; _clipScrollInteraction = clip; }

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

// enable/disable scroll
bool SelectiveScroll::enableToScroll()
{
    return _enableToScroll;
}
void SelectiveScroll::enableToScroll(bool enable)
{
    _enableToScroll = enable;
}

