#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include "sceneitem.h"

class OutputDevice : public SceneItem
{
public:
    OutputDevice(ItemType type, QMenu *contextMenu);
};

#endif // OUTPUTDEVICE_H
