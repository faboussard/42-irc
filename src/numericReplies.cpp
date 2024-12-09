/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/07 16:12:36 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/numericReplies.hpp"

#include <string>

#include "../includes/Config.hpp"
#include "../includes/Log.hpp"

void sendNumericReply(int fd, std::string *message) {
  if (message == NULL) return;
  if (send(fd, message->c_str(), message->size(), MSG_NOSIGNAL) == -1) {
    std::ostringstream oss;
    oss << RUNTIME_ERROR << " to " << "fd " << fd;
    Log::printLog(ERROR_LOG, REPLY, oss.str());
    return;
  }

  message->erase(0, message->find(' ') + 1);
  Log::printLog(INFO_LOG, REPLY,
                   message->erase(message->find_last_not_of("\r\n") + 1));
}

/*============================================================================*/
/*       Welcome messages                                                     */
/*============================================================================*/

void send001Welcome(int fd, const std::string &nick, std::string const &user,
                    std::string const &host) {
  std::string message = _001_RPL_WELCOME(nick, user, host);
  sendNumericReply(fd, &message);
}

void send002Yourhost(int fd, const std::string &nick) {
  std::string message = _002_RPL_YOURHOST(nick);
  sendNumericReply(fd, &message);
}

void send003Created(int fd, const std::string &nick,
                    const std::string &startTime) {
  std::string message = _003_RPL_CREATED(nick, startTime);
  sendNumericReply(fd, &message);
}

void send104Myinfo(int fd, const std::string &nick) {
  std::string message = _004_RPL_MYINFO(nick);
  sendNumericReply(fd, &message);
}

void send005Isupport(int fd, const std::string &nick) {
  std::string message = _005_RPL_ISUPPORT(nick);
  sendNumericReply(fd, &message);
}

void sendWelcome(int fd, const std::string &nick) {
  std::string message = _WELCOME(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    std::ostringstream oss;
    oss << RUNTIME_ERROR << " to " << "fd " << fd;
    Log::printLog(ERROR_LOG, REPLY, oss.str());
  }
}

/*============================================================================*/
/*       Users related replies                                                */
/*============================================================================*/

