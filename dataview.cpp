#include "dataview.h"
#include "commandmodel.h"
#include "field.h"
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

const int id2 = qRegisterMetaType<Field>("Field");

MyItemDelegate::MyItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

template <class T>
using limits = std::numeric_limits<T>;

//Необходимо замутить свой QSpinBox так как родной uint64_t не подерживает.
QWidget* MyItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const Field& field = index.data(Qt::UserRole).value<Field>();

    if (index.column() == 1) {
        QComboBox* dlg = new QComboBox(parent); //создаем наше поле ввода с кнопкой
        dlg->addItems(Field().typeNames().toList());
        dlg->setCurrentIndex(field.type());
        return dlg;
    } else if (index.column() == 3) {
        QDoubleSpinBox* dsbx;
        dsbx = new QDoubleSpinBox(parent);
        dsbx->setDecimals(0);

        std::visit([dsbx](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr ( //
                std::is_same_v<T, int8_t> || //
                std::is_same_v<T, int16_t> || //
                std::is_same_v<T, int32_t> || //
                std::is_same_v<T, int64_t> || //
                std::is_same_v<T, uint8_t> || //
                std::is_same_v<T, uint16_t> || //
                std::is_same_v<T, uint32_t> || //
                std::is_same_v<T, uint64_t>) {
                dsbx->setRange(limits<T>::min(), limits<T>::max());
                dsbx->setValue(arg);
                dsbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(dsbx->minimum()).arg(dsbx->maximum()));
                dsbx->setDecimals(0);
            } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                dsbx->setRange(-limits<T>::max(), limits<T>::max());
                dsbx->setValue(arg);
                dsbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(dsbx->minimum()).arg(dsbx->maximum()));
                dsbx->setDecimals(16);
            } else
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
        },
            field.value());
        return dsbx;
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

void DataView::rowsInserted(const QModelIndex& /*parent*/, int /*start*/, int /*end*/) { resizeRowsToContents(); }
