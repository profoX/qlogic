#include "logicgate.h"

LogicGate::LogicGate(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::SenderAndReceiver;
    initItem();
}
