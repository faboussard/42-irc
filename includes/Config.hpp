/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:50:36 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/12 11:53:37 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#define CONFIG_FILE_PATH "./server.conf"

#include <map>
#include <string>

typedef std::map<std::string, std::string> parametersMap;
typedef std::map<std::string, size_t> numParametersMap;

class Config {
 private:
  parametersMap    _parameters;
  numParametersMap _numerictParameters;

  std::string      _supportedChanModes;
  std::string      _isupportParamTokens;

  void _parseConfigFile(const std::string& pathToConfigFile);
  void _setNumericParameters(void);
  void _setSupportedChanModes(void);
  void _setIsupportParamToken(void);

 public:
  explicit Config(const std::string& pathToConfigFile);

  size_t getLimit(const std::string& key) const;
  const std::string& getParam(const std::string& key) const;

  const std::string& getSupportedChanModes(void) const;
  const std::string& getIsupportParamToken(void) const;
};

void initServerConfig(void);

#endif  // INCLUDES_CONFIG_HPP_
