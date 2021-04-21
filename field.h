#ifndef FIELD_H
#define FIELD_H

#include <QDebug>
#include <QObject>
#include <variant>

class Field {
public:
    explicit Field(const QString& name = "Field");

    template <typename T>
    T value() const { return *(T*)(&m_value); }

    QVariant value2() const;

    template <typename T>
    void setValue(const T& value) {
        *(T*)(&m_value) = value;
    }

    void setValue2(const char* ptr);

    void setValue3(const QVariant& value);

    int size() const;

    int type() const;
    void setType(int type);

    QString name() const;
    void setName(const QString& value);

    QString typeName() const;
    QStringList typeNames() const;

    QByteArray data();

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
    type_info ti;
    int m_type = 3; //uint8_t

    QString m_name;
    static const QStringList m_typeNames;
};

#endif // FIELD_H
