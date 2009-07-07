#ifndef BUBBLEITEM_H
#define BUBBLEITEM_H

#include "sceneitem.h"

class BubbleItem : public SceneItem
{
public:
    enum BubbleIcon {Invalid, Input};
    BubbleItem(BubbleIcon icon);
    BubbleIcon icon() {return myIcon;}

protected:
    void setSvgs();

private:
    BubbleIcon myIcon;
};

#endif // BUBBLEITEM_H
