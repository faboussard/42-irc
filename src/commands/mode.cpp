#include <map>
#include <string>
#include <utility>
#include <sstream>

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

#define PLUS_CHAR '+'
#define MINUS_CHAR '-'

void Server::switchMode(Client *client, const std::string &channelName,
                        const stringVector &modeStrings,
                        const stringVector &argumentVector) {
  Channel *channel = &_channels[channelName.substr(1)];
  size_t argumentIndex = 0;
  std::string mode;
  std::string modeChanges;
  char currentSign = 0;

  for (size_t i = 0; i < modeStrings.size(); ++i) {
    const std::string &modeString = modeStrings[i];
    bool plusMode = (modeString[0] == '+');

    for (size_t j = 1; j < modeString.size(); ++j) {
      const char &c = modeString[j];
      if (c == 'i' || c == 't') {
        if (plusMode) {
          (c == 'i') ? channel->activateInviteOnlyMode()
                     : channel->activateTopicOpsOnlyMode();
        } else {
          (c == 'i') ? channel->deactivateInviteOnlyMode()
                     : channel->deactivateTopicOpsOnlyMode();
        }
        if (currentSign != (plusMode ? '+' : '-')) {
          currentSign = (plusMode ? '+' : '-');
          mode += currentSign;
        }
        mode += c;
      }
      else if (c == 'k' || c == 'l') {
        if (plusMode) {
          if (argumentIndex >= argumentVector.size()) {
            send461NeedMoreParams(*client, "MODE +" + std::string(1, c));
            return;
          }
          const std::string &argument = argumentVector[argumentIndex++];
          if (c == 'k') {
            channel->activateKeyMode(argument, *client);
            channel->updateKey(argument);
          } else if (c == 'l') {
            if (!isNumeric(argument) || std::atoi(argument.c_str()) > 999) {
              sendNotice(*client,
                         "MODE +l argument must be a numeric value below 999.");
              return;
            }
            channel->activateLimitMode(std::atoi(argument.c_str()));
          }
          if (currentSign != (plusMode ? '+' : '-')) {
            currentSign = (plusMode ? '+' : '-');
            mode += currentSign;
          }
          mode += c;
          modeChanges += " " + argument;
        } else {
          (c == 'k') ? channel->deactivateKeyMode()
                     : channel->deactivateLimitMode();
          if (currentSign != (plusMode ? '+' : '-')) {
            currentSign = (plusMode ? '+' : '-');
            mode += currentSign;
          }
          mode += c;
        }
      } else if (c == 'o') {
        if (argumentIndex >= argumentVector.size()) {
          send461NeedMoreParams(*client, "MODE +o/-o");
          return;
        }
        Client *targetClient =
            findClientByNickname(argumentVector[argumentIndex]);
        if (!targetClient) {
          send401NoSuchNick(*client, argumentVector[argumentIndex]);
          return;
        }
        (plusMode) ? channel->addOperator(targetClient)
                   : channel->removeOperator(targetClient);
        if (currentSign != (plusMode ? '+' : '-')) {
          currentSign = (plusMode ? '+' : '-');
          mode += currentSign;
        }
        mode += c;
        modeChanges += " " + argumentVector[argumentIndex];
        ++argumentIndex;
      } else {
        sendNotice(*client, "Unknown mode: " + std::string(1, c));
        continue;
      }
    }
  }
  std::string tosend = mode + modeChanges;
  if (!tosend.empty()) {
    broadcastInChannelAndToSender(*client, *channel, "MODE", tosend);
#ifdef DEBUG
    std::ostringstream oss;
    oss << "Mode changes: " << modeChanges;
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
#endif
  }
}

char Server::checkModeArguments(const stringVector &modeStrings,
                                const stringVector &arguments) {
  std::map<char, bool> modesRequiringArgument;
  modesRequiringArgument['k'] = true;
  modesRequiringArgument['o'] = true;
  modesRequiringArgument['l'] = true;
  modesRequiringArgument['t'] = false;
  modesRequiringArgument['i'] = false;

  size_t argumentIndex = 0;
  bool plusMode = false;

  for (size_t i = 0; i < modeStrings.size(); ++i) {
    const std::string &modeString = modeStrings[i];

    for (size_t j = 0; j < modeString.size(); ++j) {
      char c = modeString[j];

      if (c == PLUS_CHAR) {
        plusMode = true;
      } else if (c == MINUS_CHAR) {
        plusMode = false;
      }
#ifdef DEBUG
      {
        std::ostringstream oss;
        oss << "Mode: " << (plusMode ? "+" : "-") << c << " | "
            << "Argument: "
            << (argumentIndex < arguments.size() ? arguments[argumentIndex]
                                                 : "N/A")
            << " | "
            << "booleen:" << modesRequiringArgument[c];
        Log::printLog(DEBUG_LOG, COMMAND, oss.str());
      }
#endif
      if ((plusMode && modesRequiringArgument[c]) ||
          (modesRequiringArgument[c] && c == 'o')) {
        if (argumentIndex >= arguments.size()) {
          return c;
        }
        ++argumentIndex;
      }
    }
  }
  return 0;
}

