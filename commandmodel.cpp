#include "commandmodel.h"

CommandModel::CommandModel(QObject* parent, ParcelModel* tx)
    : QAbstractTableModel(parent)
    , m_tx(tx)

{
    open("data.txt");
}

CommandModel::~CommandModel()
{
}

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
    QFile file(m_name);
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
                var.setValue(int8_t(tmpTx[2].toInt()));
                break;
            case 1:
                var.setValue(int16_t(tmpTx[2].toInt()));
                break;
            case 2:
                var.setValue(int32_t(tmpTx[2].toInt()));
                break;
            case 3:
                var.setValue(int64_t(tmpTx[2].toLongLong()));
                break;
            case 4:
                var.setValue(uint8_t(tmpTx[2].toUInt()));
                break;
            case 5:
                var.setValue(uint16_t(tmpTx[2].toUInt()));
                break;
            case 6:
                var.setValue(uint32_t(tmpTx[2].toUInt()));
                break;
            case 7:
                var.setValue(uint64_t(tmpTx[2].toULongLong()));
                break;
            case 8:
                var.setValue(float(tmpTx[2].toFloat()));
                break;
            case 9:
                var.setValue(double(tmpTx[2].toDouble()));
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
                var.setValue(int8_t(tmpRx[2].toInt()));
                break;
            case 1:
                var.setValue(int16_t(tmpRx[2].toInt()));
                break;
            case 2:
                var.setValue(int32_t(tmpRx[2].toInt()));
                break;
            case 3:
                var.setValue(int64_t(tmpRx[2].toLongLong()));
                break;
            case 4:
                var.setValue(uint8_t(tmpRx[2].toUInt()));
                break;
            case 5:
                var.setValue(uint16_t(tmpRx[2].toUInt()));
                break;
            case 6:
                var.setValue(uint32_t(tmpRx[2].toUInt()));
                break;
            case 7:
                var.setValue(uint64_t(tmpRx[2].toULongLong()));
                break;
            case 8:
                var.setValue(float(tmpRx[2].toFloat()));
                break;
            case 9:
                var.setValue(double(tmpRx[2].toDouble()));
                break;
            }
        }
    }
}

void CommandModel::save()
{
    //        \a        bel          Звуковой сигнал
    //        \b        bs           Курсор на одну позицию назад
    //        \f        ff           Переход к началу (перевод формата)
    //        \n        lf           Переход на одну строку вниз(перевод строки)
    //        \r        cr           Возврат на первую позицию строки
    //        \t        ht           Переход к позиции, кратной 8 (табуляция)
    //        \v        vt           Вертикальная табуляция по строкам
    //        \\  \'  \”  \?         Представление символов \, ', ", ?
    //        \nn                    Символ с восьмеричным кодом nn
    //        \xnn                   Символ с шестнадцатеричным кодом nn
    //        \0                     Символ с кодом 0

    QByteArray data;
    for (int i = 0; i < m_data.size(); ++i) {
        data.append(m_data[i].name());
        data.append('\a');
        data.append(QString::number(m_data[i].index()));
        data.append('\t');
        for (int j = 0; j < m_data[i].tx()->size(); ++j) {
            const Field& var = m_data[i].tx()->at(j);
            data.append(var.name());
            data.append('\a');
            data.append(QString::number(var.type()));
            data.append('\a');
            switch (var.type()) {
            case 0:
                data.append(QString::number(var.value<int8_t>()));
                break;
            case 1:
                data.append(QString::number(var.value<int16_t>()));
                break;
            case 2:
                data.append(QString::number(var.value<int32_t>()));
                break;
            case 3:
                data.append(QString::number(var.value<int64_t>()));
                break;
            case 4:
                data.append(QString::number(var.value<uint8_t>()));
                break;
            case 5:
                data.append(QString::number(var.value<uint16_t>()));
                break;
            case 6:
                data.append(QString::number(var.value<uint32_t>()));
                break;
            case 7:
                data.append(QString::number(var.value<uint64_t>()));
                break;
            case 8:
                data.append(QString::number(var.value<float>(), 'g', 12));
                break;
            case 9:
                data.append(QString::number(var.value<double>(), 'g', 12));
                break;
            }
            if (j < m_data[i].tx()->size() - 1)
                data.append('\b');
        }
        data.append('\t');
        for (int j = 0; j < m_data[i].rx()->size(); ++j) {
            const Field& var = m_data[i].rx()->at(j);
            data.append(var.name());
            data.append('\a');
            data.append(QString::number(var.type()));
            data.append('\a');
            switch (var.type()) {
            case 0:
                data.append(QString::number(var.value<int8_t>()));
                break;
            case 1:
                data.append(QString::number(var.value<int16_t>()));
                break;
            case 2:
                data.append(QString::number(var.value<int32_t>()));
                break;
            case 3:
                data.append(QString::number(var.value<int64_t>()));
                break;
            case 4:
                data.append(QString::number(var.value<uint8_t>()));
                break;
            case 5:
                data.append(QString::number(var.value<uint16_t>()));
                break;
            case 6:
                data.append(QString::number(var.value<uint32_t>()));
                break;
            case 7:
                data.append(QString::number(var.value<uint64_t>()));
                break;
            case 8:
                data.append(QString::number(var.value<float>(), 'g', 12));
                break;
            case 9:
                data.append(QString::number(var.value<double>(), 'g', 12));
                break;
            }
            if (j < m_data[i].rx()->size() - 1)
                data.append('\b');
        }
        if (i < m_data.size() - 1)
            data.append('\f');
    }
    QFile file(m_name);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(reinterpret_cast<QWidget*>(parent()), "", m_name + ": " + file.errorString(), QMessageBox::Ok);
        return;
    }
    file.write(data);
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
