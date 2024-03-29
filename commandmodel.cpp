#include "commandmodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

CommandModel::CommandModel(QObject* parent, ParcelModel* tx)
    : QAbstractTableModel(parent)
    , tx_(tx)

{
    open("data.json");
}

CommandModel::~CommandModel() { save(); }

void CommandModel::new_() {
    name_.clear();
    beginRemoveRows(QModelIndex(), 0, data_.size() - 1);
    data_.clear();
    endRemoveRows();
}

void CommandModel::open(const QString& name) {

    name_ = name;
    QFile file(name_);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", name_ + ": " + file.errorString(), QMessageBox::Ok);
        return;
    }

    QByteArray data(file.readAll());
    if (data.isEmpty()) {
        QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", name_ + ": is empty", QMessageBox::Ok);
        return;
    }

    auto array = QJsonDocument::fromJson(data).array();
    for (const QJsonValue& value: array) {
        auto object = value.toObject();
        data_.append(Command(object["name"].toString(), static_cast<quint8>(object["index"].toInt()), false));
        auto load = [](const QJsonArray& array, QList<Field>& fields) {
            for (const QJsonValue& value: array) {
                auto object = value.toObject();
                fields.append(Field{object["name"].toString()});
                // fields.back().setType(Field::Type(object["type"].toInt()));
                fields.back().setType(Field::Type(Field::typeNames().indexOf(object["type"].toString())));
                fields.back().setValue(object["value"].toVariant());
            }
        };

        load(object["rx"].toArray(), data_.back().rx());
        load(object["tx"].toArray(), data_.back().tx());
    }
}

void CommandModel::save() {

    QJsonArray arr;
    for (Command& cmd: data_) {
        QJsonArray rxArray;
        QJsonArray txArray;
        auto save = [](QJsonArray& array, const QList<Field>& fields) {
            for (auto&& field: fields) {
                array.append(QJsonObject{
                    {
                     {"name", field.name()},
                     {"type", field.typeName()},
                     {"value", QJsonValue::fromVariant(field.qVariant())},
                     }
                });
            }
        };
        save(rxArray, cmd.rx());
        save(txArray, cmd.tx());
        QJsonObject obj{
            {
             {"name", cmd.name()},
             {"index", cmd.index()},
             {"rx", rxArray},
             {"tx", txArray},
             }
        };
        arr.append(obj);
    }

    QFile file(name_);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", name_ + ": " + file.errorString(), QMessageBox::Ok);
        return;
    }
    file.write(QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

void CommandModel::saveAs(const QString& name) {
    name_ = name;
    save();
}

int CommandModel::rowCount(const QModelIndex&) const { return data_.size(); }

int CommandModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant CommandModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Command& var = data_[index.row()];
        switch (index.column()) {
        case Name:
            return var.name();
        case Index:
            return var.index();
        default:
            return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return QVariant();
}

bool CommandModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole) {
        Command& var = data_[index.row()];
        switch (index.column()) {
        case Name:
            var.setName(value.toString());
            return true;
        case Index:
            var.setIndex(static_cast<quint8>(value.toInt()));
            if (tx_)
                tx_->setCommand(var.index());
            return true;
        }
    }
    return {};
}

Qt::ItemFlags CommandModel::flags(const QModelIndex& /*index*/) const {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant CommandModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return section ? "Имя" : "Индекс";
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool CommandModel::insertRows(int row, int /*count*/, const QModelIndex& parent) {
    row = data_.size();
    beginInsertRows(parent, row, row);
    data_.insert(row, Command(QString("Command_%1").arg(row), static_cast<quint8>(row)));
    endInsertRows();
    return true;
}

bool CommandModel::removeRows(int row, int /*count*/, const QModelIndex& parent) {
    emit resetParcelModel();
    beginRemoveRows(parent, row, row);
    data_.removeAt(row);
    endRemoveRows();
    return true;
}
