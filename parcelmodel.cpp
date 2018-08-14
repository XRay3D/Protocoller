#include "parcelmodel.h"

#include <QColor>
#include <QDebug>
#include <QMessageBox>

ParcelModel::ParcelModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

ParcelModel::~ParcelModel()
{
}

void ParcelModel::setData(QList<Field>* data)
{
    if (m_data) {
        beginRemoveRows(QModelIndex(), 0, m_data->size() - 1);
        m_data = nullptr;
        endRemoveRows();
    }
    m_data = data;
    if (m_data) {
        beginInsertRows(QModelIndex(), 0, m_data->size() - 1);
        endInsertRows();
        updateCrc();
    }
}

void ParcelModel::setCommand(uint8_t command)
{
    if (m_data) {
        (*m_data)[3].setValue(command);
        dataChanged(createIndex(3, 3), createIndex(3, 3));
    }
}

const QByteArray& ParcelModel::parcel()
{
    return m_parcel;
}

void ParcelModel::setParcel(const QByteArray& data)
{
    if (m_parcel.size() != data.size())
        error(QString("Не соответствие размеров посылок:\n"
                      "в табличце %1 байт\n"
                      "пришло %2")
                  .arg(m_parcel.size())
                  .arg(data.size()));

    QList<Field>& fl = (*m_data);
    const char* p = data.constData();
    for (int i = 0; i < fl.size(); ++i) {
        switch (fl[i].type()) {
        case 0:
            fl[i].setValue(*reinterpret_cast<const int8_t*>(p));
            p += sizeof(int8_t);
            break;
        case 1:
            fl[i].setValue(*reinterpret_cast<const int16_t*>(p));
            p += sizeof(int16_t);
            break;
        case 2:
            fl[i].setValue(*reinterpret_cast<const int32_t*>(p));
            p += sizeof(int32_t);
            break;
        case 3:
            fl[i].setValue(*reinterpret_cast<const uint8_t*>(p));
            p += sizeof(uint8_t);
            break;
        case 4:
            fl[i].setValue(*reinterpret_cast<const uint16_t*>(p));
            p += sizeof(uint16_t);
            break;
        case 5:
            fl[i].setValue(*reinterpret_cast<const uint32_t*>(p));
            p += sizeof(uint32_t);
            break;
        case 6:
            fl[i].setValue(*reinterpret_cast<const float*>(p));
            p += sizeof(float);
            break;
        case 7:
            fl[i].setValue(*reinterpret_cast<const double*>(p));
            p += sizeof(double);
            break;
        }
    }
    dataChanged(createIndex(0, 3), createIndex(fl.size(), 3));
}

int ParcelModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (m_data == nullptr)
        return 0;
    return m_data->size();
}

int ParcelModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4;
}

QVariant ParcelModel::data(const QModelIndex& index, int role) const
{

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (m_data == nullptr)
            return QVariant();
        else {
            const Field& var = m_data->at(index.row());
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
                    return var.value<uint8_t>();
                case 4:
                    return var.value<uint16_t>();
                case 5:
                    return var.value<uint32_t>();
                case 6:
                    return var.value<float>();
                case 7:
                    return var.value<double>();
                }
            }
        }
        return QVariant();
    case Qt::BackgroundColorRole:
        if (index.row() == 0 || index.row() == 1)
            return QColor(255, 230, 230);
        else if (index.row() == 2)
            return QColor(230, 230, 255);
        else if (index.row() == 3)
            return QColor(255, 255, 230);
        else if (index.row() == m_data->size() - 1)
            return QColor(255, 230, 255);
        else
            return QColor(230, 255, 230);
    case Qt::UserRole:
        if (index.column() == 1) {
            return m_data->at(index.row()).type();
        }
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
                var.setValue(uint8_t(value.toUInt()));
                break;
            case 4:
                var.setValue(uint16_t(value.toUInt()));
                break;
            case 5:
                var.setValue(uint32_t(value.toUInt()));
                break;
            case 6:
                var.setValue(float(value.toFloat()));
                break;
            case 7:
                var.setValue(double(value.toDouble()));
                break;
            default:
                return false;
            }
            break;
        default:
            return false;
        }
        updateCrc();
        return true;
    }
    return false;
}

Qt::ItemFlags ParcelModel::flags(const QModelIndex& index) const
{
    if (index.column() == 2 || index.row() == 2 || index.row() == 3 || index.row() == m_data->size() - 1)
        return Qt::ItemIsEnabled;
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
    m_data->insert(row, Field(QString("Field_%1").arg(row - 3)));
    endInsertRows();
    (*m_data)[2].setValue(m_data->size());
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
    (*m_data)[2].setValue(m_data->size());
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
        fl[2].setValue(size);
        QModelIndex index2(createIndex(2, 3));
        emit dataChanged(index2, index2, { Qt::EditRole });

        for (int i = 0; i < fl.size(); ++i) {
            m_parcel.append(fl[i].data());
        }
        uint8_t crc = CalcCrc(m_parcel);
        fl.last().setValue(crc);
        m_parcel[m_parcel.size() - 1] = fl.last().data()[0];
        QModelIndex index(createIndex(fl.size() - 1, 3));
        emit dataChanged(index, index, { Qt::EditRole });
    }
}
