#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "sceneitem.h"

class InputDevice : public SceneItem
{
public:
    InputDevice(ItemType type, QMenu *contextMenu);
    void initAfterCreation();

protected:
    void setSvgs();

private:
    QTimeLine *oscillator;
};

#endif // INPUTDEVICE_H
