/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:50:36 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/15 22:52:52 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#include <map>
#include <string>

#include "./colors.hpp"

#define CONFIG_FILE_PATH "./server.conf"

enum eConfigKey {
  NETWORK,
  CHANLIMIT,
  CHANNELLEN,
  MAXTARGETS,
  NICKLEN,
  HOSTLEN,
  KICKLEN,
  TOPICLEN,
  USERLEN,
  CASEMAPPING,
  CHANMODES,
  CHANTYPES,
  PREFIX,
  MODES,
  USERMODES,
  STATUSMSG,
  UNKNOWN_CONFIG
};

typedef std::map<eConfigKey, std::string> parametersMap;
typedef std::map<eConfigKey, size_t> numParametersMap;

/* Default config */
#define DEFAULT_NETWORK "42IRC"
#define DEFAULT_CHANLIMIT "10"
#define DEFAULT_CHANNELLEN "200"
#define DEFAULT_MAXTARGETS "4"
#define DEFAULT_NICKLEN "9"
#define DEFAULT_HOSTLEN "63"
#define DEFAULT_KICKLEN "255"
#define DEFAULT_TOPICLEN "307"
#define DEFAULT_USERLEN "10"
#define DEFAULT_CASEMAPPING "ascii"
#define DEFAULT_CHANMODES "i,t,k,l"
#define DEFAULT_CHANTYPES "#"
#define DEFAULT_PREFIX "(o)@"
#define DEFAULT_MODES "4"
#define DEFAULT_USERMODES ""
#define DEFAULT_STATUSMSG "@"

#define LIMIT_CHANLIMIT 20
#define LIMIT_CHANNELLEN 200
#define LIMIT_MAXTARGETS 10
#define LIMIT_NICKLEN 30
#define LIMIT_HOSTLEN 255
#define LIMIT_KICKLEN 255
#define LIMIT_TOPICLEN 400
#define LIMIT_USERLEN 20

/* Messages */
#define INVALID_VALUE(key) ("Invalid value for " + key + ". " + \
                            "Using default value.")

#define NOTSUPPORTED_VALUE(key) ("Given value for " + key + \
                                " is not supported. Using default value.")

#define MISSING_PARAM(key) ("Missing " + key + ". Using default value.")

class Config {
 private:
  parametersMap      _parameters;
  numParametersMap   _numerictParameters;

  std::string        _supportedChanModes;
  std::string        _isupportParamTokens;
  const std::string  _notDefined;

 public:
  explicit Config(const std::string& pathToConfigFile);

  size_t getLimit(eConfigKey key) const;
  const std::string& getParam(eConfigKey key) const;

  const std::string& getSupportedChanModes(void) const;
  const std::string& getIsupportParamToken(void) const;

 private:
  void parseConfigFile(const std::string& pathToConfigFile);
  void applyAllDefaultValues(void);
  void setNumericParameters(void);
  void initializeSupportedChanModes(void);
  void generateIsupportParamToken(void);

  /* Helper functions */
  eConfigKey identifyKey(const std::string &keyStr) const;
  std::string getDefaultValue(eConfigKey key) const;
  std::string keyToString(eConfigKey key) const;
  bool isValidConfigValue(eConfigKey key, const std::string& keyStr,
                        const std::string& value);
  bool isNumericLimitConfig(eConfigKey key);
  bool isWithinLimit(eConfigKey key, size_t value) const;
};

void initServerConfig(void);

#endif  // INCLUDES_CONFIG_HPP_
