#pragma once

#include "field.h"
#include "parcelmodel.h"
#include <QAbstractTableModel>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

class Command {
    friend QDataStream& operator<<(QDataStream& ds, const Command& c)
    {
        ds << c.m_index;
        ds << c.m_name;
        ds << c.m_rx;
        ds << c.m_tx;
        return ds;
    }
    friend QDataStream& operator>>(QDataStream& ds, Command& c)
    {
        ds >> c.m_index;
        ds >> c.m_name;
        ds >> c.m_rx;
        ds >> c.m_tx;
        return ds;
    }

public:
    explicit Command()
        : m_index(-1)
        , m_name("")
    {
    }
    explicit Command(QString name, quint8 index, bool newCommand = true)
        : m_index(index)
        , m_name(name)

    {
        if (newCommand) {
            m_tx = { Field("Start1"), Field("Start2"), Field("Size"), Field("Addres"), Field("Command"), Field("CRC") };
            m_rx = { Field("Start1"), Field("Start2"), Field("Size"), Field("Addres"), Field("Command"), Field("CRC") };
            m_tx[0] = uint8_t(0x55);
            m_tx[1] = uint8_t(0xAA);
            m_tx[2] = uint8_t(m_tx.size());
            m_tx[3] = uint8_t(0);
            m_tx[4] = uint8_t(m_index);
        }
    }
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    quint8 index() const { return m_index; }
    void setIndex(quint8 index) { m_index = index; }
    QList<Field>* tx() { return &m_tx; }
    QList<Field>* rx() { return &m_rx; }

private:
    quint8 m_index;
    QString m_name;
    QList<Field> m_rx;
    QList<Field> m_tx;
};

class CommandModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CommandModel(QObject* parent = nullptr, ParcelModel* tx = nullptr);
    ~CommandModel() override;
    QList<Field>* txData(int row);
    QList<Field>* rxData(int row);
    void new_();
    void open(const QString& name);

    void save();
    void saveAs(const QString& name);

signals:
    void resetParcelModel();
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
    QList<Command> m_data;
    QString m_name;
    ParcelModel* m_tx;
};
