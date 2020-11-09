#pragma once

#include <QDataStream>
#include <QDebug>
#include <QObject>
#include <variant>

template <class>
inline constexpr bool always_false_v = false;

class Field {
    friend QDataStream& operator<<(QDataStream& ds, const Field& f)
    {
        ds << f.m_name;
        ds << f.m_value.index();

        std::visit([&ds](auto arg) { ds << arg; }, f.m_value);

        return ds;
    }
    friend QDataStream& operator>>(QDataStream& ds, Field& f)
    {
        size_t index;

        ds >> f.m_name;
        ds >> index;

        f.setType(int(index));
        std::visit([&ds](auto& arg) { ds >> arg; }, f.m_value);

        return ds;
    }

    using variant = std::variant<
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        uint8_t,
        uint16_t,
        uint32_t,
        uint64_t,
        float,
        double>;

public:
    explicit Field(const QString& name = "Field");

    template <typename CastType>
    operator CastType() const
    {
        //qDebug(__FUNCSIG__);
        CastType qVar;
        std::visit([&qVar](auto arg) { qVar = arg; }, m_value);
        return qVar;
    }

    template <typename ValType>
    Field& operator=(ValType value)
    {
        //qDebug(__FUNCSIG__);
        if constexpr (std::is_pointer_v<ValType>) { // const char *
            std::visit([value](auto& arg) { using T = std::decay_t<decltype(arg)>; arg = *reinterpret_cast<const T*>(value); }, m_value);
        } else if constexpr (std::is_same_v<ValType, QVariant>) {
            std::visit([value](auto& arg) { using T = std::decay_t<decltype(arg)>; arg = value.template value<T>(); }, m_value);
        } else {
            std::visit([value](auto& arg) { arg = value; }, m_value);
        }
        return *this;
    }

    const variant& value() const { return m_value; }

    int size() const;

    int type() const;
    void setType(int type);

    QString string() const;

    QString name() const;
    void setName(const QString& value);

    QString typeName() const;
    QVector<QString> typeNames() const;

    QByteArray data() const;

    enum Type {
        INT_8,
        INT16,
        INT32,
        INT64,
        UINT_8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT,
        DOUBLE
    };

private:
    variant m_value;
    inline static const QVector<QString> m_typeNames {
        "INT 8",
        "INT 16",
        "INT 32",
        "INT 64",
        "UINT 8",
        "UINT 16",
        "UINT 32",
        "UINT 64",
        "FLOAT",
        "DOUBLE"
    };
    QString m_name;
};

Q_DECLARE_METATYPE(Field)
