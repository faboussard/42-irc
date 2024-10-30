/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:55:24 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/30 23:05:19 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"

#include "../includes/Server.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Config::Config(const std::string& pathToConfigFile) {
  std::ifstream file(pathToConfigFile.c_str());
  if (!file.is_open())
    throw std::runtime_error("Failed to open config file");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::istringstream stream(buffer.str());

  std::string line;
  while (std::getline(stream, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream lineStream(line);
    std::string key;
    if (std::getline(lineStream, key, '=')) {
      std::string value;
      if (std::getline(lineStream, value)) {}
        _parameters[key] = value;
      std::cout << "key: " << key << " value: " << value << std::endl;
    }
  }
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

const std::string& Config::getParam(const std::string& key) const {
  return _parameters.at(key);
}

/*============================================================================*/
/*       Global function                                                      */
/*============================================================================*/

Config* gConfig = NULL;

void initServerConfig(void) {
  gConfig = new Config(CONFIG_FILE_PATH);
}
