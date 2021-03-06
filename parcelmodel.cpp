#include "parcelmodel.h"

#include <QColor>
#include <QDebug>
#include <QMessageBox>

const int id1 = qRegisterMetaType<Field>("Field");

ParcelModel::ParcelModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

ParcelModel::~ParcelModel() { }

void ParcelModel::setData(QList<Field>* data)
{
    if (m_data) {
        beginRemoveRows({}, 0, m_data->size() - 1);
        m_data = nullptr;
        endRemoveRows();
    }
    m_data = data;
    if (m_data) {
        beginInsertRows({}, 0, m_data->size() - 1);
        endInsertRows();
        updateCrc();
    }
}

void ParcelModel::setCommand(uint8_t command)
{
    if (m_data) {
        (*m_data)[3] = command;
        dataChanged(createIndex(3, 3), createIndex(3, 3));
    }
}

const QByteArray& ParcelModel::parcel()
{
    return m_parcel;
}

void ParcelModel::setParcel(const QByteArray& data)
{
    if (!m_data)
        return;
    if (m_parcel.size() != data.size())
        error(QString("Не соответствие размеров посылок:\n"
                      "в табличце %1 байт пришло %2!")
                  .arg(m_parcel.size())
                  .arg(data.size()));

    const char* ptr = data.constData();

    QList<Field>& fields = (*m_data);

    for (int i = 0; i < fields.size(); ++i) {
        fields[i] = ptr;
        ptr += fields[i].size();
    }
    dataChanged(createIndex(0, 3), createIndex(fields.size(), 3));
}

int ParcelModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (m_data == nullptr)
        return 0;
    return m_data->size();
}

int ParcelModel::columnCount(const QModelIndex& /*parent*/) const { return 4; }

QVariant ParcelModel::data(const QModelIndex& index, int role) const
{

    switch (role) {
    case Qt::DisplayRole:
        if (m_data) {
            const Field& var = m_data->at(index.row());
            switch (index.column()) {
            case 0:
                return var.name();
            case 1:
                return var.typeName();
            case 2:
                return var.size();
            case 3:
                if (var.type() == Field::INT_8 || var.type() == Field::UINT_8) {
                    return var.string();
                } else {
                    return var;
                }
            }
        }
        return QVariant();
    case Qt::EditRole:
        if (m_data) {
            const Field& var = m_data->at(index.row());
            switch (index.column()) {
            case 0:
                return var.name();
            case 1:
                return var.typeName();
            case 2:
                return var.size();
            case 3:
                return var;
            }
        }
        return QVariant();
    case Qt::BackgroundRole:
        if (index.row() == 0 || index.row() == 1)
            return QColor(255, 230, 230);
        else if (index.row() == 2)
            return QColor(230, 230, 255);
        else if (index.row() == 3)
            return QColor(255, 255, 230);
        else if (index.row() == 4)
            return QColor(255, 255, 230);
        else if (index.row() == m_data->size() - 1)
            return QColor(255, 230, 255);
        else
            return QColor(230, 255, 230);
    case Qt::UserRole:
        return QVariant::fromValue(m_data->at(index.row()));
    }
    return QVariant();
}

bool ParcelModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole) {
        Field& var = (*m_data)[index.row()];
        switch (index.column()) {
        case 0:
            var.setName(value.toString());
            break;
        case 1:
            var.setType(value.toInt());
            break;
        case 3:
            var = value;
            break;
        default:
            return false;
        }
        updateCrc();
        return true;
    }
    return false;
}

Qt::ItemFlags ParcelModel::flags(const QModelIndex& /*index*/) const
{
    //    if (index.column() == 2 || (index.row() < 5 && (index.row() != 3 && index.column() < 3)) || index.row() == m_data->size() - 1)
    //        return Qt::ItemIsEnabled;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant ParcelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
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
        } else {
            return QString::number(section);
        }
    }
    return QVariant();
}

bool ParcelModel::insertRows(int row, int /*count*/, const QModelIndex& parent)
{
    if (m_data == nullptr)
        return false;
    row = m_data->size() - 1;
    beginInsertRows(parent, row, row);
    m_data->insert(row, Field(QString("Field_%1").arg(row - 4)));
    endInsertRows();
    (*m_data)[2] = uint8_t(m_data->size());
    QModelIndex index(createIndex(2, 3));
    emit dataChanged(index, index, { Qt::EditRole });
    updateCrc();
    return true;
}

bool ParcelModel::removeRows(int row, int /*count*/, const QModelIndex& parent)
{
    if (m_data == nullptr)
        return false;
    beginRemoveRows(parent, row, row);
    m_data->removeAt(row);
    endRemoveRows();
    (*m_data)[2] = uint8_t(m_data->size());
    QModelIndex index(createIndex(2, 3));
    emit dataChanged(index, index, { Qt::EditRole });
    updateCrc();
    return true;
}

void ParcelModel::updateCrc()
{
    m_parcel.clear();
    if (m_data) {
        QList<Field>& fl = (*m_data);

        uint8_t size = 0;
        for (int i = 0; i < fl.size(); ++i)
            size += fl[i].size();
        fl[2] = size;
        QModelIndex index2(createIndex(2, 3));
        emit dataChanged(index2, index2, { Qt::EditRole });

        for (int i = 0; i < fl.size(); ++i) {
            m_parcel.append(fl[i].data());
        }
        uint8_t crc = CalcCrc(m_parcel);
        fl.last() = crc;
        if (const auto& data = fl.last().data(); !data.isEmpty())
            m_parcel[m_parcel.size() - 1] = data[0];
        QModelIndex index(createIndex(fl.size() - 1, 3));
        emit dataChanged(index, index, { Qt::EditRole });
    }
}
