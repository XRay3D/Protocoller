#include "dataview.h"
#include "commandmodel.h"
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>

#include <QApplication>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QSpinBox>
#include <limits>

MyItemDelegate::MyItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

//Необходимо замутить свой QSpinBox так как родной юинт не подерживает.
QWidget* MyItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 1) {
        QComboBox* dlg = new QComboBox(parent); //создаем наше поле ввода с кнопкой
        dlg->addItems(Field().typeNames());
        dlg->setCurrentIndex(index.data(Qt::UserRole).toInt());
        return dlg;
    } else if (index.column() == 3) {
        QDoubleSpinBox* dsbx;
        int type = index.sibling(index.row(), 1).data(Qt::UserRole).toInt();
        dsbx = new QDoubleSpinBox(parent);
        dsbx->setDecimals(0);
        switch (type) {
        case 0:
            dsbx->setRange(std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());
            dsbx->setValue(int8_t(index.data(Qt::UserRole).toInt()));
            break;
        case 1:
            dsbx->setRange(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
            dsbx->setValue(int16_t(index.data(Qt::UserRole).toInt()));
            break;
        case 2:
            dsbx->setRange(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());
            dsbx->setValue(int32_t(index.data(Qt::UserRole).toInt()));
            break;
        case 3:
            dsbx->setRange(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
            dsbx->setValue(uint8_t(index.data(Qt::UserRole).toUInt()));
            break;
        case 4:
            dsbx->setRange(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
            dsbx->setValue(uint16_t(index.data(Qt::UserRole).toUInt()));
            break;
        case 5:
            dsbx->setRange(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
            dsbx->setValue(uint32_t(index.data(Qt::UserRole).toUInt()));
            break;
        case 6:
            dsbx->setRange(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            dsbx->setValue(uint32_t(index.data(Qt::UserRole).toFloat()));
            break;
        case 7:
            dsbx->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
            dsbx->setValue(uint32_t(index.data(Qt::UserRole).toDouble()));
            break;
        }
        if (type < 6) {
            dsbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(dsbx->minimum()).arg(dsbx->maximum()));
            return dsbx;
        } else {
            dsbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(dsbx->minimum()).arg(dsbx->maximum()));
            dsbx->setDecimals(16);
            return dsbx;
        }
    }

    return QItemDelegate::createEditor(parent, option, index);
}

void MyItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 1) {
        model->setData(index, static_cast<QComboBox*>(editor)->currentIndex());
        return;
    }
    //    if (POWER == index.row()) {
    //        QComboBox* dlg = static_cast<QComboBox*>(editor); //создаем наше поле ввода с кнопкой
    //        model->setData(index, dlg->currentData());
    //        return;
    //    }
    QItemDelegate::setModelData(editor, model, index);
}

void MyItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //    if (index.row() >= 0 && index.row() <= 3) {
    //        painter->fillRect(option.rect, index.data(Qt::BackgroundColorRole).value<QColor>());
    //        painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());
    //        drawFocus(painter, option, option.rect);
    //    }
    //    else if (1) {
    //        painter->fillRect(option.rect, index.data(Qt::BackgroundColorRole).value<QColor>());
    //        painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());
    //        drawFocus(painter, option, option.rect);
    //    }
    //    else
    QItemDelegate::paint(painter, option, index);
}

void MyItemDelegate::drawFocus(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect) const
{
    if (!(option.state & QStyle::State_HasFocus) || !rect.isValid())
        return;
    painter->setBrush(QColor(255, 0, 0, 20));
    painter->setPen(Qt::red);
    painter->drawRect(QRect(rect.topLeft(), rect.size() - QSize(1, 1))); //без QSize(1, 1) вылезает на сетку, не красиво.
}

///////////////////////////////////////////
/// \brief TableView::TableView
/// \param parent
///
DataView::DataView(QWidget* parent)
    : QTableView(parent)
{

    //setEditTriggers(SelectedClicked);
}

void DataView::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;
    int row = indexAt(event->pos()).row();
    if (objectName() == "tvCommand") {
        menu.addAction(QIcon(), "Добавить команду", [&]() {
            model()->insertRow(model()->rowCount());
        });
        menu.addAction(QIcon(), "Удалить команду", [&]() {
            if (QMessageBox::question(this, "", "Точно?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                model()->removeRow(row);
        });
        menu.actions()[1]->setEnabled(row > -1);
    } else {
        menu.addAction(QIcon(), "Добавить поле", [&]() {
            model()->insertRow(model()->rowCount());
        });
        menu.addAction(QIcon(), "Удалить поле", [&]() {
            if (QMessageBox::question(this, "", "Точно?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                model()->removeRow(row);
        });
        menu.actions()[1]->setEnabled(row > 3 && row < model()->rowCount() - 1);
    }
    menu.exec(event->globalPos());
}

void DataView::rowsInserted(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    resizeRowsToContents();
}
