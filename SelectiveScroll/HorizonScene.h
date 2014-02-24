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
    SelectiveScroll* _scroll;
    CCLayerColor* _bgLayer;
public:

    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(HorizonScene);
    
    // SelectiveScroll (Delegate)
    virtual bool isLayerSelected(CCNode* node, SelectiveScroll* sender);
    virtual void selectiveScrollHighlightLayer(bool hi, CCNode* node, SelectiveScroll* sender);
    virtual void selectiveScrollDidSelectLayer(CCNode* node, SelectiveScroll* sender);
};

#endif /* defined(__SelectiveScroll__HorizonScene__) */
