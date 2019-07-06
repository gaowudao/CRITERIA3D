#include "keyboardFilter.h"
#include "QEvent"

bool KeyboardFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}
