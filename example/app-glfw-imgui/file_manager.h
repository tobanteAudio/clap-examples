#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class FileManager
{
public:
    FileManager();
    ~FileManager() = default;
    static auto read(std::string const& filename) -> std::string;
};
