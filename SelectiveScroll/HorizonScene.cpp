//
//  HorizonScene.cpp
//  SelectiveScroll
//
//  Created by nakagawa.toshitaka on 2013/08/11.
//
//

#include "HorizonScene.h"
#include "HelloWorldScene.h"

#define WINSIZE CCDirector::sharedDirector()->getWinSize()
#define WINCENTER CCPointMake(WINSIZE.width*0.5, WINSIZE.height*0.5)

using namespace cocos2d;

CCScene* HorizonScene::scene()
{
    CCScene* scene = CCScene::create();
    HorizonScene* layer = HorizonScene::create();
    scene->addChild(layer);
    
    return scene;
}

bool HorizonScene::init()
{
    if (!CCLayer::init()) {
        return false;
    }
    
    // setup BG
    ccColor4B color = ccc4(255.0 * 0.9, 255.0 * 0.9, 255.0 * 0.9, 255.0 * 1.0);
    _bgLayer = CCLayerColor::create(color, WINSIZE.width, WINSIZE.height);
    _bgLayer->setPosition(CCPointZero);
    _bgLayer->retain();
    this->addChild(_bgLayer);
    
    // setup scroll
    CCSize size = CCSizeMake(WINSIZE.width * 0.8, WINSIZE.height * 0.8);
    SelectiveScroll* scroll = SelectiveScroll::create();
    scroll->setPosition(WINCENTER.x, WINCENTER.y);
    scroll->setBoundingEffectKind(BoundingEffectElastic);
    scroll->setContentSize(size);
    scroll->setDelegate(this);
    scroll->retain();
    
    // add labels
    float lastY = 0.0;
    int rowCount = 50;
    for (int ii = 0; ii < rowCount; ii++) {
        CCLabelTTF* label = CCLabelTTF::create("", "Helvetica", 44);
        
        // position
        float margin = 100;
        float y = margin + (label->getScaleY() + margin) * ii;
        label->setPosition(CCPointMake(size.width * 0.5, y));
        scroll->getScrollLayer()->addChild(label);
        
        CCPoint p = label->getPosition();
        string title = to_string(rowCount - ii) + " (" + to_string((int)p.y) + ")";
        label->setString(title.c_str());
        
        lastY = p.y + margin;
    }
    scroll->setScrollSize(CCSizeMake(size.width, lastY));
    scroll->scrollToTop();
    
    this->addChild(scroll);
    
    return true;
}


#pragma mark - DELEGATE
#pragma mark SelectiveScroll (Delegate)

bool HorizonScene::isLayerSelected(CCLayer* layer)
{
    return ccc3BEqual(((CCLabelTTF*)layer)->getColor(), ccBLACK);
}

void HorizonScene::selectiveScrollHighlightLayer(bool hi, CCLayer* layer) 
{
    CCLabelTTF* label = (CCLabelTTF*)layer;
    label->setColor(hi ? ccBLACK : ccWHITE);
}

void HorizonScene::selectiveScrollDidSelectLayer(CCLayer* layer) 
{
    CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

