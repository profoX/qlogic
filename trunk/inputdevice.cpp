#include "inputdevice.h"

InputDevice::InputDevice(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::Sender;
    initItem();
}
