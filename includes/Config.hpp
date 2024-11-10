/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:50:36 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/10 23:52:55 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#define CONFIG_FILE_PATH "./server.conf"

#include <stdint.h>
#include <map>
#include <set>
#include <string>

#include "./colors.hpp"

// enum eConfig {
//   NETWORK,
//   CHANLIMIT,
//   CHANNELLEN,
//   MAXTARGETS,
//   NICKLEN,
//   HOSTLEN,
//   KICKLEN,
//   TOPICLEN,
//   USERLEN,
//   CASEMAPPING,
//   CHANMODES,
//   CHANTYPES,
//   PREFIX,
//   MODES,
//   USERMODES
// };

// enum eLimits {
//   CHANLIMIT,
//   CHANNELLEN,
//   MAXTARGETS,
//   NICKLEN,
//   HOSTLEN,
//   KICKLEN,
//   TOPICLEN,
//   USERLEN,
//   MODES,
// };

typedef std::map<std::string, std::string> parametersMap;
typedef std::map<std::string, size_t> numParametersMap;
// typedef std::map<eConfig, std::string> parametersMap;
// typedef std::map<eLimits, size_t> numParametersMap;
typedef std::set<std::string> stringSet;

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
#define DEFAULT_MODES "0"
#define DEFAULT_USERMODES ""

#define LIMIT_CHANLIMIT 20
#define LIMIT_CHANNELLEN 200
#define LIMIT_MAXTARGETS 10
#define LIMIT_NICKLEN 30
#define LIMIT_HOSTLEN 255
#define LIMIT_KICKLEN 255
#define LIMIT_TOPICLEN 400
#define LIMIT_USERLEN 20

/* Messages */
#define MISSING_CONFIG_FILE (std::string(YELLOW) + \
                            "WARNING [IRC server configurations]: " + \
                            "Failed to open config file. " + \
                            "Using default Isupport configuration." + RESET)

#define INVALID_VALUE(key) (std::string(YELLOW) + \
                            "WARNING [IRC server configurations]: " + \
                            "Invalid value for " + key + " parameter. " + \
                            "Using default value." + RESET)

#define MISSING_PARAM(key) (std::string(YELLOW) + \
                            "WARNING [IRC server configurations]: " + \
                            "Missing " + key + " parameter. " + \
                            "Using default value." + RESET)

class Config {
 private:
  parametersMap      _parameters;
  numParametersMap   _numerictParameters;

  std::string        _supportedChanModes;
  std::string        _isupportParamTokens;

  stringSet _requiredParams;
  stringSet _requiredLimits;
  const std::string  _notDefined;

  void parseConfigFile(const std::string& pathToConfigFile);
  void applyAllDefaultValues(void);
  void assignMissingConfigurations(void);
  void setNumericParameters(void);
  void initializeSupportedChanModes(void);
  void generateIsupportParamToken(void);

  /* Helper functions */
  std::string _getDefaultValue(const std::string &key) const;
  bool isWithinLimit(const std::string &key, size_t value) const;
  void initializeRequiredParams(void);
  void initializeRequiredLimits(void);

 public:
  explicit Config(const std::string& pathToConfigFile);

  size_t getLimit(const std::string& key) const;
  const std::string& getParam(const std::string& key) const;

  const std::string& getSupportedChanModes(void) const;
  const std::string& getIsupportParamToken(void) const;
};

void initServerConfig(void);

#endif  // INCLUDES_CONFIG_HPP_
