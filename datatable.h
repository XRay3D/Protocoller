#ifndef DATATABLE_H
#define DATATABLE_H

#include <QAbstractTableModel>

class Var {
public:
    explicit Var(const QString& name = "")
        : m_name(name)
        , m_typeNames({ "int8", "int16", "int32", "int64",
              "uint8", "uint16", "uint32", "uint64",
              "float", "double" })

    {
        m_value.Uint64 = 0;
    }

    template <typename T>
    T value() const
    {
        switch (m_type) {
        case 0:
            return m_value.Int8;
        case 1:
            return m_value.Int16;
        case 2:
            return m_value.Int32;
        case 3:
            return m_value.Int64;
        case 4:
            return m_value.Uint8;
        case 5:
            return m_value.Uint16;
        case 6:
            return m_value.Uint32;
        case 7:
            return m_value.Uint64;
        case 8:
            return m_value.Float;
        case 9:
            return m_value.Double;
        default:
            return -9999;
        }
    }

    template <typename T>
    void setValue(const T& value)
    {
        switch (m_type) {
        case 0:
            m_value.Int8 = value;
            break;
        case 1:
            m_value.Int16 = value;
            break;
        case 2:
            m_value.Int32 = value;
            break;
        case 3:
            m_value.Int64 = value;
            break;
        case 4:
            m_value.Uint8 = value;
            break;
        case 5:
            m_value.Uint16 = value;
            break;
        case 6:
            m_value.Uint32 = value;
            break;
        case 7:
            m_value.Uint64 = value;
            break;
        case 8:
            m_value.Float = value;
            break;
        case 9:
            m_value.Double = value;
            break;
        }
    }

    int size() const
    {
        switch (m_type) {
        case 0:
            return sizeof(int8_t);
        case 1:
            return sizeof(int16_t);
        case 2:
            return sizeof(int32_t);
        case 3:
            return sizeof(int64_t);
        case 4:
            return sizeof(uint8_t);
        case 5:
            return sizeof(uint16_t);
        case 6:
            return sizeof(uint32_t);
        case 7:
            return sizeof(uint64_t);
        case 8:
            return sizeof(float);
        case 9:
            return sizeof(double);
        default:
            return -1;
        }
    }

    int type() const { return m_type; }
    void setType(int type) { m_type = type; }

    QString name() const { return m_name; }
    void setName(const QString& value) { m_name = value; }

    QString typeName() const { return m_typeNames[m_type]; }
    QStringList typeNames() const { return m_typeNames; }

private:
    union {
        int8_t Int8;
        int16_t Int16;
        int32_t Int32;
        int64_t Int64;
        uint8_t Uint8;
        uint16_t Uint16;
        uint32_t Uint32;
        uint64_t Uint64;
        float Float;
        double Double;
    } m_value;

    int m_type = 0;
    QString m_name;
    const QStringList m_typeNames;
};

class DataTable : public QAbstractTableModel {
public:
    DataTable(QObject* parent = Q_NULLPTR);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;

private:
    QList<Var> m_data;
};

#endif // DATATABLE_H
