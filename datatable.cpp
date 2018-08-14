#include "datatable.h"
#include "datatable.h"
#include "datatable.h"

DataTable::DataTable(QObject* parent)
    : QAbstractTableModel(parent)
    , m_data({ Var("Start1"), Var("Start2"), Var("Size"), Var("Command"), Var("CRC") })
{
}

int DataTable::rowCount(const QModelIndex& parent) const
{
    return m_data.size();
}

int DataTable::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QVariant DataTable::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        const Var& var = m_data[index.row()];
        switch (index.column()) {
        case 0:
            return var.name();
        case 1:
            return var.typeName();
        case 2:
            return var.size();
        case 3:
            switch (var.type()) {
            case 0:
                return var.value<int8_t>();
            case 1:
                return var.value<int16_t>();
            case 2:
                return var.value<int32_t>();
            case 3:
                return var.value<int64_t>();
            case 4:
                return var.value<uint8_t>();
            case 5:
                return var.value<uint16_t>();
            case 6:
                return var.value<uint32_t>();
            case 7:
                return var.value<uint64_t>();
            case 8:
                return var.value<float>();
            case 9:
                return var.value<double>();
            default:
                return QVariant();
            }
        default:
            return QVariant();
        }
    }
    else if (role == Qt::UserRole && index.column() == 1) {
        return m_data[index.row()].type();
    }
    return QVariant();
}

bool DataTable::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (role == Qt::EditRole) {
        Var& var = m_data[index.row()];
        switch (index.column()) {
        case 0:
            var.setName(value.toString());
            break;
        case 1:
            var.setType(value.toInt());
            break;
        case 2: //return QString("Размер");
            break;
        case 3: //return QString("Значение");
            switch (var.type()) {
            case 0:
                var.setValue(int8_t(value.toInt()));
                break;
            case 1:
                var.setValue(int16_t(value.toInt()));
                break;
            case 2:
                var.setValue(int32_t(value.toInt()));
                break;
            case 3:
                var.setValue(int64_t(value.toLongLong()));
                break;
            case 4:
                var.setValue(uint8_t(value.toUInt()));
                break;
            case 5:
                var.setValue(uint16_t(value.toUInt()));
                break;
            case 6:
                var.setValue(uint32_t(value.toUInt()));
                break;
            case 7:
                var.setValue(uint64_t(value.toULongLong()));
                break;
            case 8:
                var.setValue(float(value.toFloat()));
                break;
            case 9:
                var.setValue(double(value.toDouble()));
                break;
            default:
                return false;
            }
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

Qt::ItemFlags DataTable::flags(const QModelIndex& index) const
{
    if (index.column() == 2)
        return Qt::ItemIsEnabled;
    if (index.row() == 0 || index.row() == 1 || index.row() == 2 || index.row() == 3 || index.row() == m_data.size() - 1)
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant DataTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Имя");
            case 1:
                return QString("Тип");
            case 2:
                return QString("Размер");
            case 3:
                return QString("Значение");
            }
        }
        else {
            return QString::number(section);
        }
    return QVariant();
}

bool DataTable::insertRows(int row, int count, const QModelIndex& parent)
{
    row = m_data.size() - 1;
    beginInsertRows(parent, row, row);
    m_data.insert(row, Var());
    endInsertRows();
    return true;
}
