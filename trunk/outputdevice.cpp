#include "outputdevice.h"

OutputDevice::OutputDevice(ItemType type, QMenu *contextMenu)
{
    myType = type;
    myContextMenu = contextMenu;
    mySignalType = SceneItem::Receiver;
    initItem();
}
