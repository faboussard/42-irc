/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:55:24 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/31 11:49:08 by yusengok         ###   ########.fr       */
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

Config::Config(const std::string& pathToConfigFile) {
  _parseConfigFile(pathToConfigFile);
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
  return (_parameters.at(key));
}

// const std::string& Config::getChanPrefix(void) const {
//   return (_parameters.at("CHANTYPES"));
// }

/*============================================================================*/
/*       Public functions                                                     */
/*============================================================================*/

// const std::string& Config::generateIsupportParamToken(void) const {
  
// }

/*============================================================================*/
/*       Private functions                                                    */
/*============================================================================*/

void Config::_parseConfigFile(const std::string& pathToConfigFile) {
  std::ifstream file(pathToConfigFile.c_str());
  if (!file.is_open()) throw std::runtime_error("Failed to open config file");
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
      if (std::getline(lineStream, value)) {
      }
      _parameters[key] = value;
      std::cout << "key: " << key << " value: " << value << std::endl;
    }
  }
  _setNumericParameters();
}

void Config::_setNumericParameters(void) {
  parametersMap::iterator itEnd = _parameters.end();
  for (parametersMap::iterator it = _parameters.begin(); it != itEnd; ++it) {
    if (isNumeric(it->second) && it->second != "") {
      char* end = NULL;
      _numerictParameters[it->first] =
          std::strtoul(it->second.c_str(), &end, 10);
      std::cout << "key: " << it->first
                << " value: " << _numerictParameters[it->first] << std::endl;
    }
  }
}
