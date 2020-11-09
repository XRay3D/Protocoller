#include "commandmodel.h"

CommandModel::CommandModel(QObject* parent, ParcelModel* tx)
    : QAbstractTableModel(parent)
    , m_tx(tx)
{
    open("data.bin");
}

CommandModel::~CommandModel() { }

QList<Field>* CommandModel::txData(int row) { return m_data[row].tx(); }

QList<Field>* CommandModel::rxData(int row) { return m_data[row].rx(); }

void CommandModel::new_()
{
    m_name.clear();
    beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
    m_data.clear();
    endRemoveRows();
}

void CommandModel::open(const QString& name)
{
    m_name = name;
    if (1) {
        QFile file(m_name);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", m_name + ": " + file.errorString(), QMessageBox::Ok);
            return;
        }
        QDataStream in(&file);
        in >> m_name;
        in >> m_data;
    } else {
        QFile file("data.txt");
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", m_name + ": " + file.errorString(), QMessageBox::Ok);
            return;
        }
        QByteArray data(file.readAll());
        if (data.isEmpty()) {
            QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", m_name + ": is empty", QMessageBox::Ok);
            return;
        }

        QList<QByteArray> commandList(data.split('\f'));
        for (int i = 0; i < commandList.size(); ++i) {
            QList<QByteArray> dataList(commandList[i].split('\t'));
            QList<QByteArray> tmp(dataList[0].split('\a'));

            m_data.append(Command(tmp[0], static_cast<quint8>(tmp[1].toInt()), false));

            tmp = dataList[1].split('\b');
            for (int j = 0; j < tmp.size(); ++j) {
                QList<QByteArray> tmpTx(tmp[j].split('\a'));
                m_data[i].tx()->append(Field(tmpTx[0]));

                Field& var = (*m_data[i].tx())[j];

                var.setType(tmpTx[1].toInt());

                switch (var.type()) {
                case 0:
                    var = int8_t(tmpTx[2].toInt());
                    break;
                case 1:
                    var = int16_t(tmpTx[2].toInt());
                    break;
                case 2:
                    var = int32_t(tmpTx[2].toInt());
                    break;
                case 3:
                    var = int64_t(tmpTx[2].toLongLong());
                    break;
                case 4:
                    var = uint8_t(tmpTx[2].toUInt());
                    break;
                case 5:
                    var = uint16_t(tmpTx[2].toUInt());
                    break;
                case 6:
                    var = uint32_t(tmpTx[2].toUInt());
                    break;
                case 7:
                    var = uint64_t(tmpTx[2].toULongLong());
                    break;
                case 8:
                    var = float(tmpTx[2].toFloat());
                    break;
                case 9:
                    var = double(tmpTx[2].toDouble());
                    break;
                }
            }

            tmp = dataList[2].split('\b');
            for (int j = 0; j < tmp.size(); ++j) {
                QList<QByteArray> tmpRx(tmp[j].split('\a'));
                m_data[i].rx()->append(Field(tmpRx[0]));

                Field& var = (*m_data[i].rx())[j];

                var.setType(tmpRx[1].toInt());

                switch (var.type()) {
                case 0:
                    var = int8_t(tmpRx[2].toInt());
                    break;
                case 1:
                    var = int16_t(tmpRx[2].toInt());
                    break;
                case 2:
                    var = int32_t(tmpRx[2].toInt());
                    break;
                case 3:
                    var = int64_t(tmpRx[2].toLongLong());
                    break;
                case 4:
                    var = uint8_t(tmpRx[2].toUInt());
                    break;
                case 5:
                    var = uint16_t(tmpRx[2].toUInt());
                    break;
                case 6:
                    var = uint32_t(tmpRx[2].toUInt());
                    break;
                case 7:
                    var = uint64_t(tmpRx[2].toULongLong());
                    break;
                case 8:
                    var = float(tmpRx[2].toFloat());
                    break;
                case 9:
                    var = double(tmpRx[2].toDouble());
                    break;
                }
            }
        }
    }
}

void CommandModel::save()
{
    QFile file(m_name);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << m_name;
    out << m_data;
}

void CommandModel::saveAs(const QString& name)
{
    m_name = name;
    save();
}

int CommandModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_data.size();
}

int CommandModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

QVariant CommandModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Command& var = m_data[index.row()];
        switch (index.column()) {
        case 0:
            return var.name();
        case 1:
            return var.index();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool CommandModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole) {
        Command& var = m_data[index.row()];
        switch (index.column()) {
        case 0:
            var.setName(value.toString());
            break;
        case 1:
            var.setIndex(static_cast<quint8>(value.toInt()));
            if (m_tx)
                m_tx->setCommand(var.index());
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

Qt::ItemFlags CommandModel::flags(const QModelIndex& /*index*/) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant CommandModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Имя");
            case 1:
                return QString("Индекс");
            }
        } else {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}

bool CommandModel::insertRows(int row, int /*count*/, const QModelIndex& parent)
{
    row = m_data.size();
    beginInsertRows(parent, row, row);
    m_data.insert(row, Command(QString("Command_%1").arg(row), static_cast<quint8>(row)));
    endInsertRows();
    return true;
}

bool CommandModel::removeRows(int row, int /*count*/, const QModelIndex& parent)
{
    emit resetParcelModel();
    beginRemoveRows(parent, row, row);
    m_data.removeAt(row);
    endRemoveRows();
    return true;
}
