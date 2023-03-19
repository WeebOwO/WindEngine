#pragma once

#include <algorithm>
#include <functional>
#include <vector>

namespace wind {
template <typename T, typename U>
void RemoveNosupportedElems(std::vector<T>& elems, const std::vector<U>& supportedElems,
                            std::function<bool(const T&, const U&)> eq) {
    int i = 0;
    while (i < elems.size()) {
        if (std::find_if(supportedElems.begin(), supportedElems.end(), [&](const U& elem) {
                return eq(elems[i], elem);
            }) == supportedElems.end()) {
            elems.erase(elems.begin() + i);
        } else {
            i++;
        }
    }
}

} // namespace wind