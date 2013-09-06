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

#define CANCEL_SELECTION_THRESHOLD  4.0
#define RUNNING_ANIMATION_TAG       1

// MEMO:
// Define Layer order.
typedef enum {
    ZIndex_BGLayer,
    ZIndex_ScrollLayer,
} ZIndex;


SelectiveScroll::SelectiveScroll()
:   _scrollLayerColor(ccc4(255.0, 0.0, 0.0, 255.0 * 0.5))
,   _bgColor(ccc4(0.0, 0.0, 255.0, 255.0 * 0.5))
,   _clipToBounds(true)
,   _clipScrollInteraction(true)
,   _enableToScroll(true)
,   _scrollSize(CCSizeMake(600, 600))
,   _runningAction(NULL)
{}

SelectiveScroll::~SelectiveScroll()
{
    CC_SAFE_RELEASE(_bgLayer);
    CC_SAFE_RELEASE(_scrollLayer);
}


#pragma mark - CCLayer Lifecycle

bool SelectiveScroll::init()
{
    if (!CCLayer::init()) {
        return false;
    }
    _delegate = NULL;
    
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
    this->addChild(_bgLayer, ZIndex_BGLayer);
    _bgLayer->setAnchorPoint(CCPointZero);  
    _bgLayer->retain();
    
    // scrollLayer
    _scrollLayer = CCLayerColor::create(_scrollLayerColor, _scrollSize.width, _scrollSize.height);
    _scrollLayer->retain();
    this->addChild(_scrollLayer, ZIndex_ScrollLayer);
    _scrollLayer->setAnchorPoint(CCPointMake(0.0, 0.0));
    
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
        // not visible, then no need to clip
        if (!this->isVisible()) return;
        
        glEnable(GL_SCISSOR_TEST);
        CCRect bound = this->absoluteBoundingBox();
        CCEGLView::sharedOpenGLView()->setScissorInPoints(bound.origin.x, bound.origin.y, bound.size.width, bound.size.height);
        CCLayer::visit();
        glDisable(GL_SCISSOR_TEST);
    }
    else {
        CCLayer::visit();
    }
}

