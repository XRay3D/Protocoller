#include "field.h"

Field::Field(const QString& name)
    : name_{name} { }

QVariant Field::qVariant() const {
    return std::visit([](auto&& arg) { return QVariant{arg}; }, value_);
}

Field::Variant Field::variant() const { return value_; }

int Field::size() const {
    return std::visit([](auto arg) { return +sizeof(arg); }, value_);
}

Field::Type Field::type() const { return static_cast<Type>(value_.index()); }

void Field::setType(Type type) {
    static constexpr std::array array{
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<int8_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<int16_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<int32_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<int64_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<uint8_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<uint16_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<uint32_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<uint64_t>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<float>(arg); }, f.value_); },
        +[](Field& f) { f.value_ = std::visit([](auto arg) { return static_cast<double>(arg); }, f.value_); },
    };
    array.at(type)(*this);
}

QString Field::name() const { return name_; }

void Field::setName(const QString& value) { name_ = value; }

QString Field::typeName() const { return typeNames_[value_.index()]; }

QStringList Field::typeNames() { return typeNames_; }

QByteArray Field::byteArray() const {
    return std::visit([]<typename T>(const T& arg) {
        char data[sizeof(T)]{};
        memcpy(data, &arg, sizeof(T)); // std::bit_cast<char[sizeof(T)]>(arg);
        return QByteArray{data, +sizeof(T)};
    },
        value_);
}

QByteArray Field::hex() const { return byteArray().toHex().toUpper(); }
