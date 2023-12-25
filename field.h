#ifndef FIELD_H
#define FIELD_H

#include <QDebug>
#include <QObject>
#include <array>
#include <variant>

constexpr QLatin1String operator""_ql1(const char* str, size_t size) {
    return QLatin1String{str, static_cast<int>(size - 1)};
}

class Field {
    using Variant = std::variant<
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        float, double /*, QByteArray*/>;

public:
    explicit Field(const QString& name = "Field");

    enum Type {
        I08,
        I16,
        I32,
        I64,
        U08,
        U16,
        U32,
        U64,
        Float,
        Double,
        ByteArray
    };

    template <typename T>
    T value() const {
        return std::visit([](auto&& arg) { return static_cast<T>(arg); }, value_);
    }

    QVariant qVariant() const;
    Variant variant() const;

    template <typename T>
    void setValue(const T& value) {
        if constexpr (std::is_pointer_v<T>) {
            return std::visit([value]<typename Ty>(Ty& arg) {
                memcpy(&arg, value, sizeof(Ty));
            },
                value_);

        } else if constexpr (std::is_same_v<T, QVariant>) {
            return std::visit([&value]<typename Ty>(Ty& arg) {
                arg = value.template value<Ty>();
            },
                value_);
        } else if constexpr (std::is_arithmetic_v<T>) {
            value_ = value;
        }
    }

    int size() const;

    Type type() const;

    void setType(Type type);

    QString name() const;
    void setName(const QString& value);

    QString typeName() const;
    static QStringList typeNames();

    QByteArray byteArray() const;
    QByteArray hex() const;

private:
    QString name_;
    Variant value_{uint8_t{}};
    static inline const QStringList typeNames_{
        "int8", "int16", "int32", "int64",
        "uint8", "uint16", "uint32", "uint64",
        "float", "double", "ByteArray"};
};

#endif // FIELD_H
