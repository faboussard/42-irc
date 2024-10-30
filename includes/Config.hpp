/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:50:36 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/30 13:49:51 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#include <map>
#include <string>

class Config {
 private:
  std::map<std::string, std::string> _parameters;

 public:
  explicit Config(const std::string& pathToConfigFile);

  void setParameter(const std::string& key, const std::string& value);
  const std::string& getParameter(const std::string& key) const;
};

#endif  // INCLUDES_CONFIG_HPP_
