#include "tableDelegate.h"
#include <QLineEdit>
#include <QDoubleValidator>

TableDelegate::TableDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget* TableDelegate::createEditor(QWidget* parent,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QLineEdit* editor = new QLineEdit(parent);
    QDoubleValidator* val = new QDoubleValidator(editor);
    val->setBottom(0);
    val->setNotation(QDoubleValidator::StandardNotation);
    editor->setValidator(val);
    return editor;
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index,Qt::EditRole).toDouble();
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    line->setText(QString().setNum(value));
}

void TableDelegate::setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex &index) const
{
    QLineEdit* line = static_cast<QLineEdit*>(editor);
    QString value = line->text();
    model->setData(index,value);
}

void TableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
