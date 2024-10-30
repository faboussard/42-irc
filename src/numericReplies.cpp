/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/30 14:53:05 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/numericReplies.hpp"

#include "../includes/colors.hpp"

/*------ Welcome messages ----------------------------------------------------*/

void send001Welcome(int fd, std::string const &nick, std::string const &user,
                    std::string const &host) {
  std::string message = _001_RPL_WELCOME(nick, user, host);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send002Yourhost(int fd, const std::string &nick) {
  std::string message = _002_RPL_YOURHOST(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send003Created(int fd, const std::string &nick,
                    const std::string &startTime) {
  std::string message = _003_RPL_CREATED(nick, startTime);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send104Myinfo(int fd, const std::string &nick) {
  std::string message = _004_RPL_MYINFO(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send005Isupport(int fd, const std::string &nick,
                     const std::string &tokens) {
  std::string message = _005_RPL_ISUPPORT(nick, tokens);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void sendWelcome(int fd, const std::string &nick) {
  std::string message = _WELCOME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*------ Users related replies -----------------------------------------------*/

void send221Umodeis(int fd, const Client &client) {
  std::string nick = client.getNickName().empty() ? "*" : client.getNickName();
  std::string uModes = client.getUserModesFlag();
  std::string message = _221_RPL_UMODEIS(nick, uModes);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*------ Channel related replies ---------------------------------------------*/
void send321Liststart(int fd, const std::string &nick) {
  std::string message = _321_RPL_LISTSTART(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send322List(int fd, const std::string &nick, const Channel &channel) {
  std::string numUsers = toString(channel.getClientsInChannel().size());
  std::string message = _322_RPL_LIST(nick, channel.getNameWithPrefix(),
                                      numUsers, channel.getTopic().topicName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send323Listend(int fd, const std::string &nick) {
  std::string message = _323_RPL_LISTEND(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send324Channelmodeis(int fd, const std::string &nick,
                          const Channel &channel) {
  std::string modeArgs = "";
  if (channel.getMode().keyRequired) modeArgs += channel.getKey() + " ";
  if (channel.getMode().limitSet)
    modeArgs += toString(channel.getLimit()) + " ";
  std::string message =
      _324_RPL_CHANNELMODEIS(nick, channel.getNameWithPrefix(),
                             channel.getChannelModeFlag(), modeArgs);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send329Creationtime(int fd, const std::string &nick,
                         const Channel &channel) {
  std::string message = _329_RPL_CREATIONTIME(nick, channel.getNameWithPrefix(),
                                              channel.getCreationTime());
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send331Notopic(int fd, const std::string &nick, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string message = _331_RPL_NOTOPIC(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send332Topic(int fd, const std::string &nick, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string topicName = channel.getTopic().topicName;
  std::string message = _332_RPL_TOPIC(nick, chanName, topicName);
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
  std::string chanNameWithSymbol =
      GENERAL_CHAN_SYMBOL + std::string(" ") + channel.getNameWithPrefix();

  std::string nicknames = "";
  clientsMap chanOps = channel.getChannelOperators();
  clientsMap::const_iterator itBegin = chanOps.begin();
  clientsMap::const_iterator itEnd = chanOps.end();
  for (clientsMap::const_iterator it = itBegin; it != itEnd; ++it) {
    nicknames += OP_PREFIX + it->second.getNickName() + " ";
  }
  clientsMap chanClients = channel.getClientsInChannel();
  itBegin = chanClients.begin();
  itEnd = chanClients.end();
  for (clientsMap::const_iterator it = itBegin; it != itEnd; ++it) {
    if (chanOps.find(it->first) == chanOps.end())
      nicknames += it->second.getNickName() + " ";
  }

  std::string message = _353_RPL_NAMREPLY(nick, chanNameWithSymbol, nicknames);
    #ifdef DEBUG
  std::cout << " nick " << nick << " nicknames " << nicknames << " chanNameWithSymbol " << chanNameWithSymbol <<  std::endl;
  #endif
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send366Endofnames(int fd, const std::string &nick,
                       const std::string &chanName) {
  #ifdef DEBUG
  std::cout << "nick " << nick << " chanName " << chanName <<  std::endl;
  #endif
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

void send404CannotSendToChan(int fd, const std::string &nick,
                             const std::string &chanName) {
  std::string message = _404_ERR_CANNOTSENDTOCHAN(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send405TooManyChannels(int fd, const std::string &nick) {
  std::string message = _405_ERR_TOOMANYCHANNELS(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send409NoOrigin(int fd, const std::string &nick) {
  std::string message = _409_ERR_NOORIGIN(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send411NoRecipient(int fd, const std::string &nick,
                        const std::string &command) {
  std::string message = _411_ERR_NORECIPIENT(nick, command);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send412NoTextToSend(int fd, const std::string &nick) {
  std::string message = _412_ERR_NOTEXTTOSEND(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send417InputTooLong(int fd, const std::string &nick) {
  std::string message = _417_ERR_INPUTTOOLONG(nick);
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
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send432ErroneusNickname(int fd, const std::string &nick) {
  std::string message = _432_ERR_ERRONEUSNICKNAME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send433NickAlreadyInUse(int fd, const std::string &nick) {
  std::string message = _433_ERR_NICKNAMEINUSE(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send441UserNotInChannel(int fd, const std::string &nick,
                             const std::string &targetNick,
                             const std::string &chanName) {
  std::string message = _441_ERR_USERNOTINCHANNEL(nick, targetNick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send442NotOnChannel(int fd, const std::string &nick,
                         const std::string &chanName) {
  std::string message = _442_ERR_NOTONCHANNEL(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send443UserOnChannel(int fd, const std::string &nick,
                          const std::string &invitedNick,
                          const std::string &chanName) {
  std::string message = _443_ERR_USERONCHANNEL(nick, invitedNick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send451NotRegistered(int fd, const std::string &nick) {
  std::string message = _451_ERR_NOTREGISTERED(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send461NeedMoreParams(int fd, const std::string &nick,
                           const std::string &command) {
  std::string message = _461_ERR_NEEDMOREPARAMS(nick, command);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send462AlreadyRegistered(int fd, const std::string &nick) {
  std::string message = _462_ERR_ALREADYREGISTERED(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send464PasswdMismatch(int fd, const std::string &nick) {
  std::string message;
  if (nick.empty())
    message = _464_ERR_PASSWD_MISMATCH("*");
  else
    message = _464_ERR_PASSWD_MISMATCH(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send471ChannelIsFull(int fd, const std::string &nick,
                          const std::string &chanName) {
  std::string message = _471_ERR_CHANNELISFULL(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send472UnknownMode(int fd, const std::string &nick,
                        const std::string &modeChar) {
  std::string message = _472_ERR_UNKNOWNMODE(nick, modeChar);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send473InviteOnlyChan(int fd, const std::string &nick,
                           const std::string &chanName) {
  std::string message = _473_ERR_INVITEONLYCHAN(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send475BadChannelKey(int fd, const std::string &nick,
                          const std::string &chanName) {
  std::string message = _475_ERR_BADCHANNELKEY(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send476BadChanMask(int fd, const std::string &nick,
                        const std::string &chanName) {
  std::string message = _476_ERR_BADCHANMASK(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send481NoPrivileges(int fd, const std::string &nick) {
  std::string message = _481_ERR_NOPRIVILEGES(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send482ChanOPrivsNeeded(int fd, const std::string &nick,
                             const std::string &chanName) {
  std::string message = _482_ERR_CHANOPRIVSNEEDED(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send501UmodeUnknownFlag(int fd, const std::string &nick) {
  std::string message = _501_ERR_UMODEUNKNOWNFLAG(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send525InvalidKey(int fd, const std::string &nick,
                       const std::string &chanName) {
  std::string message = _525_ERR_INVALIDKEY(nick, chanName);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
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
  send321Liststart(fd, nick);
  send322List(fd, nick, testChannel);
  send323Listend(fd, nick);
  send324Channelmodeis(fd, nick, testChannel);
  send329Creationtime(fd, nick, testChannel);
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
  send404CannotSendToChan(fd, nick, testChannel.getNameWithPrefix());
  send405TooManyChannels(fd, nick);
  send409NoOrigin(fd, nick);
  send411NoRecipient(fd, nick, command);
  send412NoTextToSend(fd, nick);
  send417InputTooLong(fd, nick);
  send421UnknownCommand(fd, nick, command);
  send431NoNicknameGiven(fd, nick);
  send432ErroneusNickname(fd, nick);
  send433NickAlreadyInUse(fd, nick);
  send441UserNotInChannel(fd, nick, targetNick,
                          testChannel.getNameWithPrefix());
  send442NotOnChannel(fd, nick, testChannel.getNameWithPrefix());
  send443UserOnChannel(fd, nick, targetNick,
                       invitedChannel.getNameWithPrefix());
  send451NotRegistered(fd, nick);
  send461NeedMoreParams(fd, nick, command);
  send462AlreadyRegistered(fd, nick);
  send464PasswdMismatch(fd, nick);
  send471ChannelIsFull(fd, nick, testChannel.getNameWithPrefix());
  send472UnknownMode(fd, nick, "x");
  send473InviteOnlyChan(fd, nick, privateChannel.getNameWithPrefix());
  send475BadChannelKey(fd, nick, privateChannel.getNameWithPrefix());
  send476BadChanMask(fd, nick, testChannel.getNameWithPrefix());
  send481NoPrivileges(fd, nick);
  send482ChanOPrivsNeeded(fd, nick, privateChannel.getNameWithPrefix());
  send501UmodeUnknownFlag(fd, nick);
  send525InvalidKey(fd, nick, privateChannel.getNameWithPrefix());
}
