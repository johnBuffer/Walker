#pragma once

#include <cstdint>


namespace nt {

    struct ID {
        uint32_t value;

        explicit
        ID(uint32_t id_)
            : value{id_}
        {}

        ID(const ID& other) = default;

        bool operator==(const ID &other) const {
            return value == other.value;
        }

        ID& operator=(const ID& other)
        {
            value = other.value;
            return *this;
        }

        explicit
        operator uint32_t() const {
            return value;
        }
    };

}