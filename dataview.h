#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QItemDelegate>
#include <QTableView>
#include <QTimer>

class MyItemDelegate : public QItemDelegate {

public:
    MyItemDelegate(QObject* parent = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // QItemDelegate interface
protected:
    void drawFocus(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect) const override;
};

class DataView : public QTableView {
    Q_OBJECT
public:
    explicit DataView(QWidget* parent = nullptr);

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

    // QAbstractItemView interface
protected slots:
    void rowsInserted(const QModelIndex& parent, int start, int end) override;
};

#endif // TABLEVIEW_H
