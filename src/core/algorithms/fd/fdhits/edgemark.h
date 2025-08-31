#pragma once

#include <vector>
#include <boost/dynamic_bitset.hpp>

namespace algos::fdhits {

class Edgemark {
public:
    typedef boost::dynamic_bitset<>::size_type size_type;
    typedef boost::dynamic_bitset<>::reference reference;

private:
    boost::dynamic_bitset<> bits_;

public:
    explicit Edgemark(size_type n);
    
    void Set();
    bool None() const;
    bool Any() const;
    size_type Size() const;
    
    bool operator[](size_type pos) const;
    reference operator[](size_type pos);
    
    Edgemark& operator&=(Edgemark const& other);
    Edgemark& operator|=(Edgemark const& other);
    Edgemark& operator^=(Edgemark const& other);
    Edgemark operator~() const;
    
    std::vector<size_type> Ones() const;
};

Edgemark operator&(Edgemark const& lhs, Edgemark const& rhs);
Edgemark operator|(Edgemark const& lhs, Edgemark const& rhs);
Edgemark operator^(Edgemark const& lhs, Edgemark const& rhs);

bool operator==(Edgemark const& lhs, Edgemark const& rhs);
bool operator!=(Edgemark const& lhs, Edgemark const& rhs);

} // namespace algos::fdhits
