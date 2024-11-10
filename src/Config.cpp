/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:55:24 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/10 23:49:52 by yusengok         ###   ########.fr       */
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

size_t Config::getLimit(const std::string& key) const {
  return (_numerictParameters.at(key));
}

const std::string& Config::getParam(const std::string& key) const {
  if (_parameters.find(key) != _parameters.end())
    return (_parameters.at(key));
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

void Config::parseConfigFile(const std::string& pathToConfigFile) {
  initializeRequiredParams();
  initializeRequiredLimits();
  std::ifstream file(pathToConfigFile.c_str());
  if (!file.is_open()) {
    std::cerr << MISSING_CONFIG_FILE << std::endl;
    applyAllDefaultValues();
  } else {
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::istringstream stream(buffer.str());

    std::string line;
    while (std::getline(stream, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream lineStream(line);
      std::string key;
      if (std::getline(lineStream, key, '=')) {
        std::string value;
        if (std::getline(lineStream, value))
          _parameters[key] = value;
      }
    }
  }
  assignMissingConfigurations();
  setNumericParameters();
#ifdef DEBUG
  for (parametersMap::const_iterator it = _parameters.begin();
       it != _parameters.end(); ++it)
    std::cout << it->first << " => " << it->second << std::endl;
#endif
}

void Config::applyAllDefaultValues(void) {
  _parameters["CASEMAPPING"] = DEFAULT_CASEMAPPING;
  _parameters["CHANLIMIT"] = DEFAULT_CHANLIMIT;
  _parameters["CHANMODES"] = DEFAULT_CHANMODES;
  _parameters["CHANNELLEN"] = DEFAULT_CHANNELLEN;
  _parameters["CHANTYPES"] = DEFAULT_CHANTYPES;
  _parameters["HOSTLEN"] = DEFAULT_HOSTLEN;
  _parameters["KICKLEN"] = DEFAULT_KICKLEN;
  _parameters["MAXTARGETS"] = DEFAULT_MAXTARGETS;
  _parameters["NETWORK"] = DEFAULT_NETWORK;
  _parameters["NICKLEN"] = DEFAULT_NICKLEN;
  _parameters["PREFIX"] = DEFAULT_PREFIX;
  _parameters["TOPICLEN"] = DEFAULT_TOPICLEN;
  _parameters["USERLEN"] = DEFAULT_USERLEN;
  _parameters["MODES"] = DEFAULT_MODES;
}

void Config::assignMissingConfigurations(void) {
  stringSet::iterator itEnd = _requiredParams.end();
  for (stringSet::iterator it = _requiredParams.begin();
      it != itEnd; ++it) {
    #ifdef DEBUG
      std::cout << "Checking for " << *it << std::endl;
    #endif
    if (_parameters.find(*it) == _parameters.end() ||
        _parameters[*it] == "") {
      std::cerr << MISSING_PARAM(*it) << std::endl;
      _parameters[*it] = _getDefaultValue(*it);
    } else if (_requiredLimits.find(*it) != _requiredLimits.end() &&
               !isNumeric(_parameters[*it])) {
      std::cerr << INVALID_VALUE(*it) << std::endl;
        _parameters[*it] = _getDefaultValue(*it);
    }
  }
}

void Config::setNumericParameters(void) {
  parametersMap::iterator itEnd = _parameters.end();
  for (parametersMap::iterator it = _parameters.begin(); it != itEnd; ++it) {
    if (isNumeric(it->second) && it->second != "") {
      char* end = NULL;
      errno = 0;
      size_t value = std::strtoul(it->second.c_str(), &end, 10);
      if (errno == ERANGE || *end != '\0' || !isWithinLimit(it->first, value)) {
        std::cerr << INVALID_VALUE(it->first) << std::endl;
        value = std::strtoul(_getDefaultValue(it->first).c_str(), &end, 10);
      }
      _numerictParameters[it->first] = value;
    }
  }
}

void Config::initializeSupportedChanModes(void) {
  std::string modes = _parameters.at("CHANMODES");
  std::istringstream iss(modes);
  std::string buffer;
  while (std::getline(iss, buffer, ',')) {
    _supportedChanModes += buffer;
  }
}

void Config::generateIsupportParamToken(void) {
  parametersMap::const_iterator itEnd = _parameters.end();
  for (parametersMap::const_iterator it = _parameters.begin(); it != itEnd;
       ++it) {
    if (it->second.empty() ||
        _requiredParams.find(it->first) == _requiredParams.end())
      continue;
    else
      _isupportParamTokens += it->first + "=" + it->second + " ";
  }
}

/* Helper functions */

std::string Config::_getDefaultValue(const std::string &key) const {
  if (key == "NETWORK") return (DEFAULT_NETWORK);
  if (key == "CHANLIMIT") return (DEFAULT_CHANLIMIT);
  if (key == "CHANNELLEN") return (DEFAULT_CHANNELLEN);
  if (key == "MAXTARGETS") return (DEFAULT_MAXTARGETS);
  if (key == "MODES") return (DEFAULT_MODES);
  if (key == "NICKLEN") return (DEFAULT_NICKLEN);
  if (key == "HOSTLEN") return (DEFAULT_HOSTLEN);
  if (key == "KICKLEN") return (DEFAULT_KICKLEN);
  if (key == "TOPICLEN") return (DEFAULT_TOPICLEN);
  if (key == "USERLEN") return (DEFAULT_USERLEN);
  if (key == "CASEMAPPING") return (DEFAULT_CASEMAPPING);
  if (key == "CHANMODES") return (DEFAULT_CHANMODES);
  if (key == "CHANTYPES") return (DEFAULT_CHANTYPES);
  if (key == "PREFIX") return (DEFAULT_PREFIX);
  return "";
}

bool Config::isWithinLimit(const std::string &key, size_t value) const {
  if (key == "CHANLIMIT" && value > LIMIT_CHANLIMIT) return false;
  if (key == "CHANNELLEN" && value > LIMIT_CHANNELLEN) return false;
  if (key == "MAXTARGETS" && value > LIMIT_MAXTARGETS) return false;
  if (key == "NICKLEN" && value > LIMIT_NICKLEN) return false;
  if (key == "HOSTLEN" && value > LIMIT_HOSTLEN) return false;
  if (key == "KICKLEN" && value > LIMIT_KICKLEN) return false;
  if (key == "TOPICLEN" && value > LIMIT_TOPICLEN) return false;
  if (key == "USERLEN" && value > LIMIT_USERLEN) return false;
  return true;
}

void Config::initializeRequiredParams(void) {
  _requiredParams.insert("CASEMAPPING");
  _requiredParams.insert("CHANLIMIT");
  _requiredParams.insert("CHANMODES");
  _requiredParams.insert("CHANNELLEN");
  _requiredParams.insert("CHANTYPES");
  _requiredParams.insert("HOSTLEN");
  _requiredParams.insert("KICKLEN");
  _requiredParams.insert("MAXTARGETS");
  _requiredParams.insert("NETWORK");
  _requiredParams.insert("NICKLEN");
  _requiredParams.insert("PREFIX");
  _requiredParams.insert("TOPICLEN");
  _requiredParams.insert("USERLEN");
  _requiredParams.insert("MODES");
}

void Config::initializeRequiredLimits(void) {
  _requiredLimits.insert("CHANLIMIT");
  _requiredLimits.insert("CHANNELLEN");
  _requiredLimits.insert("HOSTLEN");
  _requiredLimits.insert("KICKLEN");
  _requiredLimits.insert("MAXTARGETS");
  _requiredLimits.insert("NICKLEN");
  _requiredLimits.insert("TOPICLEN");
  _requiredLimits.insert("USERLEN");
  _requiredLimits.insert("MODES");
}
