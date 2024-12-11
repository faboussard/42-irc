# Our bot implementation using API requests

This bot (named KawaiiBot) is designed to operate seamlessly as an IRC client, offering robust functionality for connection, interaction, and event handling.   

## Table of content
- [Bot Communication Flow](#bot-communication-flow)
- [Bot Features](#bot-features)
- [API Requests Handling](#api-requests-handling)

## Bot Communication Flow

To address the following constraints, communication with the IRC server is handled via sockets, while communication with the API server is performed by executing curl with popen and reading the output using the fgets function. All incoming messages are monitored using poll.

- C++98 (Third-party libraries are prohibited).
- Most API servers require HTTPS communication; HTTP communication does not yield the expected response.

![schma_bot](./bot.png)

### 1. Create a socket and connect to IRC Server

Create a socket to **communicate with the IRC Server** using `socket()`

### 2. Connect to IRC Server

Connect to the IRC server using a socket (connect()), then wait for the connection to be established using a temporary poll with a timeout of 10,000 milliseconds.

### 3. Listen to requests from IRC Server

Using another `poll`, the bot continuously listens for new requests from the IRC server sent as `PRIVMSG`.

### 4. Process requests

#### a. Validate the request

Determinate if the request received from the Serve is valid to trigger an API request.

#### b. Send an API request 

Construct a curl command to send a request.
Execute the command using popen and stores the file pointer and file descriptor in the  BotRequest structure object.   
Also add the corresponding file descriptor to a `poll` to monitor the response.

#### c. Wait for response from API server

The response for each request is monitored using poll.   
In case the set timeout is reached, an error message is sent to the client via IRC Server, and the request is removed.


#### d. Read the response from API server

When poll detects a response, the content is read using `fgets()`and parsed.
  
##### Parse the response
Some APIs may return a response in JSON format.

Response in JSON format might be parsed using string manipulation.
JSON format responses may consist of `headers` and `a body`. The headers and body are usually separated by a blank line.

Example of response (source: https://jsonapi.org/examples/):
```JSON

HTTP/1.1  200 OK
Content-Type: application/vnd.api+json

{
  "data": [{
  "type": "articles",
  "id": "1",
  "attributes": {
    "title": "JSON:API paints my bikeshed!",
    "body": "The shortest article. Ever.",
    "created": "2015-05-22T14:56:29.000Z",
    "updated": "2015-05-22T14:56:28.000Z"
    },
  "relationships": {
    "author": {
      "data": {"id": "42", "type": "people"}
      }
    }
  }],
  "included": [
    {
      "type": "people",
      "id": "42",
      "attributes": {
        "name": "John",
        "age": 80,
        "gender": "male"
      }
    }
  ]
}

```

> [!CAUTION]
>Don't forget to handle errors as connection failures, timeouts, or HTTP error codes.

#### f. Send back the response to IRC server

The bot sends the response to the client via the IRC server as a PRIVMSG.   
After sending the response, the process created by `popen()` should be closed using `fclose()`.

### 5. Close the socket

Before exiting from the program, close the sockets


### Important notes

**Buffer management**: Since responses from API server may be large, proper buffer management is required. If the response is too large to receive in one go, the bot needs to receive in multiple times.

---

## Bot Features

### 1. Secure and Reliable Authentication

At launch, the bot automatically attempts to authenticate with the IRC server using PASS, NICK, and USER commands. It includes timeout management using poll to ensure that authentication steps (password verification, nickname validation, and server registration) are handled efficiently.   
Handles common error scenarios, such as invalid passwords (ERR_PASSWD_MISMATCH) or nickname conflicts (ERR_NICKNAMEINUSE), with appropriate logging and error messages.  
    
### 2. Command-Based Interaction with KawaiiBot

```
/PRIVMSG KawaiiBot :!hello

  /\_/\
 ( o.o )
─ U───U────────────────────────────────────────────────────────
    Hi there! I'm KawaiiBot, How can I brighten your day?  
────────────────────────────────────────────────────────── ♥ ──  
!JOKE 🤣  Feeling down? I've got a dad joke just for you.  
!INSULTME 😈  In the mood for sass? Let me roast you (with love).  
!ADVICE 👼  Looking for wisdom? I'll share a thoughtful advice.  
!WEATHER <city name> 🌤️  Want tomorrow's forecast? Just ask!  
!RANDOM 🎲  Feeling bored? Let's spice it up with something fun.  
```

Bot brings a playful and interactive experience to our IRC server with its command-based functionality via `PRIVMSG`.   
Here's what makes it stand out:

- **!HELLO**   
When you greet KawaiiBot with `!hello`, it responds with an adorable ASCII cat (the project's mascot) to show supported commands for every mood.

- **!JOKE**   
A dad joke is delivered to lighten the mood and provide some humor.

- **!INSULTME**   
This command allows the bot to roast the user in a playful, sassy way, adding a bit of fun to the interaction.

- **!ADVICE**   
KawaiiBot shares some thoughtful advice, offering wisdom for any moment.

- **!WEATHER <city>**   
Provides the weather forecast for the given city, helping users plan their day. The default city is set to Lyon, so that it can reply even if clients put no argument. 

- **!RANDOM**   
A random response that adds variety and unpredictability to the interaction.


### 3. Playful Responses with ASCII Art

The bot enhances responses by including pretty cat ASCII art, as the cat serves as the mascot of the project, adding a fun and unique touch to interactions.

---

## API Requests Handling

In KawaiiBot, API requests are used to fetch data for various interactive commands, including jokes, insults, advice, and weather forecasts. It utilizes four key APIs:

- Joke: [Random dad joke](https://icanhazdadjoke.com/)
- Insult: [Evil Insult Generator](https://evilinsult.com)
- Advice: [Advice Slip JSON API](https://api.adviceslip.com)
- Weather: [weather api](https://www.weatherapi.com/)

### 1. Request Execution

Each API request is sent using the `popen(curl)` command, which opens a process for executing the cURL command.   
The responses are handled asynchronously with the poll system to wait for data, allowing the bot to continue processing other tasks without blocking.

### 2. Polling for API Responses

It uses a polling mechanism to monitor multiple file descriptors (FDs) simultaneously.   
This ensures that when data becomes available from any of the API requests, it is processed promptly. The `poll` function checks for new data and triggers the appropriate handler when the response is received.

### 3. Timeout Handling

To prevent the bot from waiting indefinitely for a response, the bot checks for timeouts.   
Each API request is given a specific timeout period, and if no response is received within that time, the bot closes the request, removes it from the polling list, and sends a "service temporary unavailable" message to clients.

