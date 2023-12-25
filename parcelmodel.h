#ifndef ParcelTable_H
#define ParcelTable_H

#include "field.h"

#include <QAbstractTableModel>

#include <hw/myprotokol.h>

class ParcelModel : public QAbstractTableModel, public MyProtokol {
    Q_OBJECT

    QList<Field>* data_ = nullptr;
    QByteArray m_parcel;
    void updateCrc();
    bool m_useAddress{};

    enum Rows {
        Start1,
        Start2,
        Size,
        Address,
        Command,
        Data,
        Crc,
    };
    enum Columns {
        Name,
        Type,
        TypeSize,
        Value,
        ValueHex,
        ColumnCount,
    };

signals:
    void error(const QString& error);

public:
    explicit ParcelModel(QObject* parent = Q_NULLPTR);
    ~ParcelModel() override;
    void setData(QList<Field>* data = nullptr);
    void setCommand(uint8_t command);
    const QByteArray& parcel();
    void setParcel(const QByteArray& data);

    void setUseAddress(bool fl);
    void setAddress(int val);

    // QAbstractItemModel interface

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;

    bool insertRows(int row, int = 1, const QModelIndex& = {}) override;
    bool removeRows(int row, int = 1, const QModelIndex& = {}) override;
};

#endif // ParcelTable_H
