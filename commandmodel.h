#ifndef DATATABLE_H
#define DATATABLE_H

#include "field.h"
#include "parcelmodel.h"
#include <QAbstractTableModel>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

class Command {
public:
    explicit Command(QString name, quint8 index, bool newCommand = true)
        : name_(name)
        , index_(index) //
    {
        if(newCommand) {
            tx_ = {Field("Start1"), Field("Start2"), Field("Size"), /*Field("Addres"),*/ Field("Command"), Field("CRC")};
            rx_ = {Field("Start1"), Field("Start2"), Field("Size"), /*Field("Addres"),*/ Field("Command"), Field("CRC")};
            tx_[0].setValue<uint8_t>(0x55);
            tx_[1].setValue<uint8_t>(0xAA);
            tx_[2].setValue<uint8_t>(tx_.size());
            tx_[3].setValue<uint8_t>(0);
            tx_[4].setValue<uint8_t>(index_);
        }
    }
    QString name() const { return name_; }
    void setName(const QString& name) { name_ = name; }

    quint8 index() const { return index_; }
    void setIndex(quint8 index) { index_ = index; }

    QList<Field>& tx() { return tx_; }
    QList<Field>& rx() { return rx_; }
    const QList<Field>& tx() const { return tx_; }
    const QList<Field>& rx() const { return rx_; }

private:
    QList<Field> tx_;
    QList<Field> rx_;

    QString name_;
    quint8 index_;
};

class CommandModel : public QAbstractTableModel {
    Q_OBJECT
    QList<Command> data_;
    QString name_;
    ParcelModel* tx_;

signals:
    void resetParcelModel();

public:
    explicit CommandModel(QObject* parent = Q_NULLPTR, ParcelModel* tx = nullptr);
    ~CommandModel() override;

    enum {
        Name,
        Index,
        ColumnCount
    };

    QList<Field>& txData(int row) { return data_[row].tx(); }
    QList<Field>& rxData(int row) { return data_[row].rx(); }
    const QList<Field>& txData(int row) const { return data_[row].tx(); }
    const QList<Field>& rxData(int row) const { return data_[row].rx(); }

    void new_();
    void open(const QString& name);

    void save();
    void saveAs(const QString& name);

    // QAbstractItemModel interface
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex& = {}) const override;
    int rowCount(const QModelIndex& = {}) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;
};

#endif // DATATABLE_H
