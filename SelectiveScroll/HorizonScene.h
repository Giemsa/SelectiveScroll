//
//  HorizonScene.h
//  SelectiveScroll
//
//  Created by nakagawa.toshitaka on 2013/08/11.
//
//

#ifndef __SelectiveScroll__HorizonScene__
#define __SelectiveScroll__HorizonScene__

#include "cocos2d.h"
#include "SelectiveScroll.h"

using namespace std;
USING_NS_CC;


class HorizonScene : public cocos2d::CCLayer, public SelectiveScrollDelegate {
private:
    
public:
    CC_SYNTHESIZE(SelectiveScroll*, _scroll, Scroll);
    CC_SYNTHESIZE(CCLayerColor*, _bgLayer, BGLayer);

    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HorizonScene);
    
    // SelectiveScroll (Delegate)
    virtual bool isLayerSelected(CCLayer* layer);
    virtual void selectiveScrollHighlightLayer(bool hi, CCLayer* layer);
    virtual void selectiveScrollDidSelectLayer(CCLayer* layer);
};

#endif /* defined(__SelectiveScroll__HorizonScene__) */
