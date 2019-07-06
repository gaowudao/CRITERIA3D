#ifndef KEYBOARDFILTER_H
#define KEYBOARDFILTER_H

#include <QObject>

class KeyboardFilter : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // KEYBOARDFILTER_H
