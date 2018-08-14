#include "field.h"

const QStringList Field::m_typeNames{ "int8", "int16", "int32", "uint8", "uint16", "uint32", "float", "double" };

Field::Field(const QString& name)
    : m_name(name)
{
    m_value.Uint64 = 0;
}

QVariant Field::value2() const
{
    switch (m_type) {
    case 0:
        return QVariant(*(int8_t*)(&m_value));
    case 1:
        return QVariant(*(int16_t*)(&m_value));
    case 2:
        return QVariant(*(int32_t*)(&m_value));
    case 3:
        return QVariant(*(uint8_t*)(&m_value));
    case 4:
        return QVariant(*(uint16_t*)(&m_value));
    case 5:
        return QVariant(*(uint32_t*)(&m_value));
    case 6:
        return QVariant(*(float*)(&m_value));
    case 7:
        return QVariant(*(double*)(&m_value));
    }
}

void Field::setValue2(const char *ptr)
{
    switch (m_type) {
    case 0:
        *(int8_t*)(&m_value) = *(int8_t*)ptr;
        break;
    case 1:
        *(int16_t*)(&m_value) = *(int16_t*)ptr;
        break;
    case 2:
        *(int32_t*)(&m_value) = *(int32_t*)ptr;
        break;
    case 3:
        *(uint8_t*)(&m_value) = *(uint8_t*)ptr;
        break;
    case 4:
        *(uint16_t*)(&m_value) = *(uint16_t*)ptr;
        break;
    case 5:
        *(uint32_t*)(&m_value) = *(uint32_t*)ptr;
        break;
    case 6:
        *(float*)(&m_value) = *(float*)ptr;
        break;
    case 7:
        *(double*)(&m_value) = *(double*)ptr;
        break;
    }
}

void Field::setValue3(const QVariant &value)
{
    switch (m_type) {
    case 0:
        *(int8_t*)(&m_value) = value.toInt();
        break;
    case 1:
        *(int16_t*)(&m_value) = value.toInt();
        break;
    case 2:
        *(int32_t*)(&m_value) = value.toInt();
        break;
    case 3:
        *(uint8_t*)(&m_value) = value.toUInt();
        break;
    case 4:
        *(uint16_t*)(&m_value) = value.toUInt();
        break;
    case 5:
        *(uint32_t*)(&m_value) = value.toUInt();
        break;
    case 6:
        *(float*)(&m_value) = value.toFloat();
        break;
    case 7:
        *(double*)(&m_value) = value.toDouble();
        break;
    }
}

int Field::size() const
{
    switch (m_type) {
    case 0:
        return sizeof(int8_t);
    case 1:
        return sizeof(int16_t);
    case 2:
        return sizeof(int32_t);
    case 3:
        return sizeof(uint8_t);
    case 4:
        return sizeof(uint16_t);
    case 5:
        return sizeof(uint32_t);
    case 6:
        return sizeof(float);
    case 7:
        return sizeof(double);
    default:
        return -1;
    }
}

int Field::type() const { return m_type; }

void Field::setType(int type) { m_type = type; }

QString Field::name() const { return m_name; }

void Field::setName(const QString& value) { m_name = value; }

QString Field::typeName() const { return m_typeNames[m_type]; }

QStringList Field::typeNames() const { return m_typeNames; }

QByteArray Field::data()
{
    const char* const p = reinterpret_cast<char*>(&m_value);
    switch (m_type) {
    case 0:
        return QByteArray(p, sizeof(int8_t));
    case 1:
        return QByteArray(p, sizeof(int16_t));
    case 2:
        return QByteArray(p, sizeof(int32_t));
    case 3:
        return QByteArray(p, sizeof(uint8_t));
    case 4:
        return QByteArray(p, sizeof(uint16_t));
    case 5:
        return QByteArray(p, sizeof(uint32_t));
    case 6:
        return QByteArray(p, sizeof(float));
    case 7:
        return QByteArray(p, sizeof(double));
    }
    return QByteArray();
}