void SelectiveScroll::onEnterTransitionDidFinish()
{
    //    this->scrollToTop();
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

// returns Eased Action
void* easeAction(CCMoveTo* moveTo, BoundingEffect effect)
{
    void* action = CCEaseExponentialOut::create(moveTo);
    switch ((int)effect) {
        case BoundingEffectBack:    action = CCEaseBackOut::create(moveTo);         break;
        case BoundingEffectBounce:  action = CCEaseBounceOut::create(moveTo);       break;
        case BoundingEffectElastic: action = CCEaseElasticOut::create(moveTo);      break;
    }
    return action;
}

// returns bounds on Scene.
CCRect SelectiveScroll::absoluteBoundingBox()
{
    CCNode* parentLayer = this->getParent();
    if (!parentLayer) return this->boundingBox();
    
    CCRect bound = this->boundingBox();
    CCNode* runningScene = CCDirector::sharedDirector()->getRunningScene();
    int layerDepth = 0;
    const int LayerHierarchyMax = 100.0;
    
    // search for running scene. (to avoid infinity-loop, max 100 hierarchies are supported)
    while (!parentLayer->isEqual(runningScene)) {
        if (LayerHierarchyMax <= layerDepth) break;
        
        bound.origin.x += parentLayer->boundingBox().origin.x;
        bound.origin.y += parentLayer->boundingBox().origin.y;
        
        parentLayer = parentLayer->getParent();
        if (!parentLayer) break;
        layerDepth++;
    }
    return bound;
}

bool SelectiveScroll::canScrollVertical()
{
    return this->getContentSize().width < _scrollSize.width;
}

bool SelectiveScroll::canScrollHorizontal() 
{
    return this->getContentSize().height < _scrollSize.height;
}

void SelectiveScroll::pagingScrollDidEndCallback(CCNode* node)
{
    if (_delegate) {
        _delegate->pagingScrollDidEnd(node, this);
    }
}

CCAction* SelectiveScroll::fitToAction(CCPoint delta)
{
    CCPoint toPoint = CCPointMake(_scrollLayer->getPositionX() + delta.x, _scrollLayer->getPositionY() + delta.y);
    CCPoint fitPoint = CCPointZero;
    float top = this->getContentSize().height - _scrollSize.height;
    float right = this->getContentSize().width - _scrollSize.width;
    float bottom = 0.0;
    float left = 0.0;
    
    if (this->canScrollHorizontal() && toPoint.y < top) {
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
    else if (this->canScrollHorizontal() && bottom < toPoint.y) {
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
    else if (canScrollVertical() && toPoint.x < right) {
        fitPoint = CCPointMake(right, top);
    }
    // left
    else if (canScrollVertical() && left < toPoint.x) {
        fitPoint = CCPointMake(left, top);
    }
    else {
        CCPoint byPoint = CCPointMake((canScrollVertical() ? delta.x : 0.0), (canScrollHorizontal() ? delta.y : 0.0));
        CCPoint toPoint = _scrollLayer->getPosition() + byPoint;
        
        bool isPagingEnabled = false;
        
        // MEMO:
        // invert position.x while searching paging point.
        // (scrollLayer position goes negative when offset goes positive)
        toPoint.x *= -1.0; 
        
        // search paging point
        vector<CCPoint> pagingPoints;
        int nearestIndex = 0;
        CCArray* children = _scrollLayer->getChildren();
        for (int i = 0; i < children->count(); i++) {
            CCNode* node = dynamic_cast<CCNode*>(children->objectAtIndex(i));
            if (!node) continue;
            
            if (_delegate && _delegate->isPagingPointNode(node, this)) {
                CCPoint p = node->getPosition();
                p.x -= this->getContentSize().width * 0.5;
                pagingPoints.push_back(p);
                
                isPagingEnabled = true;
            }
        }
        if (pagingPoints.size() != 0) {
            // calc distances between "scroll offset" & "paging point".
            vector<float> distances;
            vector<CCPoint>::iterator iP = pagingPoints.begin();
            for (; iP != pagingPoints.end(); iP++) {
                CCPoint p = *iP;
                float distance = p.getDistance(toPoint);
                distances.push_back(distance);
            }
            // find the neareset "paging point".
            vector<float>::iterator iD = distances.begin();
            float nearestDistance;
            for (int i = 0; iD != distances.end(); i++, iD++) {
                if (i == 0 || *iD < nearestDistance) {
                    nearestDistance = *iD;
                    nearestIndex = i;
                }
            }
            toPoint = pagingPoints[nearestIndex];
            toPoint = CCPointMake((canScrollVertical() ? toPoint.x : 0.0), (canScrollHorizontal() ? toPoint.y : 0.0));
        }
        // back to positive.
        toPoint.x *= -1.0;
        
        // will end callback
        if (isPagingEnabled) {
            CCNode* targetNode = dynamic_cast<CCNode*>(children->objectAtIndex(nearestIndex));
            _delegate->pagingScrollWillEnd(targetNode, this);
        }
        CCMoveTo* moveTo = CCMoveTo::create(0.87, toPoint);
        CCArray* actions = CCArray::create(CCEaseSineOut::create(moveTo), NULL);
        
        // did end callback
        if (isPagingEnabled) {
            void* node = children->objectAtIndex(nearestIndex);
            CCCallFuncND* didEndCallback = CCCallFuncND::create(this, callfuncND_selector(SelectiveScroll::pagingScrollDidEndCallback), node);
            actions->addObject(didEndCallback);
        }
        return (CCAction*)CCSequence::create(actions);
    }
    CCMoveTo* moveTo = CCMoveTo::create(1.0, fitPoint);
    return (CCAction*)easeAction(moveTo, _topBoundingEffect);
}

void SelectiveScroll::detectSelectedItem(CCPoint p)
{
    bool cancelSelection;
    
    if (this->canScrollHorizontal()) {
        cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.y - _lastTouchPoint.y));
    }
    else {
        cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.x - _lastTouchPoint.x));
    }
    
    // convert "view touch point" to "layer touch point"
    CCRect rect = this->absoluteBoundingBox();
    float winHeight = CCDirector::sharedDirector()->getWinSize().height;
    CCPoint pointOnThisLayer = ccp((p.x - rect.origin.x), (p.y - (winHeight - rect.size.height) + rect.origin.y));
    CCPoint pointOnScrollLayer = CCPointZero;
    pointOnScrollLayer.x = -_scrollLayer->getPositionX() + pointOnThisLayer.x;
    pointOnScrollLayer.y = rect.size.height - _scrollLayer->getPositionY() - pointOnThisLayer.y;
    
    // reset first.
    _selectedItem = NULL;
    
    // search selected item
    CCArray* sprites = _scrollLayer->getChildren();
    if (sprites == NULL) return;
    
    for (int i = 0; i < sprites->count(); i++) {
        CCNode* node = dynamic_cast<CCNode*>(sprites->objectAtIndex(i));
        if (!node) return;
        
        bool isSelected = !cancelSelection && (CCSprite*)node->boundingBox().containsPoint(pointOnScrollLayer);
        if (isSelected) {
            _selectedItem = (CCSprite*)node;
        }
        // delegate callback 
        if (_delegate) {
            // if selection state hasn't changed, change it.
            if (_delegate->isLayerSelected(node, this) != isSelected) {
                _delegate->selectiveScrollHighlightLayer(isSelected, node, this);
            }
        }
    }
}


// began
bool SelectiveScroll::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint p = pTouch->getLocationInView();
    
    // IMPORTANT:
    // convert GL-coordinate to View-coordinate.
    CCSize winSize =  CCDirector::sharedDirector()->getWinSize();
    CCRect viewRect = this->absoluteBoundingBox();
    viewRect.origin.y = winSize.height - viewRect.size.height - viewRect.origin.y;
    
    // is outside of this layer, ignore touch event.
    if (_clipScrollInteraction && !viewRect.containsPoint(p)) {
        return false;
    }
    // stop scrolling animation.
    this->stopAction((CCAction*)_runningAction);
    
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
        if (canScrollVertical()) {
            _scrollLayer->setPositionX(_beganScrollPosition.x - _beganTouchPoint.x + p.x);
        }
        else if (canScrollHorizontal()) {
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
        delta.x *= -(14.0 <= abs(delta.x) ? 12.0 : 0.0);
        delta.y *= (14.0 <= abs(delta.y) ? 12.0 : 0.0);
        
        // fit
        CCAction* fitAction = this->fitToAction(delta);
        _scrollLayer->runAction(fitAction);
        _runningAction = fitAction;
        
        // has selected item.
        if (_selectedItem != NULL) {
            SelectiveScrollDelegate* delegate = this->getDelegate();
            if (delegate) {
                delegate->selectiveScrollHighlightLayer(false, (CCLayer*)_selectedItem, this);
                delegate->selectiveScrollDidSelectLayer((CCLayer*)_selectedItem, this);
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

