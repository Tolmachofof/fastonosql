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

#pragma once

#include "core/connection_settings/connection_settings.h"
#include "core/connection_settings/iconnection_settings_remote.h"
#include "core/connection_settings/iconnection_settings_ssh.h"

namespace fastonosql {
namespace core {

class ConnectionSettingsFactory {
 public:
  static IConnectionSettingsBase* CreateFromType(connectionTypes type,
                                                 const connection_path_t& conName);
  static IConnectionSettingsBase* FromString(const std::string& val);

  static IConnectionSettingsRemote* CreateFromType(connectionTypes type,
                                                   const connection_path_t& conName,
                                                   const common::net::HostAndPort& host);

  static IConnectionSettingsRemoteSSH* createFromType(connectionTypes type,
                                                      const connection_path_t& conName,
                                                      const common::net::HostAndPort& host);
};

}  // namespace core
}  // namespace fastonosql
