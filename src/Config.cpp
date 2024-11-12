/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:55:24 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/12 17:18:58 by yusengok         ###   ########.fr       */
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
#ifdef DEBUG
    std::cout << "< Value check for non numeric config > "
    << "keyStr: " << keyStr << " value: " << value << std::endl;
#endif
    if ((key == CASEMAPPING && value != DEFAULT_CASEMAPPING) ||
        (key == CHANMODES && value != DEFAULT_CHANMODES) ||
        (key == CHANTYPES && value != DEFAULT_CHANTYPES) ||
        (key == PREFIX && value != DEFAULT_PREFIX) ||
        (key == USERMODES && value != DEFAULT_USERMODES)) {
      std::cerr << NOTSUPPORTED_VALUE(keyStr) << std::endl;
      return (false);
    }
  } else if (!isNumeric(value)) {
    std::cerr << INVALID_VALUE(keyStr) << std::endl;
    return (false);
  }
  return (true);
}

void Config::parseConfigFile(const std::string& pathToConfigFile) {
  applyAllDefaultValues();
  std::ifstream file(pathToConfigFile.c_str());
  if (!file.is_open()) {
    std::cerr << MISSING_CONFIG_FILE << std::endl;
  } else {
    std::stringstream buffer;
    buffer << file.rdbuf();
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
            std::cerr << MISSING_PARAM(keyStr) << std::endl;
        }
      }
    }
    setNumericParameters();
#ifdef DEBUG
    std::cout << "===== All configurations =====" << std::endl;
    for (parametersMap::const_iterator it = _parameters.begin();
         it != _parameters.end(); ++it)
      std::cout << keyToString(it->first) << " => " << it->second << std::endl;
