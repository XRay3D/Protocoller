#include "parcelmodel.h"

#include <QColor>
#include <QDebug>
#include <QMessageBox>

ParcelModel::ParcelModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

ParcelModel::~ParcelModel() {
}

void ParcelModel::setData(QList<Field>* data) {
    if (data_) {
        beginRemoveRows({}, 0, data_->size() - 1);
        data_ = nullptr;
        endRemoveRows();
    }
    data_ = data;
    if (data_) {
        beginInsertRows({}, 0, data_->size() - 1);
        endInsertRows();
        updateCrc();
    }
}

void ParcelModel::setCommand(uint8_t command) {
    if (data_) {
        (*data_)[Address].setValue(command);
        dataChanged(createIndex(Address, ValueHex), createIndex(Address, ValueHex));
    }
}

const QByteArray& ParcelModel::parcel() {
    return parcel_;
}

void ParcelModel::setParcel(const QByteArray& data) {
    if (!data_)
        return;
    if (parcel_.size() != data.size())
        error(QString("Не соответствие размеров посылок:\n"
                      "в табличце %1 байт пришло %2!")
                  .arg(parcel_.size())
                  .arg(data.size()));

    const char* ptr = data.constData();

    QList<Field>& fields = (*data_);

    for (int i = 0; i < fields.size(); ++i) {
        fields[i].setValue(ptr);
        ptr += fields[i].size();
    }
    dataChanged(createIndex(Start1, ValueHex), createIndex(fields.size(), ValueHex));
}

void ParcelModel::setUseAddress(bool fl) {
    if (useAddress_ != fl) {
        useAddress_ ? removeRows(Address) : insertRows(Address);
        if (fl) (*data_)[Address].setName("Address");
        useAddress_ = fl;
    }
}

void ParcelModel::setAddress(int val) {
    if (useAddress_) {
        (*data_)[Address].setValue(val);
        dataChanged(index(Address, Name), index(Address, Value), {Qt::DisplayRole});
    }
}

int ParcelModel::rowCount(const QModelIndex&) const {
    if (data_ == nullptr) return {};
    return data_->size();
}

int ParcelModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant ParcelModel::data(const QModelIndex& index, int role) const {
    if (!data_) return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Field& var = data_->at(index.row());
        switch (index.column()) {
        case Name:
            return var.name();
        case Type:
            return var.typeName();
        case TypeSize:
            return var.size();
        case Value:
            return var.qVariant();
        case ValueHex:
            return var.hex();
        }
    }
    if (role == Qt::BackgroundRole) {
        if (index.row() == Start1 || index.row() == Start2)
            return QColor(255, 230, 230);
        else if (index.row() == Size)
            return QColor(230, 230, 255);
        else if (index.row() == Address /*&& useAddress_*/)
            return QColor(255, 255, 230);
        else if (index.row() == Command)
            return QColor(255, 255, 230);
        else if (index.row() == data_->size() - 1)
            return QColor(255, 230, 255);
        else // Crc
            return QColor(230, 255, 230);
    }
    if (role == Qt::UserRole)
        if (index.column() == 1) return data_->at(index.row()).type();
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

bool ParcelModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole) {
        Field& var = (*data_)[index.row()];
        switch (index.column()) {
        case Name:
            var.setName(value.toString());
            break;
        case Type:
            var.setType(static_cast<Field::Type>(value.toUInt()));
            break;
        case Value:
            var.setValue(value);
            break;
        default:
            return false;
        }
        updateCrc();
        return true;
    }
    return false;
}

Qt::ItemFlags ParcelModel::flags(const QModelIndex& index) const {
    if (index.column() == Size || (index.row() < 5 && (index.row() != Value && index.column() < Value)) || index.row() == data_->size() - 1)
        return Qt::ItemIsEnabled;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant ParcelModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static QStringList hd{"Имя", "Тип", "Размер", "Значение", "HEX"};
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return hd[section];
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool ParcelModel::insertRows(int row, int /*count*/, const QModelIndex& parent) {
    if (data_ == nullptr)
        return false;
    row = data_->size() - 1;
    beginInsertRows(parent, row, row);
    data_->insert(row, Field(QString("Field_%1").arg(row - 4)));
    endInsertRows();
    (*data_)[Size].setValue<uint8_t>(data_->size());
    QModelIndex index(createIndex(Size, Value));
    emit dataChanged(index, index, {Qt::EditRole});
    updateCrc();
    return true;
}

bool ParcelModel::removeRows(int row, int /*count*/, const QModelIndex& parent) {
    if (data_ == nullptr)
        return false;
    beginRemoveRows(parent, row, row);
    data_->removeAt(row);
    endRemoveRows();
    (*data_)[Size].setValue<uint8_t>(data_->size());
    QModelIndex index(createIndex(Size, Value));
    emit dataChanged(index, index, {Qt::EditRole});
    updateCrc();
    return true;
}

void ParcelModel::updateCrc() {
    parcel_.clear();
    if (data_) {
        auto& refData = (*data_);

        uint8_t size{};
        for (auto&& field: refData) size += field.size();

        refData[Size].setValue(size);
        QModelIndex index2(createIndex(Size, Value));
        emit dataChanged(index2, index2, {Qt::EditRole});

        for (auto&& field: refData) parcel_.append(field.byteArray());

        refData.last().setValue(CalcCrc(parcel_));
        QModelIndex index(createIndex(refData.size() - 1, Value));
        emit dataChanged(index, index, {Qt::EditRole});

        parcel_[parcel_.size() - 1] = refData.last().byteArray()[0];
    }
}
