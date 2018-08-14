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
    }
    //    else if (index.column() == 3) {
    //        QSpinBox* sbx;
    //        QDoubleSpinBox* dsbx;
    //        int type = index.sibling(index.row(), 1).data(Qt::UserRole).toInt();
    //        if (type < 6)
    //            sbx = new QSpinBox(parent); //создаем наше поле ввода с кнопкой
    //        else
    //            dsbx = new QDoubleSpinBox(parent);
    //        switch (type) {
    //        case 0:
    //            sbx->setRange(std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());
    //            sbx->setValue(int8_t(index.data(Qt::UserRole).toInt()));
    //            break;
    //        case 1:
    //            sbx->setRange(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
    //            sbx->setValue(int16_t(index.data(Qt::UserRole).toInt()));
    //            break;
    //        case 2:
    //            sbx->setRange(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());
    //            sbx->setValue(int32_t(index.data(Qt::UserRole).toInt()));
    //            break;
    //        case 3:
    //            sbx->setRange(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
    //            sbx->setValue(uint8_t(index.data(Qt::UserRole).toUInt()));
    //            break;
    //        case 4:
    //            sbx->setRange(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
    //            sbx->setValue(uint16_t(index.data(Qt::UserRole).toUInt()));
    //            break;
    //        case 5:
    //            sbx->setRange(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
    //            sbx->setValue(uint32_t(index.data(Qt::UserRole).toUInt()));
    //            break;
    //        case 6:
    //            dsbx->setRange(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    //            dsbx->setValue(uint32_t(index.data(Qt::UserRole).toFloat()));
    //            break;
    //        case 7:
    //            dsbx->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    //            dsbx->setValue(uint32_t(index.data(Qt::UserRole).toDouble()));
    //            break;
    //        }
    //        if (type < 6) {
    //            sbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(sbx->minimum()).arg(sbx->maximum()));
    //            return sbx;
    //        }
    //        else {
    //            dsbx->setToolTip(QString("Принимаемые значения от %1 до %2").arg(dsbx->minimum()).arg(dsbx->maximum()));
    //            dsbx->setDecimals(12);
    //            return dsbx;
    //        }
    //    }

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
    connect(&resizeTtimer, &QTimer::timeout, [&]() {
        resizeTtimer.stop();
        int columnCount = model()->columnCount();
        if (!columnCount)
            return;

        int width = size().width() - verticalHeader()->width() - 2;
        if (verticalScrollBar()->isVisible())
            width -= verticalScrollBar()->width();

        if (objectName() == "tvCommand") {
            resizeColumnToContents(1);
            setColumnWidth(0, width - columnWidth(1));
        } else {
            resizeColumnToContents(0);
            setColumnWidth(1, 60);
            resizeColumnToContents(2);
            setColumnWidth(3, width - columnWidth(0) - columnWidth(1) - columnWidth(2));
        }
        resizeRowsToContents();
    });
}

void DataView::showEvent(QShowEvent* /*event*/)
{
    resizeTtimer.start(50);
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

void DataView::resizeEvent(QResizeEvent* event)
{
    resizeTtimer.start(50);
    QWidget::resizeEvent(event);
}

void DataView::rowsInserted(const QModelIndex& /*parent*/, int /*start*/, int /*end*/)
{
    resizeRowsToContents();
}