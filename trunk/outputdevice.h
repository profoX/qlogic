#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include "sceneitem.h"

class OutputDevice : public SceneItem
{
public:
    OutputDevice(ItemType type, QMenu *contextMenu);
    void processIncomingSignals();

protected:
    void setSvgs();
};

#endif // OUTPUTDEVICE_H
