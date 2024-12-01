# 42-irc

## Usage:

```
make
./ircserv 6667 pass
```

## Usage for bot calling:

We created a kawaiibot which can be called on when you need it.

You have to call it with the following syntax:

in a terminal:
```
make bot
./ircbot 6667 pass
```

in HexChat:
```
/privmsg KawaiiBot :!command
```

The commands are the following:
- !hello
- !joke
- !advice
- !insultMe
- !random
- !weather

To use `!weather` command, an API key for [Weather API](https://www.weatherapi.com/) is required.
The key should be set in `.env` file as `WEATHER_API_KEY=key` at the root of the project.
