# RapidJSON

hunter_add_package(RapidJSON)
find_package(RapidJSON CONFIG REQUIRED)
add_definitions(-DRAPIDJSON_HAS_STDSTRING)