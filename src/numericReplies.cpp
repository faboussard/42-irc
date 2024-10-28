/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <fanny@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/28 17:59:22 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);

// general format of how numeric replies are structured when sent to the client:
// :<source> <numeric> <target> <parameters> :<message>
// source: The source of the message (server name or a user's nickname for
// direct user messages). target: nickname of the client receiving the message
// parameters: Additional parameters depending on the type of message
// message: A human readable text message (welcome message or error description)

#include "../includes/numericReplies.hpp"

#include "../includes/colors.hpp"

/*------ Welcome messages ----------------------------------------------------*/

void send001Welcome(int fd, std::string const &nick, std::string const &user,
                    std::string const &host) {
  std::string message = _001_RPL_WELCOME(nick, user, host);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send002Yourhost(int fd, const std::string &nick) {
  std::string message = _002_RPL_YOURHOST(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send003Created(int fd, const std::string &nick,
                    const std::string &startTime) {
  std::string message = _003_RPL_CREATED(nick, startTime);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send104Myinfo(int fd, const std::string &nick) {
  std::string message = _004_RPL_MYINFO(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send005Isupport(int fd, const std::string &nick,
                     const std::string &tokens) {
  std::string message = _005_RPL_ISUPPORT(nick, tokens);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void sendWelcome(int fd, const std::string &nick) {
  std::string message = _WELCOME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

/*------ Users related messages ----------------------------------------------*/

void send221Umodeis(int fd, const Client &client) {
  std::string nick = client.getNickName().empty() ? "*" : client.getNickName();
  std::string uModes = client.getUserModesFlag();
  std::string message = _221_RPL_UMODEIS(nick, uModes);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

/*------ Channel related messages --------------------------------------------*/

void send331Notopic(int fd, const std::string &nick, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string message = _331_RPL_NOTOPIC(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send332Topic(int fd, const std::string &nick, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string topic = channel.getTopic().topic;
  std::string message = _332_RPL_TOPIC(nick, chanName, topic);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send333Topicwhotime(int fd, const std::string &nick,
                         const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string author = channel.getTopic().author;
  std::string setTime = channel.getTopic().setTime;
  std::string message = _333_RPL_TOPICWHOTIME(nick, chanName, author, setTime);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send336Invitelist(int fd, const std::string &nick,
                       const std::string &chanName) {
  std::string message = _336_RPL_INVITELIST(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send337Endofinvitelist(int fd, const std::string &nick) {
  std::string message = _337_RPL_ENDOFINVITELIST(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send341Inviting(int fd, const std::string &nick,
                     const std::string &invitedNick,
                     const std::string &chanName) {
  std::string message = _341_RPL_INVITING(nick, invitedNick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send353Namreply(int fd, const std::string &nick, const Channel &channel) {
  std::string chanNameWithSymbol = channel.getMode().keyRequired ? "* " : "= " ;
  chanNameWithSymbol += channel.getNameWithPrefix();

  std::string nicknames = "";
  clientsMap chanOps = channel.getChannelOperators();
  clientsMap::const_iterator itBegin = chanOps.begin();
  clientsMap::const_iterator itEnd = chanOps.end();
  for (clientsMap::const_iterator it = itBegin; it != itEnd; ++it) {
    nicknames += "@" + it->second.getNickName() + " ";
  }
  clientsMap chanClients = channel.getClientsInChannel();
  itBegin = chanClients.begin();
  itEnd = chanClients.end();
  for (clientsMap::const_iterator it = itBegin; it != itEnd; ++it) {
    if (chanOps.find(it->first) == chanOps.end())
      nicknames += it->second.getNickName() + " ";
  }
  std::string message = _353_RPL_NAMREPLY(nick, chanNameWithSymbol, nicknames);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send366Endofnames(int fd, const std::string &nick,
                       const std::string &chanName) {
  std::string message = _366_RPL_ENDOFNAMES(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*------ Error messages ------------------------------------------------------*/

void send401NoSuchNick(int fd, const std::string &nick,
                       const std::string &targetNick) {
  std::string message = _401_ERR_NOSUCHNICK(nick, targetNick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send403NoSuchChannel(int fd, const std::string &nick,
                          const std::string &chanName) {
  std::string message = _403_ERR_NOSUCHCHANNEL(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send421UnknownCommand(int fd, const std::string &nick,
                           const std::string &command) {
  std::string message = _421_ERR_UNKNOWNCOMMAND(nick, command);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send431NoNicknameGiven(int fd, const std::string &nick) {
  std::string message;
  if (nick.empty())
    message = _431_ERR_NONICKNAMEGIVEN("*");
  else
    message = _431_ERR_NONICKNAMEGIVEN(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send432ErroneusNickname(int fd, const std::string &nick) {
  std::string message = _432_ERR_ERRONEUSNICKNAME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send433NickAlreadyInUse(int fd, const std::string &nick) {
  std::string message = _433_ERR_NICKNAMEINUSE(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send442NotOnChannel(int fd, const std::string &nick,
                         const std::string &chanName) {
  std::string message = _442_ERR_NOTONCHANNEL(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send443UserOnChannel(int fd, const std::string &nick,
                          const std::string &invitedNick,
                          const std::string &chanName) {
  std::string message = _443_ERR_USERONCHANNEL(nick, invitedNick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send461NeedMoreParams(int fd, const std::string &nick,
                           const std::string &command) {
  std::string message = _461_ERR_NEEDMOREPARAMS(nick, command);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send462AlreadyRegistered(int fd, const std::string &nick) {
  std::string message = _462_ERR_ALREADYREGISTERED(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send464PasswdMismatch(int fd, const std::string &nick) {
  std::string message;
  if (nick.empty())
    message = _464_ERR_PASSWD_MISMATCH("*");
  else
    message = _464_ERR_PASSWD_MISMATCH(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send482ChanOPrivsNeeded(int fd, const std::string &nick,
                             const std::string &chanName) {
  std::string message = _482_ERR_CHANOPRIVSNEEDED(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}


/* Just for test */
void testAllNumericReplies(const std::string &serverStartTime,
                           const Client &client, const std::string &command,
                           const std::string &targetNick) {
  int fd = client.getFd();
  std::string nick = client.getNickName().empty() ? "*" : client.getNickName();
  std::string user = client.getUserName().empty() ? "*" : client.getUserName();
  std::string host = client.getIp().empty() ? "*" : client.getIp();
  Channel testChannel("testChannel");
  testChannel.setTopic("This is the topic of a test channel", "Author");
  Channel invitedChannel("testInvited");
  invitedChannel.setTopic("This is a test channel", "Author");
  Channel privateChannel("privateChannel");
  privateChannel.activateKeyMode("password", client);

  /* Welcome */
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, serverStartTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick, TOKENS);
  /* User */
  send221Umodeis(fd, client);
  /* Channel */
  send331Notopic(fd, nick, testChannel);
  send332Topic(fd, nick, testChannel);
  send333Topicwhotime(fd, nick, testChannel);
  send341Inviting(fd, nick, targetNick, invitedChannel.getNameWithPrefix());
  send336Invitelist(fd, nick, invitedChannel.getNameWithPrefix());
  send337Endofinvitelist(fd, nick);
  send353Namreply(fd, nick, testChannel);
  send366Endofnames(fd, nick, testChannel.getNameWithPrefix());
  /* Errors */
  send401NoSuchNick(fd, nick, targetNick);
  send403NoSuchChannel(fd, nick, "notExistingChannel");
  send421UnknownCommand(fd, nick, command);
  send431NoNicknameGiven(fd, nick);
  send432ErroneusNickname(fd, nick);
  send433NickAlreadyInUse(fd, nick);
  send442NotOnChannel(fd, nick, testChannel.getNameWithPrefix());
  send443UserOnChannel(fd, nick, targetNick, invitedChannel.getNameWithPrefix());
  send461NeedMoreParams(fd, nick, command);
  send462AlreadyRegistered(fd, nick);
  send464PasswdMismatch(fd, nick);
  send482ChanOPrivsNeeded(fd, nick, privateChannel.getNameWithPrefix());
}
