#pragma once
#include <logstat/errors.hpp>

#define RAPIDJSON_ASSERT(x) (!(x) ? throw logstat::ParseError("broken line") : (void)0u)

#include <rapidjson/document.h>
