#pragma once

#include <functional>
#include <span>
#include <vector>

namespace wind::utils {
template <typename T> constexpr auto MakeView(T&& v) {
    using ValueType = typename std::decay_t<T>::value_type;
    using Ret =
        std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, const ValueType, ValueType>;
    return std::span<Ret>{v.data(), v.size()};
}

template <typename StartFunc, typename EndFunc> struct ScopeGuard {
    ScopeGuard() {
        StartFunc();
    }
    ~ScopeGuard() {
        EndFunc();
    };
private:
    StartFunc funcStart;
    EndFunc funcEnd;
};
} // namespace wind::utils