#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define WINSIZE CCDirector::sharedDirector()->getWinSize()
#define WINCENTER CCPointMake(WINSIZE.width*0.5, WINSIZE.height*0.5)

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if (!CCLayer::init()) {
        return false;
    }
    
    // setup BG
    ccColor4B color = ccc4(255.0 * 0.9, 255.0 * 0.9, 255.0 * 0.9, 255.0 * 1.0);
    _bgLayer = CCLayerColor::create(color, WINSIZE.width, WINSIZE.height);
    _bgLayer->setPosition(CCPointZero);
    this->addChild(_bgLayer);
    
    // setup scroll
    for (int i = 0; i < BoundingEffect_Count; i++) {
        CCSize size = CCSizeMake(WINSIZE.width / (BoundingEffect_Count + 2), WINSIZE.height * 0.9);
        SelectiveScroll* scroll = SelectiveScroll::create();
        scroll->setPosition(CCSizeMake(WINSIZE.width / (BoundingEffect_Count + 1) * (i + 1), WINCENTER.y));
        scroll->setBoundingEffectKind((BoundingEffect)i);
        scroll->setContentSize(size);
        scroll->retain();
        this->addChild(scroll);
    }
    return true;
}
