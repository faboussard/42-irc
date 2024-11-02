/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/02 23:50:12 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/numericReplies.hpp"

#include "../includes/Config.hpp"

/*============================================================================*/
/*       Welcome messages                                                     */
/*============================================================================*/

void send001Welcome(int fd, const std::string &nick, std::string const &user,
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

void send005Isupport(int fd, const std::string &nick) {
  std::string message = _005_RPL_ISUPPORT(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void sendWelcome(int fd, const std::string &nick) {
  std::string message = _WELCOME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*============================================================================*/
/*       Users related replies                                                */
/*============================================================================*/

void send221Umodeis(const Client &client) {
  std::string nick = client.getNickname();
  std::string uModes = client.getUserModesFlag();
  std::string message = _221_RPL_UMODEIS(nick, uModes);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*============================================================================*/
/*       Channel related replies                                              */
/*============================================================================*/

void send321Liststart(const Client &client) {
  std::string message = _321_RPL_LISTSTART(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send322List(const Client &client, const Channel &channel) {
  std::string numUsers = toString(channel.getClientsInChannel().size());
  std::string message = _322_RPL_LIST(client.getNickname(), channel.getNameWithPrefix(),
                                      numUsers, channel.getTopic().topic);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send323Listend(const Client &client) {
  std::string message = _323_RPL_LISTEND(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send324Channelmodeis(const Client &client,
                          const Channel &channel) {
  std::string modeArgs = "";
  if (channel.getMode().keyRequired) modeArgs += channel.getKey() + " ";
  if (channel.getMode().limitSet)
    modeArgs += toString(channel.getLimit()) + " ";
  std::string message =
      _324_RPL_CHANNELMODEIS(client.getNickname(), channel.getNameWithPrefix(),
                             channel.getChannelModeFlag(), modeArgs);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send329Creationtime(const Client &client,
                         const Channel &channel) {
  std::string message = _329_RPL_CREATIONTIME(client.getNickname(), channel.getNameWithPrefix(),
                                              channel.getCreationTime());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send331Notopic(const Client &client, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string message = _331_RPL_NOTOPIC(client.getNickname(), chanName);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send332Topic(const Client &client, const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string topic = channel.getTopic().topic;
  std::string message = _332_RPL_TOPIC(client.getNickname(), chanName, topic);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send333Topicwhotime(const Client &client,
                         const Channel &channel) {
  std::string chanName = channel.getNameWithPrefix();
  std::string author = channel.getTopic().author;
  std::string setTime = channel.getTopic().setTime;
  std::string message = _333_RPL_TOPICWHOTIME(client.getNickname(), chanName, author, setTime);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send336Invitelist(const Client &client,
                       const Channel &channel) {
  std::string message = _336_RPL_INVITELIST(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send337Endofinvitelist(const Client &client) {
  std::string message = _337_RPL_ENDOFINVITELIST(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send341Inviting(const Client &client,
                     const std::string &invitedNick, const Channel &channel) {
  std::string message =
      _341_RPL_INVITING(client.getNickname(), invitedNick, channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send353Namreply(const Client &client, const Channel &channel) {
  std::string chanNameWithSymbol =
      REG_CHAN + std::string(" ") + channel.getNameWithPrefix();

  std::string nicknames = "";
  clientPMap chanOps = channel.getChannelOperators();
  clientPMap::const_iterator itBegin = chanOps.begin();
  clientPMap::const_iterator itEnd = chanOps.end();
  for (clientPMap::const_iterator it = itBegin; it != itEnd; ++it) {
    nicknames += CHAN_OP + it->second->getNickname() + " ";
  }
  clientPMap chanClients = channel.getClientsInChannel();
  itBegin = chanClients.begin();
  itEnd = chanClients.end();
  for (clientPMap::const_iterator it = itBegin; it != itEnd; ++it) {
    if (chanOps.find(it->first) == chanOps.end())
      nicknames += it->second->getNickname() + " ";
  }

  std::string message = _353_RPL_NAMREPLY(client.getNickname(), chanNameWithSymbol, nicknames);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send366Endofnames(const Client &client,
                       const Channel &channel) {
  std::string message = _366_RPL_ENDOFNAMES(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*============================================================================*/
/*       Error messages                                                       */
/*============================================================================*/

void send401NoSuchNick(const Client &client,
                       const std::string &targetNick) {
  std::string message = _401_ERR_NOSUCHNICK(client.getNickname(), targetNick);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send403NoSuchChannel(const Client &client,
                          const std::string &chanName) {
  std::string message = _403_ERR_NOSUCHCHANNEL(client.getNickname(), chanName);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send404CannotSendToChan(const Client &client,
                             const Channel &channel) {
  std::string message =
      _404_ERR_CANNOTSENDTOCHAN(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send405TooManyChannels(const Client &client) {
  std::string message = _405_ERR_TOOMANYCHANNELS(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send409NoOrigin(const Client &client) {
  std::string message = _409_ERR_NOORIGIN(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send411NoRecipient(const Client &client,
                        const std::string &command) {
  std::string message = _411_ERR_NORECIPIENT(client.getNickname(), command);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send412NoTextToSend(const Client &client) {
  std::string message = _412_ERR_NOTEXTTOSEND(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send417InputTooLong(const Client &client) {
  std::string message = _417_ERR_INPUTTOOLONG(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send421UnknownCommand(const Client &client,
                           const std::string &command) {
  std::string message = _421_ERR_UNKNOWNCOMMAND(client.getNickname(), command);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send431NoNicknameGiven(const Client &client) {
  std::string message;
    message = _431_ERR_NONICKNAMEGIVEN(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send432ErroneusNickname(const Client &client) {
  std::string message = _432_ERR_ERRONEUSNICKNAME(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send433NickAlreadyInUse(const Client &client) {
  std::string message = _433_ERR_NICKNAMEINUSE(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send441UserNotInChannel(const Client &client,
                             const std::string &targetNick,
                             const Channel &channel) {
  std::string message =
      _441_ERR_USERNOTINCHANNEL(client.getNickname(), targetNick, channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send442NotOnChannel(const Client &client,
                         const Channel &channel) {
  std::string message =
      _442_ERR_NOTONCHANNEL(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send443UserOnChannel(const Client &client,
                          const std::string &invitedNick,
                          const Channel &channel) {
  std::string message =
      _443_ERR_USERONCHANNEL(client.getNickname(), invitedNick, channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send451NotRegistered(const Client &client) {
  std::string message = _451_ERR_NOTREGISTERED(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send461NeedMoreParams(const Client &client,
                           const std::string &command) {
  std::string message = _461_ERR_NEEDMOREPARAMS(client.getNickname(), command);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send462AlreadyRegistered(const Client &client) {
  std::string message = _462_ERR_ALREADYREGISTERED(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send464PasswdMismatch(const Client &client) {
  std::string message;
    message = _464_ERR_PASSWD_MISMATCH(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send471ChannelIsFull(const Client &client,
                          const Channel &channel) {
  std::string message =
      _471_ERR_CHANNELISFULL(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send472UnknownMode(const Client &client,
                        const std::string &modeChar) {
  std::string message = _472_ERR_UNKNOWNMODE(client.getNickname(), modeChar);
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send473InviteOnlyChan(const Client &client,
                           const Channel &channel) {
  std::string message =
      _473_ERR_INVITEONLYCHAN(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send475BadChannelKey(const Client &client,
                          const Channel &channel) {
  std::string message =
      _475_ERR_BADCHANNELKEY(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send476BadChanMask(const Client &client,
                        const Channel &channel) {
  std::string message = _476_ERR_BADCHANMASK(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send481NoPrivileges(const Client &client) {
  std::string message = _481_ERR_NOPRIVILEGES(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send482ChanOPrivsNeeded(const Client &client,
                             const Channel &channel) {
  std::string message =
      _482_ERR_CHANOPRIVSNEEDED(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send501UmodeUnknownFlag(const Client &client) {
  std::string message = _501_ERR_UMODEUNKNOWNFLAG(client.getNickname());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send525InvalidKey(const Client &client,
                       const Channel &channel) {
  std::string message = _525_ERR_INVALIDKEY(client.getNickname(), channel.getNameWithPrefix());
  if (send(client.getFd(), message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

/*============================================================================*/
/*       Unit test - numeric replies                                          */
/*============================================================================*/

  // testAllNumericReplies(_startTime, cli, "COMMAND", "puppy");
void testAllNumericReplies(const std::string &serverStartTime,
                           const Client &client, const std::string &command,
                           const std::string &targetNick) {
  int fd = client.getFd();
  Client testClient(fd, client.getIp(), client.getHostName());
  testClient.setNickname("testClient");
  testClient.setUserName("testUser");
  testClient.setRealName("Test User");
  testClient.setUInvisibleMode(true);
  testClient.setUOperatorMode(false);
  testClient.setURegisteredMode(true);
  std::string nick =testClient.getNickname();
  std::string user = testClient.getUserName();
  std::string host = testClient.getHostName();

  Channel testChannel("testChannel");
  Client testOp(4, "127.0.0.1", "localhost");
  testOp.setNickname("testOp");
  testChannel.acceptClientInTheChannel(&testClient);
  testChannel.addOperator(&testOp);
  Client testNonOPClient(42, "127.0.0.1", "localhost");
  testNonOPClient.setNickname("testNonOPClient");
  testChannel.acceptClientInTheChannel(&testNonOPClient);
  testChannel.activateLimitMode(42, testClient);
  testChannel.setTopic("This is the topic of a test channel", "Author");
  Channel invitedChannel("testInvited");
  invitedChannel.setTopic("This is a test channel", "Author");
  Channel kModeChannel("kModeChannel");
  kModeChannel.activateKeyMode("password", client);

  send(fd, "\n=========== This is a Numeric replies Test ===========\n", 59, 0);
  /* Welcome */
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, serverStartTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick);
  /* User */
  send221Umodeis(client);
  /* Channel */
  send321Liststart(client);
  send322List(client, testChannel);
  send323Listend(client);
  send324Channelmodeis(client, testChannel);
  send329Creationtime(client, testChannel);
  send331Notopic(client, testChannel);
  send332Topic(client, testChannel);
  send333Topicwhotime(client, testChannel);
  send341Inviting(client, targetNick, invitedChannel);
  send336Invitelist(client, invitedChannel);
  send337Endofinvitelist(client);
  send353Namreply(client, testChannel);
  send366Endofnames(client, testChannel);
  /* Errors */
  send401NoSuchNick(client, targetNick);
  send403NoSuchChannel(client, "notExistingChannel");
  send404CannotSendToChan(client, testChannel);
  send405TooManyChannels(client);
  send409NoOrigin(client);
  send411NoRecipient(client, command);
  send412NoTextToSend(client);
  send417InputTooLong(client);
  send421UnknownCommand(client, command);
  send431NoNicknameGiven(client);
  send432ErroneusNickname(client);
  send433NickAlreadyInUse(client);
  send441UserNotInChannel(client, targetNick, testChannel);
  send442NotOnChannel(client, testChannel);
  send443UserOnChannel(client, targetNick, invitedChannel);
  send451NotRegistered(client);
  send461NeedMoreParams(client, command);
  send462AlreadyRegistered(client);
  send464PasswdMismatch(client);
  send471ChannelIsFull(client, testChannel);
  send472UnknownMode(client, "x");
  send473InviteOnlyChan(client, kModeChannel);
  send475BadChannelKey(client, kModeChannel);
  send476BadChanMask(client, testChannel);
  send481NoPrivileges(client);
  send482ChanOPrivsNeeded(client, kModeChannel);
  send501UmodeUnknownFlag(client);
  send525InvalidKey(client, kModeChannel);
  send(fd, "==================== End of Test =====================\n\n", 59, 0);
}
