#include "field.h"

Field::Field(const QString& name)
    : m_value(int8_t(0))
    , m_name(name)
{
}

int Field::size() const
{
    return std::visit([](auto arg) -> int { return sizeof(arg); }, m_value);
}

int Field::type() const
{
    return m_value.index();
}

void Field::setType(int type)
{
    m_value = std::visit([type](auto arg) -> variant {
        switch (type) {
        case INT_8:
            return int8_t(arg);
        case INT16:
            return int16_t(arg);
        case INT32:
            return int32_t(arg);
        case INT64:
            return int64_t(arg);
        case UINT_8:
            return uint8_t(arg);
        case UINT16:
            return uint16_t(arg);
        case UINT32:
            return uint32_t(arg);
        case UINT64:
            return uint64_t(arg);
        case FLOAT:
            return float(arg);
        case DOUBLE:
            return double(arg);
        default:
            assert("non-exhaustive visitor!");
            return uint8_t(0);
        }
    },
        m_value);
}

QString Field::string() const
{
    return std::visit([](auto arg) -> QString {
        return QString("%1 '%2'")
            .arg(arg)
            .arg(QString::fromLocal8Bit(reinterpret_cast<const char*>(&arg), sizeof(arg)));
    },
        m_value);
}

QString Field::name() const
{
    return m_name;
}

void Field::setName(const QString& value)
{
    m_name = value;
}

QString Field::typeName() const
{
    return m_typeNames[m_value.index()];
}

QVector<QString> Field::typeNames() const
{
    return m_typeNames;
}

QByteArray Field::data() const
{
    QByteArray rv;
    std::visit([&rv](auto arg) {
        using T = std::decay_t<decltype(arg)>;
        const char* const p = reinterpret_cast<char*>(&arg);
        rv = QByteArray(p, sizeof(T));
    },
        m_value);
    return rv;
}