void send315EndOfWho(const Client &client, const Channel &channel) {
  std::string message =
      _315_RPL_ENDOFWHO(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send352Whoreply(const Client &client, const Client &clientInChannel,
                     const Channel &channel) {
  std::string message = _352_RPL_WHOREPLY(
      client.getNickname(), channel.getNameWithPrefix(),
      clientInChannel.getUserName(), clientInChannel.getHostName(),
      clientInChannel.getNickname(), clientInChannel.getRealName());
  sendNumericReply(client.getFd(), &message);
}

/*============================================================================*/
/*       Channel related replies                                              */
/*============================================================================*/

void send321Liststart(int fd, const std::string &nick) {
  std::string message = _321_RPL_LISTSTART(nick);
  sendNumericReply(fd, &message);
}

void send322List(int fd, const std::string &nick, const Channel &channel) {
  std::string numUsers = toString(channel.getChannelClients().size());
  std::string message = _322_RPL_LIST(nick, channel.getNameWithPrefix(),
                                      numUsers, channel.getTopic().topic);
  sendNumericReply(fd, &message);
}

void send323Listend(int fd, const std::string &nick) {
  std::string message = _323_RPL_LISTEND(nick);
  sendNumericReply(fd, &message);
}

void send324Channelmodeis(const Client &client, const Channel &channel) {
  std::ostringstream modeArgs;
  if (channel.getMode().keyRequired) modeArgs << channel.getKey() << ' ';
  if (channel.getMode().limitSet) modeArgs << channel.getLimit();
  std::string message =
      _324_RPL_CHANNELMODEIS(client.getNickname(), channel.getNameWithPrefix(),
                             channel.getChannelModeFlag(), modeArgs.str());
  sendNumericReply(client.getFd(), &message);
}

void send329Creationtime(const Client &client, const Channel &channel) {
  std::string message =
      _329_RPL_CREATIONTIME(client.getNickname(), channel.getNameWithPrefix(),
                            channel.getCreationTime());
  sendNumericReply(client.getFd(), &message);
}

void send331Notopic(const Client &client, const Channel &channel) {
  std::string message =
      _331_RPL_NOTOPIC(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send332Topic(const Client &client, const Channel &channel) {
  std::string message =
      _332_RPL_TOPIC(client.getNickname(), channel.getNameWithPrefix(),
                     channel.getTopic().topic);
  sendNumericReply(client.getFd(), &message);
}

void send333Topicwhotime(const Client &client, const Channel &channel) {
  std::string message = _333_RPL_TOPICWHOTIME(
      client.getNickname(), channel.getNameWithPrefix(),
      channel.getTopic().author, channel.getTopic().setTime);
  sendNumericReply(client.getFd(), &message);
}

void send336Invitelist(const Client &client, const Channel &channel) {
  std::string message =
      _336_RPL_INVITELIST(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send337Endofinvitelist(const Client &client) {
  std::string message = _337_RPL_ENDOFINVITELIST(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send341Inviting(const Client &client, const std::string &invitedNick,
                     const Channel &channel) {
  std::string message = _341_RPL_INVITING(client.getNickname(), invitedNick,
                                          channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send353Namreply(const Client &client, const Channel &channel) {
  std::ostringstream nicknames;
  std::ostringstream chanNameWithSymbol;

  clientPMap chanOps = channel.getChannelOperators();
  clientPMap::const_iterator itEnd = chanOps.end();
  for (clientPMap::const_iterator it = chanOps.begin(); it != itEnd; ++it) {
    nicknames << CHAN_OP << it->second->getNickname() << " ";
  }
  clientPMap chanClients = channel.getChannelClients();
  itEnd = chanClients.end();
  for (clientPMap::const_iterator it = chanClients.begin(); it != itEnd; ++it) {
    if (chanOps.find(it->first) == chanOps.end())
      nicknames << it->second->getNickname() << " ";
  }
  chanNameWithSymbol << PUBLIC_CHAN << " " << channel.getNameWithPrefix();
  std::string message = _353_RPL_NAMREPLY(
      client.getNickname(), chanNameWithSymbol.str(), nicknames.str());
  sendNumericReply(client.getFd(), &message);
}

void send366Endofnames(const Client &client, const Channel &channel) {
  std::string message =
      _366_RPL_ENDOFNAMES(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

/*============================================================================*/
/*       Error messages                                                       */
/*============================================================================*/

void send400UnknownError(const Client &client, const std::string &keyword,
                         const std::string &content) {
  std::string message =
      _400_ERR_UNKNOWNERROR(client.getNickname(), keyword, content);
  sendNumericReply(client.getFd(), &message);
}

void send401NoSuchNick(const Client &client, const std::string &targetNick) {
  std::string message = _401_ERR_NOSUCHNICK(client.getNickname(), targetNick);
  sendNumericReply(client.getFd(), &message);
}

void send403NoSuchChannel(const Client &client, const std::string &chanName) {
  std::string message = _403_ERR_NOSUCHCHANNEL(client.getNickname(), chanName);
  sendNumericReply(client.getFd(), &message);
}

void send404CannotSendToChan(const Client &client, const Channel &channel) {
  std::string message = _404_ERR_CANNOTSENDTOCHAN(client.getNickname(),
                                                  channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send405TooManyChannels(const Client &client) {
  std::string message = _405_ERR_TOOMANYCHANNELS(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send407TooManyTargets(const Client &client) {
  std::string message = _407_ERR_TOOMANYTARGETS(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send409NoOrigin(const Client &client) {
  std::string message = _409_ERR_NOORIGIN(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send411NoRecipient(const Client &client, const std::string &command) {
  std::string message = _411_ERR_NORECIPIENT(client.getNickname(), command);
  sendNumericReply(client.getFd(), &message);
}

void send412NoTextToSend(const Client &client) {
  std::string message = _412_ERR_NOTEXTTOSEND(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send417InputTooLong(const Client &client) {
  std::string message = _417_ERR_INPUTTOOLONG(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send421UnknownCommand(const Client &client, const std::string &command) {
  std::string message = _421_ERR_UNKNOWNCOMMAND(client.getNickname(), command);
  sendNumericReply(client.getFd(), &message);
}

void send431NoNicknameGiven(const Client &client) {
  std::string message;
  message = _431_ERR_NONICKNAMEGIVEN(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send432ErroneusNickname(const Client &client,
                             const std::string &erroneusNick) {
  std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
  std::string message = _432_ERR_ERRONEUSNICKNAME(nick, erroneusNick);
  sendNumericReply(client.getFd(), &message);
}

void send433NickAlreadyInUse(const Client &client,
                             const std::string &nickInUse) {
  std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
  std::string message = _433_ERR_NICKNAMEINUSE(nick, nickInUse);
  sendNumericReply(client.getFd(), &message);
}

void send441UserNotInChannel(const Client &client,
                             const std::string &targetNick,
                             const Channel &channel) {
  std::string message = _441_ERR_USERNOTINCHANNEL(
      client.getNickname(), targetNick, channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send442NotOnChannel(const Client &client, const Channel &channel) {
  std::string message =
      _442_ERR_NOTONCHANNEL(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send443UserOnChannel(const Client &client, const std::string &invitedNick,
                          const Channel &channel) {
  std::string message = _443_ERR_USERONCHANNEL(
      client.getNickname(), invitedNick, channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send451NotRegistered(const Client &client) {
  std::string message = _451_ERR_NOTREGISTERED(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send461NeedMoreParams(const Client &client, const std::string &command) {
  std::string message = _461_ERR_NEEDMOREPARAMS(client.getNickname(), command);
  sendNumericReply(client.getFd(), &message);
}

void send462AlreadyRegistered(const Client &client) {
  std::string message = _462_ERR_ALREADYREGISTERED(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send464PasswdMismatch(const Client &client) {
  std::string message;
  message = _464_ERR_PASSWD_MISMATCH(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send471ChannelIsFull(const Client &client, const Channel &channel) {
  std::string message =
      _471_ERR_CHANNELISFULL(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send472UnknownMode(const Client &client, const std::string &modeChar) {
  std::string message = _472_ERR_UNKNOWNMODE(client.getNickname(), modeChar);
  sendNumericReply(client.getFd(), &message);
}

void send473InviteOnlyChan(const Client &client, const Channel &channel) {
  std::string message = _473_ERR_INVITEONLYCHAN(client.getNickname(),
                                                channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send474BannedFromChan(const Client &client, const Channel &channel) {
  std::string message = _474_ERR_BANNEDFROMCHAN(client.getNickname(),
                                                channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send475BadChannelKey(const Client &client, const Channel &channel) {
  std::string message =
      _475_ERR_BADCHANNELKEY(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send476BadChanMask(const Client &client,
                        const std::string &chanNameWithBadMask) {
  std::string message =
      _476_ERR_BADCHANMASK(client.getNickname(), chanNameWithBadMask);
  sendNumericReply(client.getFd(), &message);
}

void send481NoPrivileges(const Client &client) {
  std::string message = _481_ERR_NOPRIVILEGES(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send482ChanOPrivsNeeded(const Client &client, const Channel &channel) {
  std::string message = _482_ERR_CHANOPRIVSNEEDED(client.getNickname(),
                                                  channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send501UmodeUnknownFlag(const Client &client) {
  std::string message = _501_ERR_UMODEUNKNOWNFLAG(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send502UserDontMatch(const Client &client) {
  std::string message = _502_ERR_USERDONTMATCH(client.getNickname());
  sendNumericReply(client.getFd(), &message);
}

void send525InvalidKey(const Client &client, const Channel &channel) {
  std::string message =
      _525_ERR_INVALIDKEY(client.getNickname(), channel.getNameWithPrefix());
  sendNumericReply(client.getFd(), &message);
}

void send696InvalidModeParam(const Client &client,
                             const std::string &chanNameWithPrefix,
                             const std::string &modeCharWithPrefix,
                             const std::string &param) {
  std::string message = _696_ERR_INVALIDMODEPARAM(
      client.getNickname(), chanNameWithPrefix, modeCharWithPrefix, param);
  sendNumericReply(client.getFd(), &message);
}

/*============================================================================*/
/*       Unit - numeric replies                                               */
/*============================================================================*/

#ifdef TESTNUMERICR
void testAllNumericReplies(const std::string &serverStartTime,
                           const Client &client, const std::string &command,
                           const std::string &targetNick) {
  int fd = client.getFd();
  Client testClient(fd, client.getIp(), client.getHostName());
  testClient.setNickname("testClient");
  testClient.setUserName("testUser");
  testClient.setRealName("Test User");
  std::string nick = testClient.getNickname();
  std::string user = testClient.getUserName();
  std::string host = testClient.getHostName();

  Channel testChannel("testChannel");
  Client testOp(4, "127.0.0.1", "localhost");
  testOp.setNickname("testOp");
  testChannel.addClientToChannelMap(&testClient);
  testChannel.addOperator(&testOp);
  Client testNonOPClient(42, "127.0.0.1", "localhost");
  testNonOPClient.setNickname("testNonOPClient");
  testChannel.addClientToChannelMap(&testNonOPClient);
  testChannel.activateLimitMode(42, testClient);
  testChannel.setTopic("This is the topic of a test channel", "Author");
  Channel invitedChannel("testInvited");
  invitedChannel.setTopic("This is a test channel", "Author");
  Channel kModeChannel("kModeChannel");
  kModeChannel.activateKeyMode("password", client);

  std::string testBegin =
      FROM_SERVER + " NOTICE " + nick + " :\n" +
      "=========== This is a Numeric replies Test ===========\n";
  std::string testEnd =
      FROM_SERVER + " NOTICE " + nick + " :\n" +
      "==================== End of Test =====================\n\n";

  send(fd, testBegin.c_str(), testBegin.size(), 0);
  /* Welcome */
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, serverStartTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick);
  /* User */
  send352Whoreply(client, testOp, testChannel);
  send315EndOfWho(client, testChannel);
  /* Channel */
  send321Liststart(fd, nick);
  send322List(fd, nick, testChannel);
  send323Listend(fd, nick);
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
  send432ErroneusNickname(client, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  send433NickAlreadyInUse(client, "nickInUse");
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
  send476BadChanMask(client, "&testChannel");
  send481NoPrivileges(client);
  send482ChanOPrivsNeeded(client, kModeChannel);
  send501UmodeUnknownFlag(client);
  send525InvalidKey(client, kModeChannel);
  send696InvalidModeParam(client, testChannel.getNameWithPrefix(), "+m",
                          "testParam");
  send(fd, testEnd.c_str(), testEnd.size(), 0);
}
#endif
