#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

// scene
#include "HorizonScene.h"

#define WINSIZE CCDirector::sharedDirector()->getWinSize()
#define WINCENTER CCPointMake(WINSIZE.width*0.5, WINSIZE.height*0.5)

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

HelloWorld::HelloWorld() {}

HelloWorld::~HelloWorld()
{
    CCLOG("%s", __PRETTY_FUNCTION__);
}

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
    int count = BoundingEffect_Count;
    for (int i = 0; i < count; i++) {
        CCSize size = CCSizeMake(WINSIZE.width / count * 0.7, WINSIZE.height * 0.7);
        SelectiveScroll* scroll = SelectiveScroll::create();
        scroll->setPosition(CCSizeMake(WINSIZE.width / (count + 1) * (i + 1), WINCENTER.y));
        scroll->setBoundingEffectKind((BoundingEffect)i);
        scroll->setContentSize(size);
        scroll->setDelegate(this);
        scroll->clipToBounds(true);
        scroll->retain();
        
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
        this->addChild(scroll);
    }
    return true;
}


#pragma mark - DELEGATE
#pragma mark SelectiveScroll (Delegate)

bool HelloWorld::isLayerSelected(CCLayer* layer)
{
    return ccc3BEqual(((CCLabelTTF*)layer)->getColor(), ccBLACK);
}

void HelloWorld::selectiveScrollHighlightLayer(bool hi, CCLayer* layer)
{
    CCLabelTTF* label = (CCLabelTTF*)layer;
    label->setColor(hi ? ccBLACK : ccWHITE);
}

void HelloWorld::selectiveScrollDidSelectLayer(CCLayer* layer)
{
    CCTransitionFade* fade = CCTransitionFade::create(0.5, HorizonScene::scene(), ccWHITE);
    CCDirector::sharedDirector()->replaceScene(fade);
    
//    CCArray* scrolls = this->getChildren();
//    
//    // find the caller 
//    SelectiveScroll* callerScroll = NULL;
//    for (int i = 0; i < scrolls->count(); i++) {
//        SelectiveScroll* scroll = dynamic_cast<SelectiveScroll*>(scrolls->objectAtIndex(i));
//        if (!scroll) continue;
//        
//        CCArray* items = scroll->getScrollLayer()->getChildren();
//        // is caller scroll layer
//        if (items->containsObject(layer)) {
//            callerScroll = scroll;
//        }
//    }
//    
//    // sync scroll points
//    for (int i = 0; i < scrolls->count(); i++) {
//        SelectiveScroll* scroll = dynamic_cast<SelectiveScroll*>(scrolls->objectAtIndex(i));
//        if (!scroll) continue;
//        
//        if (!callerScroll->isEqual(scroll)) {
//            CCPoint p = callerScroll->getScrollLayer()->getPosition();
//            scroll->scrollToPointWithAnimation(p);
//        }
//    }
}




