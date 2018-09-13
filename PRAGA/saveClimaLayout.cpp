#include "saveClimaLayout.h".h"


SaveClimaLayout::SaveClimaLayout()
{
    add.setText("Add");
    del.setText("Delete");
    delAll.setText("Delete all");
    buttonLayout.addWidget(&add);
    buttonLayout.addWidget(&del);
    buttonLayout.addWidget(&delAll);

    listLayout.addWidget(&listview);

    saveList.setText("Save list");
    loadList.setText("Load list");
    saveButtonLayout.addWidget(&saveList);
    saveButtonLayout.addWidget(&loadList);

    mainLayout.addLayout(&buttonLayout);
    mainLayout.addLayout(&listLayout);
    mainLayout.addLayout(&saveButtonLayout);

    setLayout(&mainLayout);

}

