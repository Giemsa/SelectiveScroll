//
//  SelectiveScroll.cpp
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/07.
//
//

#include "SelectiveScroll.h"
#include <cmath>
#include <limits>

namespace cocos2d
{
    static const float CANCEL_SELECTION_THRESHOLD = 4.0;
    static const float RUNNING_ANIMATION_TAG = 1;

    typedef std::vector<CCPoint> PointVector;
    typedef std::vector<float> FloatVector;

    // MEMO:
    // Define Layer order.
    namespace ZIndex
    {
        enum Type
        {
            BGLayer,
            ScrollLayer
        };
    }

    SelectiveScroll::SelectiveScroll()
    :   _container(NULL)
    ,   _background(NULL)
    ,   _clipToBounds(true)
    ,   _clipScrollInteraction(true)
    ,   _enableToScroll(true)
    ,   _scrollSize(CCSizeMake(600.0, 600.0))
    ,   _runningAction(NULL)
    {}

    SelectiveScroll::~SelectiveScroll()
    {
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
        
        return true;
    }

    void SelectiveScroll::draw()
    {
        // resize (if size was changed.)
        const CCSize &size = this->getContentSize();
       
        if(_background && !_background->getContentSize().equals(size)) {
            _background->setContentSize(size);
        }
        if (_container && !_container->getContentSize().equals(_scrollSize)) {
            _container->setContentSize(_scrollSize);
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
            const CCRect bound = this->absoluteBoundingBox();
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

    void SelectiveScroll::scrollToPoint(const CCPoint &p)
    {
        _container->setPosition(p);
    }

    void SelectiveScroll::scrollToPointWithAnimation(const CCPoint &p)
    {
        this->stopAction(_runningAction);
        CCMoveTo* moveTo = CCMoveTo::create(1.0, p);
        _container->runAction(CCEaseInOut::create(moveTo, 2.0));
    }


    #pragma mark Touch

    // returns Eased Action
    CCAction* SelectiveScroll::getEaseAction(CCMoveTo* moveTo, const BoundingEffect::Type effect)
    {
        switch (effect) {
            case BoundingEffect::Back:    return CCEaseBackOut::create(moveTo);
            case BoundingEffect::Bounce:  return CCEaseBounceOut::create(moveTo);
            case BoundingEffect::Elastic: return CCEaseElasticOut::create(moveTo);
            default:                      return CCEaseExponentialOut::create(moveTo);
        }
    }

    // returns bounds on Scene.
    CCRect SelectiveScroll::absoluteBoundingBox()
    {
        static const int LayerHierarchyMax = 100.0;

        CCNode* parentLayer = this->getParent();
        if (!parentLayer) return this->boundingBox();
        
        CCRect bound = this->boundingBox();
        CCNode* runningScene = CCDirector::sharedDirector()->getRunningScene();
        int layerDepth = 0;
        
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

    bool SelectiveScroll::canScrollVertical() const
    {
        return this->getContentSize().width < _scrollSize.width;
    }

    bool SelectiveScroll::canScrollHorizontal() const
    {
        return this->getContentSize().height < _scrollSize.height;
    }

    void SelectiveScroll::pagingScrollDidEndCallback(CCNode* node)
    {
        if (_delegate) {
            _delegate->pagingScrollDidEnd(node, this);
        }
    }

    CCAction* SelectiveScroll::fitToAction(const CCPoint &delta)
    {
        static const float bottom = 0.0;
        static const float left = 0.0;

        const CCPoint toPoint(_container->getPositionX() + delta.x, _container->getPositionY() + delta.y);
        CCPoint fitPoint = CCPointZero;
        const float top = this->getContentSize().height - _scrollSize.height;
        const float right = this->getContentSize().width - _scrollSize.width;
        
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
            const CCPoint byPoint((canScrollVertical() ? delta.x : 0.0), (canScrollHorizontal() ? delta.y : 0.0));
            CCPoint toPoint = _container->getPosition() + byPoint;
            
            bool isPagingEnabled = false;
            
            // MEMO:
            // invert position.x while searching paging point.
            // (scrollLayer position goes negative when offset goes positive)
            toPoint.x *= -1.0; 
            
            // search paging point
            PointVector pagingPoints;
            int nearestIndex = 0;
            CCArray* children = _container->getChildren();
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
                FloatVector distances;
                PointVector::iterator iP = pagingPoints.begin();
                for (; iP != pagingPoints.end(); ++iP) {
                    CCPoint p = *iP;
                    const float distance = p.getDistance(toPoint);
                    distances.push_back(distance);
                }
                // find the neareset "paging point".
                FloatVector::iterator iD = distances.begin();
                float nearestDistance = std::numeric_limits<float>::max();
                for (int i = 0; iD != distances.end(); i++, ++iD) {
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
                CCObject* node = children->objectAtIndex(nearestIndex);
                CCCallFuncND* didEndCallback = CCCallFuncND::create(this, callfuncND_selector(SelectiveScroll::pagingScrollDidEndCallback), node);
                actions->addObject(didEndCallback);
            }
            return CCSequence::create(actions);
        }
        CCMoveTo* moveTo = CCMoveTo::create(1.0, fitPoint);
        return getEaseAction(moveTo, _topBoundingEffect);
    }

    void SelectiveScroll::detectSelectedItem(const CCPoint &p)
    {
        bool cancelSelection;
        
        if (this->canScrollHorizontal()) {
            cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.y - _lastTouchPoint.y));
        }
        else {
            cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.x - _lastTouchPoint.x));
        }
        
        // convert "view touch point" to "layer touch point"
        const CCRect rect = this->absoluteBoundingBox();
        const float winHeight = CCDirector::sharedDirector()->getWinSize().height;
        CCPoint pointOnThisLayer((p.x - rect.origin.x), (p.y - (winHeight - rect.size.height) + rect.origin.y));
        CCPoint pointOnScrollLayer = CCPointZero;
        pointOnScrollLayer.x = -_container->getPositionX() + pointOnThisLayer.x;
        pointOnScrollLayer.y = rect.size.height - _container->getPositionY() - pointOnThisLayer.y;
        
        // reset first.
        _selectedItem = NULL;
        
        // search selected item
        CCArray* sprites = _container->getChildren();
        if (sprites == NULL) return;
        
        for (int i = 0; i < sprites->count(); i++) {
            CCNode* node = dynamic_cast<CCNode*>(sprites->objectAtIndex(i));
            if (!node) return;
            
            const bool isSelected = !cancelSelection && node->boundingBox().containsPoint(pointOnScrollLayer);
            if (isSelected) {
                _selectedItem = node;
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
        const CCPoint p = pTouch->getLocationInView();
        
        // IMPORTANT:
        // convert GL-coordinate to View-coordinate.
        const CCSize winSize =  CCDirector::sharedDirector()->getWinSize();
        CCRect viewRect = this->absoluteBoundingBox();
        viewRect.origin.y = winSize.height - viewRect.size.height - viewRect.origin.y;
        
        // is outside of this layer, ignore touch event.
        if (_clipScrollInteraction && !viewRect.containsPoint(p)) {
            return false;
        }
        // stop scrolling animation.
        this->stopAction(_runningAction);
        
        // save points for calc.
        _lastTouchPoint = p;
        _beganTouchPoint = p;
        _beganScrollPosition = _container->getPosition();
        
        this->detectSelectedItem(p);
        
        return true;
    }

    // moved
    void SelectiveScroll::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
    {
        // scroll
        const CCPoint p = pTouch->getLocationInView();
        
        if (_enableToScroll) {
            if (canScrollVertical()) {
                _container->setPositionX(_beganScrollPosition.x - _beganTouchPoint.x + p.x);
            }
            else if (canScrollHorizontal()) {
                _container->setPositionY(_beganScrollPosition.y + _beganTouchPoint.y - p.y);
            }
        }
        
        // save last (technically it is previous location)
        _lastTouchPoint = pTouch->getPreviousLocationInView();
        
        this->detectSelectedItem(p);
    }

    // ended
    void SelectiveScroll::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
    {
        const CCPoint p = pTouch->getLocationInView();
        
        if (_enableToScroll) {
            // speed scroll correction
            CCPoint delta(_lastTouchPoint.x - p.x, _lastTouchPoint.y - p.y);
            delta.x *= -(14.0 <= fabsf(delta.x) ? 12.0 : 0.0);
            delta.y *= (14.0 <= fabsf(delta.y) ? 12.0 : 0.0);
            
            // fit
            CCAction* fitAction = this->fitToAction(delta);
            _container->runAction(fitAction);
            _runningAction = fitAction;
            
            // has selected item.
            if (_selectedItem != NULL) {
                SelectiveScrollDelegate* delegate = this->getDelegate();
                if (delegate) {
                    delegate->selectiveScrollHighlightLayer(false, _selectedItem, this);
                    delegate->selectiveScrollDidSelectLayer(_selectedItem, this);
                }
            }
        }
    }


    #pragma mark - Setter/Getter

    // clipToBounds
    bool SelectiveScroll::getClipToBounds() const { return _clipToBounds; }
    void SelectiveScroll::setClipToBounds(const bool clip) { _clipToBounds = clip; _clipScrollInteraction = clip; }

    // bounce effect kind
    void SelectiveScroll::setBoundingEffectKind(const BoundingEffect::Type effect)
    {
        _topBoundingEffect = effect;
        _bottomBoundingEffect = effect;
    }

    void SelectiveScroll::setBoundingEffectKind(const BoundingEffect::Type top, const BoundingEffect::Type bottom)
    {
        _topBoundingEffect = top;
        _bottomBoundingEffect = bottom;
    }

    // enable/disable scroll
    bool SelectiveScroll::getEnableToScroll() const
    {
        return _enableToScroll;
    }
    void SelectiveScroll::setEnableToScroll(const bool enable)
    {
        _enableToScroll = enable;
    }

    void SelectiveScroll::setBackground(CCNode* bg)
    {
        if(_background) {
            _background->removeFromParent();
        }
        _background = bg;
        this->addChild(_background, ZIndex::BGLayer);
    }

    CCNode* SelectiveScroll::getBackground() const
    {
        return _background;
    }

    void SelectiveScroll::setContainer(CCNode* node)
    {
        if(_container) {
            _container->removeFromParent();
        }

        _container = node;
        this->addChild(_container, ZIndex::ScrollLayer);
    }

    CCNode* SelectiveScroll::getContainer() const
    {
        return _container;
    }
}
