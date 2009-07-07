#ifndef LOGICGATE_H
#define LOGICGATE_H

#include "sceneitem.h"

class LogicGate : public SceneItem
{
public:
    LogicGate(ItemType type, QMenu *contextMenu);
    void processIncomingSignals();

protected:
    void setSvgs();

private:
    bool inverted;
};

#endif // LOGICGATE_H
