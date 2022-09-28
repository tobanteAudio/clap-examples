#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


class FileManager
{
public:
    FileManager();
    ~FileManager();
    static std::string read(std::string const& filename);
};
