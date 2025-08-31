#include "algorithms/fd/fdhits/edgemark.h"

namespace algos::fdhits {

Edgemark::Edgemark(size_type n) : bits_(n) {}

void Edgemark::Set() {
    bits_.set();
}

bool Edgemark::None() const {
    return bits_.none();
}

bool Edgemark::Any() const {
    return bits_.any();
}

Edgemark::size_type Edgemark::Size() const {
    return bits_.size();
}

bool Edgemark::operator[](size_type pos) const {
    return bits_[pos];
}

Edgemark::reference Edgemark::operator[](size_type pos) {
    return bits_[pos];
}

Edgemark& Edgemark::operator&=(Edgemark const& other) {
    bits_ &= other.bits_;
    return *this;
}

Edgemark& Edgemark::operator|=(Edgemark const& other) {
    bits_ |= other.bits_;
    return *this;
}

Edgemark& Edgemark::operator^=(Edgemark const& other) {
    bits_ ^= other.bits_;
    return *this;
}

Edgemark Edgemark::operator~() const {
    Edgemark result(Size());
    result.bits_ = ~bits_;
    return result;
}

Edgemark operator&(Edgemark const& lhs, Edgemark const& rhs) {
    Edgemark result = lhs;
    result &= rhs;
    return result;
}

Edgemark operator|(Edgemark const& lhs, Edgemark const& rhs) {
    Edgemark result = lhs;
    result |= rhs;
    return result;
}

Edgemark operator^(Edgemark const& lhs, Edgemark const& rhs) {
    Edgemark result = lhs;
    result ^= rhs;
    return result;
}

bool operator==(Edgemark const& lhs, Edgemark const& rhs) {
    // Сравниваем по размеру и установленным битам
    if (lhs.Size() != rhs.Size()) {
        return false;
    }
    
    for (size_t i = 0; i < lhs.Size(); i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    
    return true;
}

bool operator!=(Edgemark const& lhs, Edgemark const& rhs) {
    return !(lhs == rhs);
}

std::vector<Edgemark::size_type> Edgemark::Ones() const {
    std::vector<size_type> result;
    for (size_type i = 0; i < Size(); i++) {
        if (bits_[i]) {
            result.push_back(i);
        }
    }
    return result;
}

} // namespace algos::fdhits
