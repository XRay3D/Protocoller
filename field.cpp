#include "field.h"

const QStringList Field::m_typeNames{ "int8", "int16", "int32", "uint8", "uint16", "uint32", "float", "double" };

Field::Field(const QString& name)
    : m_name(name)
{
    m_value.Uint64 = 0;
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
