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
    scroll->setBoundingEffectKind(BoundingEffectBack);
    scroll->setContentSize(size);
    scroll->setDelegate(this);
    scroll->retain();
    
    // add labels
    int rowCount = 50;
    float lastX = 0.0;
    for (int i = 0; i < rowCount; i++) {
        CCLabelTTF* label = CCLabelTTF::create("", "Helvetica", 44);
        
        // position
        label->setPosition(CCPointMake(size.width * (i + 1) * 0.333, size.height * 0.5));
        
        // add
        scroll->getScrollLayer()->addChild(label);
        
        CCPoint p = label->getPosition();
        string title = to_string(rowCount - i) + " (" + to_string((int)p.y) + ")";
        label->setString(title.c_str());
        
        CCRect rect = label->boundingBox();
        lastX = rect.origin.x + rect.size.width * 2.0;
    }
    scroll->setScrollSize(CCSizeMake(lastX, size.height));
    scroll->scrollToTop();
    
    this->addChild(scroll);
    
    return true;
}


#pragma mark - DELEGATE
#pragma mark SelectiveScroll (Delegate)

bool HorizonScene::isLayerSelected(CCNode* node, void* sender)
{
    return ccc3BEqual(((CCLabelTTF*)node)->getColor(), ccBLACK);
}

void HorizonScene::selectiveScrollHighlightLayer(bool hi, CCNode* node, void* sender) 
{
    CCLabelTTF* label = (CCLabelTTF*)node;
    label->setColor(hi ? ccBLACK : ccWHITE);
}

void HorizonScene::selectiveScrollDidSelectLayer(CCNode* node, void* sender)
{
    CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