std::string Server::checkModeString(const stringVector &modestringToCheck) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "modestringToCheck.size(): " << modestringToCheck.size();
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (modestringToCheck.empty()) {
    return "";
  }
  const std::string validModes = "itkol";
  for (size_t i = 0; i < modestringToCheck.size(); ++i) {
    const std::string &mode = modestringToCheck[i];
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "mode: " << mode;
      Log::printLog(DEBUG_LOG, COMMAND, oss.str());
    }
#endif
    if (mode[0] != PLUS_CHAR && mode[0] != MINUS_CHAR) return mode;
    for (size_t j = 1; j < mode.size(); ++j) {
      if (validModes.find(mode[j]) == std::string::npos) {
#ifdef DEBUG
        {
          std::ostringstream oss;
          oss << "Invalid Mode: " << mode[j];
          Log::printLog(DEBUG_LOG, COMMAND, oss.str());
        }
#endif
        return mode;
      }
    }
  }
  return "";
}

bool Server::isChannelValid(int fd, const std::string &channelName) {
  if (channelName.empty()) {
    send461NeedMoreParams(_clients[fd], "MODE");
    return (false);
  }
  if (channelName[0] != REG_CHAN) {
    send502UserDontMatch(_clients[fd]);
    return (false);
  }
  if (_channels.find(channelName.substr(1)) == _channels.end()) {
    send403NoSuchChannel(_clients[fd], channelName);
    return (false);
  }
  return (true);
}

void Server::mode(int fd, const std::string &arg) {
  const Client &client = _clients.at(fd);
  std::istringstream iss(arg);
  std::string channelName;
  iss >> channelName;
  if (isChannelValid(fd, channelName) == false) return;
  const Channel &channelObj = _channels[channelName.substr(1)];
  std::string remainingArgs;
  std::getline(iss, remainingArgs);
  stringVectorPair modestringAndArguments = parseModeParams(remainingArgs);
  if (modestringAndArguments.first.empty()) {
    send324Channelmodeis(client, channelObj);
    return;
  }
  if (channelObj.isOperator(fd) == false) {
    send482ChanOPrivsNeeded(client, channelObj);
    return;
  }
  stringVector modestringVector = modestringAndArguments.first;
  stringVector modeArgumentsVector = modestringAndArguments.second;
  std::string errorModeString = checkModeString(modestringVector);
  if (!errorModeString.empty()) {
    send472UnknownMode(client, errorModeString);
    return;
  }
  char errorModeArguments =
      checkModeArguments(modestringVector, modeArgumentsVector);
  if (errorModeArguments != 0) {
    send696InvalidModeParam(client, channelName,
                            "Mode:", std::string(1, errorModeArguments));
    return;
  }
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "ENTER SWITCH MODE FUNCTION";
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  switchMode(&_clients[fd], channelName, modestringVector, modeArgumentsVector);
}

stringVectorPair Server::parseModeParams(const std::string &arg) {
  if (arg.size() < 2) return std::make_pair(stringVector(), stringVector());
  stringVector keyVector;
  stringVector valueVector;
  stringVectorPair list;
  stringVector tokens = basicSplit(arg);
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (!tokens[i].empty() &&
        (tokens[i][0] == PLUS_CHAR || tokens[i][0] == MINUS_CHAR)) {
      keyVector.push_back(tokens[i]);
    } else {
      valueVector.push_back(tokens[i]);
    }
  }
#ifdef DEBUG
  {
    std::ostringstream before, afterKey, afterValue;
    before << "arg: Before split and process: " << arg;
    afterKey << "keyVector: ";
    for (size_t i = 0; i < keyVector.size(); ++i)
      afterKey << keyVector[i] << "|";
    afterValue << "valueVector: ";
    for (size_t i = 0; i < valueVector.size(); ++i)
      afterValue << valueVector[i] << "|";
    Log::printLog(DEBUG_LOG, COMMAND, before.str());
    Log::printLog(DEBUG_LOG, COMMAND, afterKey.str());
    Log::printLog(DEBUG_LOG, COMMAND, afterValue.str());
  }
#endif
  return std::make_pair(keyVector, valueVector);
}