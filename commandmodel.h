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
        : m_name(name)
        , m_index(index) //
    {
        if(newCommand) {
            m_tx = {Field("Start1"), Field("Start2"), Field("Size"), /*Field("Addres"),*/ Field("Command"), Field("CRC")};
            m_rx = {Field("Start1"), Field("Start2"), Field("Size"), /*Field("Addres"),*/ Field("Command"), Field("CRC")};
            m_tx[0].setValue<uint8_t>(0x55);
            m_tx[1].setValue<uint8_t>(0xAA);
            m_tx[2].setValue<uint8_t>(m_tx.size());
            m_tx[3].setValue<uint8_t>(0);
            m_tx[4].setValue<uint8_t>(m_index);
        }
    }
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    quint8 index() const { return m_index; }
    void setIndex(quint8 index) { m_index = index; }

    QList<Field>& tx() { return m_tx; }
    QList<Field>& rx() { return m_rx; }
    const QList<Field>& tx() const { return m_tx; }
    const QList<Field>& rx() const { return m_rx; }

private:
    QList<Field> m_tx;
    QList<Field> m_rx;

    QString m_name;
    quint8 m_index;
};

class CommandModel : public QAbstractTableModel {
    Q_OBJECT
    QList<Command> m_data;
    QString m_name;
    ParcelModel* m_tx;

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

    QList<Field>& txData(int row) { return m_data[row].tx(); }
    QList<Field>& rxData(int row) { return m_data[row].rx(); }
    const QList<Field>& txData(int row) const { return m_data[row].tx(); }
    const QList<Field>& rxData(int row) const { return m_data[row].rx(); }

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
