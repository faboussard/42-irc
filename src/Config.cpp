/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:55:24 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/18 13:09:09 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../includes/Server.hpp"
#include "../includes/utils.hpp"

Config* gConfig = NULL;

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Config::Config(const std::string& pathToConfigFile) : _notDefined("") {
  parseConfigFile(pathToConfigFile);
  initializeSupportedChanModes();
  generateIsupportParamToken();
}

/*============================================================================*/
/*       Global function                                                      */
/*============================================================================*/

void initServerConfig(void) { gConfig = new Config(CONFIG_FILE_PATH); }

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

size_t Config::getLimit(eConfigKey key) const {
  return (_numerictParameters.at(key));
}

const std::string& Config::getParam(eConfigKey key) const {
  if (_parameters.find(key) != _parameters.end()) return (_parameters.at(key));
  return (_notDefined);
}

const std::string& Config::getIsupportParamToken(void) const {
  return (_isupportParamTokens);
}

const std::string& Config::getSupportedChanModes(void) const {
  return (_supportedChanModes);
}

/*============================================================================*/
/*       Private functions                                                    */
/*============================================================================*/

bool Config::isValidConfigValue(eConfigKey key, const std::string& keyStr,
                                const std::string& value) {
  if (!isNumericLimitConfig(key)) {
    if ((key == CASEMAPPING && value != DEFAULT_CASEMAPPING) ||
        (key == CHANMODES && value != DEFAULT_CHANMODES) ||
        (key == CHANTYPES && value != DEFAULT_CHANTYPES) ||
        (key == PREFIX && value != DEFAULT_PREFIX) ||
        (key == USERMODES && value != DEFAULT_USERMODES) ||
        (key == STATUSMSG && value != DEFAULT_STATUSMSG)) {
      Server::printLog(WARNING_LOG, SYSTEM, NOTSUPPORTED_VALUE(keyStr));
      return (false);
    }
  } else if (key == MODES && value != DEFAULT_MODES) {
    Server::printLog(WARNING_LOG, SYSTEM, NOTSUPPORTED_VALUE(keyStr));
    return (false);
  } else if (!isNumeric(value)) {
    Server::printLog(WARNING_LOG, SYSTEM, INVALID_VALUE(keyStr));
    return (false);
  }
  return (true);
}

