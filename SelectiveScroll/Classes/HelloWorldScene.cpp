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
    const int count = BoundingEffect_Count;
    const CCSize size(WINSIZE.width / count * 0.7, WINSIZE.height * 0.7);
    for (int i = 0; i < count; i++) {
        CCLayerColor* container = CCLayerColor::create(ccc4(0xFF, 0x00, 0x00, 0x88), 600.0f, 600.0f);
        container->setAnchorPoint(CCPointMake(0.0, 0.0));
        
        CCLayerColor* background = CCLayerColor::create(ccc4(0x00, 0x00, 0xFF, 0x88), 600.0f, 600.0f);
        background->setAnchorPoint(CCPointMake(0.0, 0.0));
        
        SelectiveScroll* scroll = SelectiveScroll::create();
        scroll->setContainer(container);
        scroll->setBackground(background);
        scroll->setPosition(CCSizeMake(WINSIZE.width / (count + 1) * (i + 1), WINCENTER.y));
        scroll->setBoundingEffectKind((BoundingEffect)i);
        scroll->setContentSize(size);
        scroll->setDelegate(this);
        scroll->clipToBounds(true);
        
        float lastY = 0.0;
        int rowCount = 50;
        for (int ii = 0; ii < rowCount; ii++) {
            CCLabelTTF* label = CCLabelTTF::create("", "Helvetica", 44);
            
            // position
            float margin = 100;
            float y = margin + (label->getScaleY() + margin) * ii;
            label->setPosition(CCPointMake(size.width * 0.5, y));
            container->addChild(label);
            
            CCPoint p = label->getPosition();
            string title = to_string(rowCount - ii) + " (" + to_string((int)p.y) + ")";
            label->setString(title.c_str());
            
            lastY = p.y + margin;
        }
        scroll->setScrollSize(CCSizeMake(size.width, lastY));
        scroll->scrollToTop();
        this->addChild(scroll);
    }

    return true;
}


#pragma mark - DELEGATE
#pragma mark SelectiveScroll (Delegate)

bool HelloWorld::isLayerSelected(CCNode* node, SelectiveScroll* sender)
{
    return ccc3BEqual(((CCLabelTTF*)node)->getColor(), ccBLACK);
}

void HelloWorld::selectiveScrollHighlightLayer(bool hi, CCNode* node, SelectiveScroll* sender)
{
    CCLabelTTF* label = (CCLabelTTF*)node;
    label->setColor(hi ? ccBLACK : ccWHITE);
}

void HelloWorld::selectiveScrollDidSelectLayer(CCNode* node, SelectiveScroll* sender)
{
    CCTransitionFade* fade = CCTransitionFade::create(0.5, HorizonScene::scene(), ccWHITE);
    CCDirector::sharedDirector()->replaceScene(fade);
}




