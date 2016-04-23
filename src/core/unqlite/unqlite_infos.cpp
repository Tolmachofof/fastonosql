/*  Copyright (C) 2014-2016 FastoGT. All right reserved.

    This file is part of FastoNoSQL.

    FastoNoSQL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoNoSQL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoNoSQL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "core/unqlite/unqlite_infos.h"

#include <ostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

#define MARKER "\r\n"

namespace fastonosql {
namespace core {
namespace {

const std::vector<Field> unqliteCommonFields = {
  Field(UNQLITE_FILE_NAME_LABEL, common::Value::TYPE_STRING)
};

}  // namespace

template<>
std::vector<common::Value::Type> DBTraits<UNQLITE>::supportedTypes() {
  return  {
            common::Value::TYPE_BOOLEAN,
            common::Value::TYPE_INTEGER,
            common::Value::TYPE_UINTEGER,
            common::Value::TYPE_DOUBLE,
            common::Value::TYPE_STRING,
            common::Value::TYPE_ARRAY
          };
}

template<>
std::vector<std::string> DBTraits<UNQLITE>::infoHeaders() {
  return { UNQLITE_STATS_LABEL };
}

template<>
std::vector<std::vector<Field> > DBTraits<UNQLITE>::infoFields() {
  return { unqliteCommonFields };
}

namespace unqlite {

UnqliteServerInfo::Stats::Stats()
  : file_name() {
}

UnqliteServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos-start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == UNQLITE_FILE_NAME_LABEL) {
      file_name = value;
    }
    start = pos + 2;
  }
}

common::Value* UnqliteServerInfo::Stats::valueByIndex(unsigned char index) const {
  switch (index) {
  case 0:
    return new common::StringValue(file_name);
  default:
    NOTREACHED();
    break;
  }
  return nullptr;
}

UnqliteServerInfo::UnqliteServerInfo()
  : IServerInfo(UNQLITE) {
}

UnqliteServerInfo::UnqliteServerInfo(const Stats &stats)
  : IServerInfo(UNQLITE), stats_(stats) {
}

common::Value* UnqliteServerInfo::valueByIndexes(unsigned char property,
                                                 unsigned char field) const {
  switch (property) {
  case 0:
    return stats_.valueByIndex(field);
  default:
    NOTREACHED();
    break;
  }
  return nullptr;
}

std::ostream& operator<<(std::ostream& out, const UnqliteServerInfo::Stats& value) {
  return out << UNQLITE_FILE_NAME_LABEL":" << value.file_name << MARKER;
}

std::ostream& operator<<(std::ostream& out, const UnqliteServerInfo& value) {
  return out << value.toString();
}

UnqliteServerInfo* makeUnqliteServerInfo(const std::string& content) {
  if (content.empty()) {
      return nullptr;
  }

  UnqliteServerInfo* result = new UnqliteServerInfo;

  const std::vector<std::string> headers = DBTraits<UNQLITE>::infoHeaders();
  std::string word;
  DCHECK_EQ(headers.size(), 1);

  for (size_t i = 0; i < content.size(); ++i) {
      word += content[i];
      if (word == headers[0]) {
          std::string part = content.substr(i + 1);
          result->stats_ = UnqliteServerInfo::Stats(part);
          break;
      }
  }

  return result;
}


std::string UnqliteServerInfo::toString() const {
  std::stringstream str;
  str << UNQLITE_STATS_LABEL MARKER << stats_;
  return str.str();
}

uint32_t UnqliteServerInfo::version() const {
  return 0;
}

UnqliteServerInfo* makeUnqliteServerInfo(FastoObject* root) {
  std::string content = common::convertToString(root);
  return makeUnqliteServerInfo(content);
}

UnqliteDataBaseInfo::UnqliteDataBaseInfo(const std::string& name, bool isDefault,
                                         size_t size, const keys_container_t &keys)
  : IDataBaseInfo(name, isDefault, UNQLITE, size, keys) {
}

UnqliteDataBaseInfo* UnqliteDataBaseInfo::clone() const {
  return new UnqliteDataBaseInfo(*this);
}

UnqliteCommand::UnqliteCommand(FastoObject* parent, common::CommandValue* cmd,
                               const std::string& delemitr, const std::string& ns_separator)
  : FastoObjectCommand(parent, cmd, delemitr, ns_separator) {
}

bool UnqliteCommand::isReadOnly() const {
  std::string key = inputCmd();
  if (key.empty()) {
    return true;
  }

  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  return key != "get";
}

}  // namespace unqlite
}  // namespace core
}  // namespace fastonosql
