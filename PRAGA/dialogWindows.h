#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>
    #include "color.h"
    #include "meteo.h"

    colorScale::type chooseColorScale();
    frequencyType chooseFrequency();
    bool chooseVariable();
    QString editValue(QString windowsTitle, QString defaultValue);

#endif // DIALOGWINDOWS_H
