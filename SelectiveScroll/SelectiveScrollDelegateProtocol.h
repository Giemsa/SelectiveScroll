//
//  SelectiveScrollDelegateProtocol.h
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/08.
//
//

#ifndef __SelectiveScroll__SelectiveScrollDelegateProtocol__
#define __SelectiveScroll__SelectiveScrollDelegateProtocol__

#include "cocos2d.h"

USING_NS_CC;

class SelectiveScrollDelegate {
    
public:
    virtual bool isLayerSelected(CCNode* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender); return false;}
    virtual void selectiveScrollHighlightLayer(bool hi, CCNode* node, void* sender) {CC_UNUSED_PARAM(hi), CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    virtual void selectiveScrollDidSelectLayer(CCNode* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    
    // paging
    virtual bool isPagingPointNode(CCNode* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender); return false;}
    virtual void pagingScrollWillEnd(CCNode* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    virtual void pagingScrollDidEnd(CCNode* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
};


#endif /* defined(__SelectiveScroll__SelectiveScrollDelegateProtocol__) */