void Config::parseConfigFile(const std::string& pathToConfigFile) {
  applyAllDefaultValues();
  std::string message =
      "Setting configuration. "
      "Missing parameters will be set to default values.";
  Server::printLog(INFO_LOG, SYSTEM, message);
  std::ifstream file(pathToConfigFile.c_str());
  if (!file.is_open()) {
    Server::printLog(
        WARNING_LOG, SYSTEM,
        "Failed to open config file. Using default configuration.");
  } else {
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::istringstream stream(buffer.str());

    std::string line;
    while (std::getline(stream, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream lineStream(line);
      std::string keyStr;
      if (std::getline(lineStream, keyStr, '=')) {
        eConfigKey key = identifyKey(keyStr);
        if (key == UNKNOWN_CONFIG) continue;
        std::string value;
        if (std::getline(lineStream, value) && !value.empty()) {
          if (isValidConfigValue(key, keyStr, value))
            _parameters.at(key) = value;
        } else {
          if (key == USERMODES) continue;
          Server::printLog(WARNING_LOG, SYSTEM, MISSING_PARAM(keyStr));
        }
      }
    }
    setNumericParameters();

    message = "Configuration successfully set.";
    Server::printLog(INFO_LOG, SYSTEM, message);

    std::ostringstream config;
    config << "Configuration: ";
    for (parametersMap::const_iterator it = _parameters.begin();
         it != _parameters.end(); ++it)
      config << keyToString(it->first) << "=" << it->second << " ";
    Server::printLog(DEBUG_LOG, SYSTEM, config.str());
  }
}

void Config::applyAllDefaultValues(void) {
  _parameters[CASEMAPPING] = DEFAULT_CASEMAPPING;
  _parameters[CHANLIMIT] = DEFAULT_CHANLIMIT;
  _parameters[CHANMODES] = DEFAULT_CHANMODES;
  _parameters[CHANNELLEN] = DEFAULT_CHANNELLEN;
  _parameters[CHANTYPES] = DEFAULT_CHANTYPES;
  _parameters[HOSTLEN] = DEFAULT_HOSTLEN;
  _parameters[KICKLEN] = DEFAULT_KICKLEN;
  _parameters[MAXTARGETS] = DEFAULT_MAXTARGETS;
  _parameters[NETWORK] = DEFAULT_NETWORK;
  _parameters[NICKLEN] = DEFAULT_NICKLEN;
  _parameters[PREFIX] = DEFAULT_PREFIX;
  _parameters[TOPICLEN] = DEFAULT_TOPICLEN;
  _parameters[USERLEN] = DEFAULT_USERLEN;
  _parameters[MODES] = DEFAULT_MODES;
  _parameters[USERMODES] = DEFAULT_USERMODES;
  _parameters[STATUSMSG] = DEFAULT_STATUSMSG;
}

void Config::setNumericParameters(void) {
  parametersMap::iterator itEnd = _parameters.end();
  for (parametersMap::iterator it = _parameters.begin(); it != itEnd; ++it) {
    if (isNumeric(it->second) && it->second != "") {
      char* end = NULL;
      errno = 0;
      size_t value = std::strtoul(it->second.c_str(), &end, 10);
      if (errno == ERANGE || *end != '\0' || !isWithinLimit(it->first, value)) {
        Server::printLog(WARNING_LOG, SYSTEM,
                         INVALID_VALUE(keyToString(it->first)));
        std::string defaultValue = getDefaultValue(it->first);
        value = std::strtoul(defaultValue.c_str(), &end, 10);
        _parameters.at(it->first) = defaultValue;
      }
      _numerictParameters[it->first] = value;
    }
  }
}

void Config::initializeSupportedChanModes(void) {
  std::string modes = _parameters.at(CHANMODES);
  std::istringstream iss(modes);
  std::string buffer;
  while (std::getline(iss, buffer, ',')) {
    _supportedChanModes += buffer;
  }
}

void Config::generateIsupportParamToken(void) {
  std::stringstream ss;
  parametersMap::const_iterator itEnd = _parameters.end();
  for (parametersMap::const_iterator it = _parameters.begin(); it != itEnd;
       ++it) {
    ss << keyToString(it->first) << "=" << it->second << " ";
    _isupportParamTokens = ss.str();
  }
}

/* Helper functions */

eConfigKey Config::identifyKey(const std::string& keyStr) const {
  if (keyStr == "NETWORK") return (NETWORK);
  if (keyStr == "CHANLIMIT") return (CHANLIMIT);
  if (keyStr == "CHANNELLEN") return (CHANNELLEN);
  if (keyStr == "MAXTARGETS") return (MAXTARGETS);
  if (keyStr == "MODES") return (MODES);
  if (keyStr == "NICKLEN") return (NICKLEN);
  if (keyStr == "HOSTLEN") return (HOSTLEN);
  if (keyStr == "KICKLEN") return (KICKLEN);
  if (keyStr == "TOPICLEN") return (TOPICLEN);
  if (keyStr == "USERLEN") return (USERLEN);
  if (keyStr == "CASEMAPPING") return (CASEMAPPING);
  if (keyStr == "CHANMODES") return (CHANMODES);
  if (keyStr == "CHANTYPES") return (CHANTYPES);
  if (keyStr == "PREFIX") return (PREFIX);
  if (keyStr == "USERMODES") return (USERMODES);
  if (keyStr == "STATUSMSG") return (STATUSMSG);
  return (UNKNOWN_CONFIG);
}

bool Config::isNumericLimitConfig(eConfigKey key) {
  if (key == CHANLIMIT || key == CHANNELLEN || key == MAXTARGETS ||
      key == MODES || key == NICKLEN || key == HOSTLEN || key == KICKLEN ||
      key == TOPICLEN || key == USERLEN)
    return (true);
  return (false);
}

std::string Config::keyToString(eConfigKey key) const {
  if (key == NETWORK) return ("NETWORK");
  if (key == CHANLIMIT) return ("CHANLIMIT");
  if (key == CHANNELLEN) return ("CHANNELLEN");
  if (key == MAXTARGETS) return ("MAXTARGETS");
  if (key == NICKLEN) return ("NICKLEN");
  if (key == HOSTLEN) return ("HOSTLEN");
  if (key == KICKLEN) return ("KICKLEN");
  if (key == TOPICLEN) return ("TOPICLEN");
  if (key == USERLEN) return ("USERLEN");
  if (key == CASEMAPPING) return ("CASEMAPPING");
  if (key == CHANMODES) return ("CHANMODES");
  if (key == CHANTYPES) return ("CHANTYPES");
  if (key == PREFIX) return ("PREFIX");
  if (key == MODES) return ("MODES");
  if (key == USERMODES) return ("USERMODES");
  if (key == STATUSMSG) return ("STATUSMSG");
  return ("");
}

std::string Config::getDefaultValue(eConfigKey key) const {
  if (key == NETWORK) return (DEFAULT_NETWORK);
  if (key == CHANLIMIT) return (DEFAULT_CHANLIMIT);
  if (key == CHANNELLEN) return (DEFAULT_CHANNELLEN);
  if (key == MAXTARGETS) return (DEFAULT_MAXTARGETS);
  if (key == MODES) return (DEFAULT_MODES);
  if (key == NICKLEN) return (DEFAULT_NICKLEN);
  if (key == HOSTLEN) return (DEFAULT_HOSTLEN);
  if (key == KICKLEN) return (DEFAULT_KICKLEN);
  if (key == TOPICLEN) return (DEFAULT_TOPICLEN);
  if (key == USERLEN) return (DEFAULT_USERLEN);
  if (key == CASEMAPPING) return (DEFAULT_CASEMAPPING);
  if (key == CHANMODES) return (DEFAULT_CHANMODES);
  if (key == CHANTYPES) return (DEFAULT_CHANTYPES);
  if (key == PREFIX) return (DEFAULT_PREFIX);
  return "";
}

bool Config::isWithinLimit(eConfigKey key, size_t value) const {
  if (key == CHANLIMIT && value > LIMIT_CHANLIMIT) return false;
  if (key == CHANNELLEN && value > LIMIT_CHANNELLEN) return false;
  if (key == MAXTARGETS && value > LIMIT_MAXTARGETS) return false;
  if (key == NICKLEN && value > LIMIT_NICKLEN) return false;
  if (key == HOSTLEN && value > LIMIT_HOSTLEN) return false;
  if (key == KICKLEN && value > LIMIT_KICKLEN) return false;
  if (key == TOPICLEN && value > LIMIT_TOPICLEN) return false;
  if (key == USERLEN && value > LIMIT_USERLEN) return false;
  return true;
}
