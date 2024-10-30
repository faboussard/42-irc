/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:50:36 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/30 23:10:26 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#define CONFIG_FILE_PATH "./server.conf"

#include <map>
#include <string>

class Config {
 private:
  std::map<std::string, std::string> _parameters;

 public:
  explicit Config(const std::string& pathToConfigFile);

  const std::string& getParam(const std::string& key) const;
};

void initServerConfig(void);

#endif  // INCLUDES_CONFIG_HPP_
