#ifndef FIELD_H
#define FIELD_H

#include <QObject>

class Field {
public:
    explicit Field(const QString& name = "Field");

    template <typename T>
    T value() const
    {
        switch (m_type) {
        case 0:
            return m_value.Int8;
        case 1:
            return m_value.Int16;
        case 2:
            return m_value.Int32;
        case 3:
            return m_value.Uint8;
        case 4:
            return m_value.Uint16;
        case 5:
            return m_value.Uint32;
        case 6:
            return m_value.Float;
        case 7:
            return m_value.Double;
        }
        return 0;
    }

    template <typename T>
    void setValue(const T& value)
    {
        switch (m_type) {
        case 0:
            m_value.Int8 = value;
            break;
        case 1:
            m_value.Int16 = value;
            break;
        case 2:
            m_value.Int32 = value;
            break;
        case 3:
            m_value.Uint8 = value;
            break;
        case 4:
            m_value.Uint16 = value;
            break;
        case 5:
            m_value.Uint32 = value;
            break;
            break;
        case 6:
            m_value.Float = value;
            break;
        case 7:
            m_value.Double = value;
            break;
        }
    }

    int size() const;

    int type() const;
    void setType(int type);

    QString name() const;
    void setName(const QString& value);

    QString typeName() const;
    QStringList typeNames() const;

    QByteArray data()
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

private:
    union {
        int8_t Int8;
        int16_t Int16;
        int32_t Int32;
        int64_t Int64;
        uint8_t Uint8;
        uint16_t Uint16;
        uint32_t Uint32;
        uint64_t Uint64;
        float Float;
        double Double;
    } m_value;

    int m_type = 3; //uint8_t
    QString m_name;
    static const QStringList m_typeNames;
};

#endif // FIELD_H
