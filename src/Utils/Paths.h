#pragma once
#include <cstdlib>
#include <string>

const auto envvar = getenv("QLP_DATA_PATH");
const static std::string ASSET_PATH = envvar != nullptr ? envvar : _ASSET_PATH;