#endif
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
  }

  void Config::setNumericParameters(void) {
    parametersMap::iterator itEnd = _parameters.end();
    for (parametersMap::iterator it = _parameters.begin(); it != itEnd; ++it) {
      if (isNumeric(it->second) && it->second != "") {
        char* end = NULL;
        errno = 0;
        size_t value = std::strtoul(it->second.c_str(), &end, 10);
        if (errno == ERANGE || *end != '\0' ||
            !isWithinLimit(it->first, value)) {
          std::cerr << INVALID_VALUE(keyToString(it->first)) << std::endl;
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

  // size_t Config::getLimit(const std::string& key) const {
  //   return (_numerictParameters.at(key));
  // }

  // const std::string& Config::getParam(const std::string& key) const {
  //   if (_parameters.find(key) != _parameters.end())
  //     return (_parameters.at(key));
  //   return (_notDefined);
  // }

  // void Config::parseConfigFile(const std::string& pathToConfigFile) {
  //   initializeRequiredParams();
  //   initializeRequiredLimits();
  //   std::ifstream file(pathToConfigFile.c_str());
  //   if (!file.is_open()) {
  //     std::cerr << MISSING_CONFIG_FILE << std::endl;
  //     applyAllDefaultValues();
  //   } else {
  //     std::stringstream buffer;
  //     buffer << file.rdbuf();
  //     std::istringstream stream(buffer.str());

  //     std::string line;
  //     while (std::getline(stream, line)) {
  //       if (line.empty() || line[0] == '#') continue;
  //       std::istringstream lineStream(line);
  //       std::string key;
  //       if (std::getline(lineStream, key, '=')) {
  //         std::string value;
  //         if (std::getline(lineStream, value))
  //           _parameters[key] = value;
  //       }
  //     }
  //   }
  //   // assignMissingConfigurations();
  //   setNumericParameters();
  // #ifdef DEBUG
  //   for (parametersMap::const_iterator it = _parameters.begin();
  //        it != _parameters.end(); ++it)
  //     std::cout << it->first << " => " << it->second << std::endl;
  // #endif
  // }

  // void Config::applyAllDefaultValues(void) {
  //   _parameters["CASEMAPPING"] = DEFAULT_CASEMAPPING;
  //   _parameters["CHANLIMIT"] = DEFAULT_CHANLIMIT;
  //   _parameters["CHANMODES"] = DEFAULT_CHANMODES;
  //   _parameters["CHANNELLEN"] = DEFAULT_CHANNELLEN;
  //   _parameters["CHANTYPES"] = DEFAULT_CHANTYPES;
  //   _parameters["HOSTLEN"] = DEFAULT_HOSTLEN;
  //   _parameters["KICKLEN"] = DEFAULT_KICKLEN;
  //   _parameters["MAXTARGETS"] = DEFAULT_MAXTARGETS;
  //   _parameters["NETWORK"] = DEFAULT_NETWORK;
  //   _parameters["NICKLEN"] = DEFAULT_NICKLEN;
  //   _parameters["PREFIX"] = DEFAULT_PREFIX;
  //   _parameters["TOPICLEN"] = DEFAULT_TOPICLEN;
  //   _parameters["USERLEN"] = DEFAULT_USERLEN;
  //   _parameters["MODES"] = DEFAULT_MODES;
  // }

  // void Config::assignMissingConfigurations(void) {
  //   stringSet::iterator itEnd = _requiredParams.end();
  //   for (stringSet::iterator it = _requiredParams.begin();
  //       it != itEnd; ++it) {
  //     if (_parameters.find(*it) == _parameters.end() ||
  //         _parameters[*it] == "") {
  //       std::cerr << MISSING_PARAM(*it) << std::endl;
  //       _parameters[*it] = _getDefaultValue(*it);
  //     } else if (_requiredLimits.find(*it) != _requiredLimits.end() &&
  //                !isNumeric(_parameters[*it])) {
  //       std::cerr << INVALID_VALUE(*it) << std::endl;
  //         _parameters[*it] = _getDefaultValue(*it);
  //     }
  //   }
  // }

  // void Config::initializeSupportedChanModes(void) {
  //   std::string modes = _parameters.at("CHANMODES");
  //   std::istringstream iss(modes);
  //   std::string buffer;
  //   while (std::getline(iss, buffer, ',')) {
  //     _supportedChanModes += buffer;
  //   }
  // }

  // void Config::generateIsupportParamToken(void) {
  //   parametersMap::const_iterator itEnd = _parameters.end();
  //   for (parametersMap::const_iterator it = _parameters.begin(); it != itEnd;
  //        ++it) {
  //     if (it->second.empty() ||
  //         _requiredParams.find(it->first) == _requiredParams.end())
  //       continue;
  //     else
  //       _isupportParamTokens += it->first + "=" + it->second + " ";
  //   }
  // }

  // std::string Config::_getDefaultValue(const std::string &key) const {
  //   if (key == "NETWORK") return (DEFAULT_NETWORK);
  //   if (key == "CHANLIMIT") return (DEFAULT_CHANLIMIT);
  //   if (key == "CHANNELLEN") return (DEFAULT_CHANNELLEN);
  //   if (key == "MAXTARGETS") return (DEFAULT_MAXTARGETS);
  //   if (key == "MODES") return (DEFAULT_MODES);
  //   if (key == "NICKLEN") return (DEFAULT_NICKLEN);
  //   if (key == "HOSTLEN") return (DEFAULT_HOSTLEN);
  //   if (key == "KICKLEN") return (DEFAULT_KICKLEN);
  //   if (key == "TOPICLEN") return (DEFAULT_TOPICLEN);
  //   if (key == "USERLEN") return (DEFAULT_USERLEN);
  //   if (key == "CASEMAPPING") return (DEFAULT_CASEMAPPING);
  //   if (key == "CHANMODES") return (DEFAULT_CHANMODES);
  //   if (key == "CHANTYPES") return (DEFAULT_CHANTYPES);
  //   if (key == "PREFIX") return (DEFAULT_PREFIX);
  //   return "";
  // }

  // bool Config::isWithinLimit(const std::string &key, size_t value) const {
  //   if (key == "CHANLIMIT" && value > LIMIT_CHANLIMIT) return false;
  //   if (key == "CHANNELLEN" && value > LIMIT_CHANNELLEN) return false;
  //   if (key == "MAXTARGETS" && value > LIMIT_MAXTARGETS) return false;
  //   if (key == "NICKLEN" && value > LIMIT_NICKLEN) return false;
  //   if (key == "HOSTLEN" && value > LIMIT_HOSTLEN) return false;
  //   if (key == "KICKLEN" && value > LIMIT_KICKLEN) return false;
  //   if (key == "TOPICLEN" && value > LIMIT_TOPICLEN) return false;
  //   if (key == "USERLEN" && value > LIMIT_USERLEN) return false;
  //   return true;
  // }

  // void Config::initializeRequiredParams(void) {
  //   _requiredParams.insert("CASEMAPPING");
  //   _requiredParams.insert("CHANLIMIT");
  //   _requiredParams.insert("CHANMODES");
  //   _requiredParams.insert("CHANNELLEN");
  //   _requiredParams.insert("CHANTYPES");
  //   _requiredParams.insert("HOSTLEN");
  //   _requiredParams.insert("KICKLEN");
  //   _requiredParams.insert("MAXTARGETS");
  //   _requiredParams.insert("NETWORK");
  //   _requiredParams.insert("NICKLEN");
  //   _requiredParams.insert("PREFIX");
  //   _requiredParams.insert("TOPICLEN");
  //   _requiredParams.insert("USERLEN");
  //   _requiredParams.insert("MODES");
  // }

  // void Config::initializeRequiredLimits(void) {
  //   _requiredLimits.insert("CHANLIMIT");
  //   _requiredLimits.insert("CHANNELLEN");
  //   _requiredLimits.insert("HOSTLEN");
  //   _requiredLimits.insert("KICKLEN");
  //   _requiredLimits.insert("MAXTARGETS");
  //   _requiredLimits.insert("NICKLEN");
  //   _requiredLimits.insert("TOPICLEN");
  //   _requiredLimits.insert("USERLEN");
  //   _requiredLimits.insert("MODES");
  // }
