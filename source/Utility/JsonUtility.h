#pragma once

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

typedef rapidjson::Writer<rapidjson::StringBuffer> SerializeObj;
typedef rapidjson::Value::Object DeserializeObj;