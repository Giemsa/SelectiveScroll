#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SelectiveScroll.h"

USING_NS_CC;

class HelloWorld : public cocos2d::CCLayer, public SelectiveScrollDelegate
{
private:
    SelectiveScroll* _scroll;
    CCLayerColor* _bgLayer;
public:
    HelloWorld();
    virtual ~HelloWorld();
    
    
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HelloWorld);
    
    void menuCloseCallback(CCObject* pSender);
    
    // SelectiveScrollDelegate methods
    virtual bool isLayerSelected(CCNode* node, SelectiveScroll* sender);
    virtual void selectiveScrollHighlightLayer(bool hi, CCNode* node, SelectiveScroll* sender);
    virtual void selectiveScrollDidSelectLayer(CCNode* node, SelectiveScroll* sender);
};

#endif // __HELLOWORLD_SCENE_H__
