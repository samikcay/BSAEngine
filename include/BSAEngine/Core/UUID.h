#pragma once

#include <cstdint>

namespace BSA {

    class UUID {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };

} // namespace BSA

namespace std {
    template <typename T> struct hash;

    template<>
    struct hash<BSA::UUID> {
        size_t operator()(const BSA::UUID& uuid) const {
            return (uint64_t)uuid;
        }
    };
}
