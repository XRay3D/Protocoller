#ifndef ParcelTable_H
#define ParcelTable_H

#include "field.h"

#include <QAbstractTableModel>

#include <hw/myprotokol.h>

class ParcelModel : public QAbstractTableModel, public MyProtokol {
    Q_OBJECT
public:
    explicit ParcelModel(QObject* parent = Q_NULLPTR);
    ~ParcelModel() override;
    void setData(QList<Field>* data = nullptr);
    void setCommand(uint8_t command);
    const QByteArray& parcel();
    void setParcel(const QByteArray& data);

signals:
    void error(const QString& error);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

private:
    QList<Field>* m_data = nullptr;
    QByteArray m_parcel;
    void updateCrc();
};

#endif // ParcelTable_H
