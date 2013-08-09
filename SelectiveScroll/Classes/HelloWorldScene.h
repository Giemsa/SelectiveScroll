#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SelectiveScroll.h"

class HelloWorld : public cocos2d::CCLayer, public SelectiveScrollDelegate
{
public:
    CC_SYNTHESIZE(SelectiveScroll*, _scroll, Scroll);
    CC_SYNTHESIZE(CCLayerColor*, _bgLayer, BGLayer);
    
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HelloWorld);
    
    void menuCloseCallback(CCObject* pSender);
    
    // SelectiveScrollDelegate methods
    virtual bool isLayerSelected(CCLayer* layer);
    virtual void selectiveScrollHighlightLayer(bool hi, CCLayer* layer);
    virtual void selectiveScrollDidSelectLayer(CCLayer* layer);
};

#endif // __HELLOWORLD_SCENE_H__
