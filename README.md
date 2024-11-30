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
make run_bot SRV_PORT=6667 SRV_PASS=pass BOT_PORT=6668
```

in HexChat:
```
/privmsg KawaiiBot :!command
```

The commands are the following:
- !menu // to be eventually changed to hello
- !joke
- !advice
- !insultMe
- !random
