#pragma once

#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string_view>

namespace io {  
    std::vector<char> readFile(std::string_view filename_view) {
        std::string filename = std::string(filename_view);
        std::ifstream file {filename, std::ios::ate | std::ios::binary};
        if(!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filename);
        }
        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(filesize);

        file.seekg(0);
        file.read(buffer.data(), filesize);
        file.close();
        return buffer;
    };

}