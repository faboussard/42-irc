/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   asciiCats.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 18:20:53 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/02 10:21:30 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_ASCIICATS_HPP_
#define INCLUDES_ASCIICATS_HPP_

// HELLO
#define BOT_HELLO1 "  /\\_/\\"
#define BOT_HELLO2 " ( o.o )"
#define BOT_HELLO3 \
  "─ U───U────────────────────────────────────────────────────────"
#define BOT_HELLO4 "    Hi there! I'm KawaiiBot, How can I brighten your day?"
#define BOT_HELLO5 \
  "────────────────────────────────────────────────────────── ♥ ──"
#define BOT_HELLO6 "!JOKE 🤣Feeling down? I've got a dad joke just for you."
#define BOT_HELLO7 "!INSULTME 😈In the mood for sass? Let me roast you (with love)"
#define BOT_HELLO8 \
  "!ADVICE 👼Looking for wisdom? I'll share a thoughtful advice"
#define BOT_HELLO9 \
  "!WEATHER <city name> 🌤️Want tomorrow's forecast? Just ask!"
#define BOT_HELLO10 "!RANDOM 🎲Feeling bored? Let's spice it up with something fun."
#define BOT_HELLO                                              \
  {BOT_HELLO1, BOT_HELLO2, BOT_HELLO3, BOT_HELLO4, BOT_HELLO5, \
  BOT_HELLO6, BOT_HELLO7, BOT_HELLO8, BOT_HELLO9, BOT_HELLO10}

// JOKE
#define JOKE_CAT_1 "     /\\_/\\"
#define JOKE_CAT_2 "    (• o •)    Haha! What a joke!"
#define JOKE_CAT_3 "     > ^ < 🐾"
#define JOKE_CAT {JOKE_CAT_1, JOKE_CAT_2, JOKE_CAT_3}

// ADVICE
#define ADVICE_CAT_1 "     /\\_/\\"
#define ADVICE_CAT_2 "    ( o.o )   Here's some wisdom:"
#define ADVICE_CAT_3 "     > ^ <"
#define ADVICE_CAT_4 "     || ||       *meow*"
#define ADVICE_CAT {ADVICE_CAT_1, ADVICE_CAT_2, ADVICE_CAT_3, ADVICE_CAT_4}

// INSULTME
#define INSULTME_CAT_1 "     /\\_/\\"
#define INSULTME_CAT_2 "    (>_< )   That was uncalled for!"
#define INSULTME_CAT_3 "     > ^ <"
#define INSULTME_CAT_4 "    (    )/   🐾"
#define INSULTME_CAT \
  {INSULTME_CAT_1, INSULTME_CAT_2, INSULTME_CAT_3, INSULTME_CAT_4}

// WEATHER
#define SUNNY_CAT_1 "     /\\_/\\"
#define SUNNY_CAT_2 "    ( ^_^ )   Gonna be a sunny day, full of smiles!"
#define SUNNY_CAT_3 "     > ^ < 🐾"
#define SUNNY_CAT {SUNNY_CAT_1, SUNNY_CAT_2, SUNNY_CAT_3}

#define CLOUDY_CAT_1 "     /\\_/\\"
#define CLOUDY_CAT_2 "    ( -_-)   It'll be a bit gloomy tomorrow..."
#define CLOUDY_CAT_3 "     > ^ < ☁️"
#define CLOUDY_CAT {CLOUDY_CAT_1, CLOUDY_CAT_2, CLOUDY_CAT_3}

#define FOGGY_CAT_1 "     /\\_/\\"
#define FOGGY_CAT_2 "    ( o.o )   Will be foggy... Who turned off the lights?"
#define FOGGY_CAT_3 "     > ^ <"
#define FOGGY_CAT {FOGGY_CAT_1, FOGGY_CAT_2, FOGGY_CAT_3}

#define RAINY_CAT_1 "     /\\_/\\"
#define RAINY_CAT_2 "    ( T_T )   Gonna rain tomorrow...!"
#define RAINY_CAT_3 "     > ^ < 🐾"
#define RAINY_CAT {RAINY_CAT_1, RAINY_CAT_2, RAINY_CAT_3}

#define SNOWY_CAT_1 "     /\\_/\\"
#define SNOWY_CAT_2 "    ( >_< )   Will be snowy, Brrr!"
#define SNOWY_CAT_3 "     > ^ < ❄️"
#define SNOWY_CAT {SNOWY_CAT_1, SNOWY_CAT_2, SNOWY_CAT_3}

#define THUNDER_CAT_1 "     /\\_/\\"
#define THUNDER_CAT_2 "    ( o_o )   Watch out! Thunderstorms are coming."
#define THUNDER_CAT_3 "     > ^ < ⚡"
#define THUNDER_CAT {THUNDER_CAT_1, THUNDER_CAT_2, THUNDER_CAT_3}

#define FROSTY_CAT_1 "     /\\_/\\"
#define FROSTY_CAT_2 "    ( >_< )   Frosty morning tomorrow!"
#define FROSTY_CAT_3 "     > ^ < ❄️"
#define FROSTY_CAT {FROSTY_CAT_1, FROSTY_CAT_2, FROSTY_CAT_3}

#define UNKNOWN_WEATHER_CAT_1 "     /\\_/\\"
#define UNKNOWN_WEATHER_CAT_2 "    ( o_O )   Hmm... I’m not sure what’s coming"
#define UNKNOWN_WEATHER_CAT_3 "     > ^ < ❓"
#define UNKNOWN_WEATHER_CAT \
  {UNKNOWN_WEATHER_CAT_1, UNKNOWN_WEATHER_CAT_2, UNKNOWN_WEATHER_CAT_3}

// DEFAULT / UNKNOWN
#define DEFAULT_CAT_1 "     /\\_/\\"
#define DEFAULT_CAT_2 "    ( o.o )   I'm confused!"
#define DEFAULT_CAT_3 "     > ^ < 🐾"
#define DEFAULT_CAT_4 " Try !HELLO for a list of commands!"
#define DEFAULT_CAT {DEFAULT_CAT_1, DEFAULT_CAT_2, DEFAULT_CAT_3, DEFAULT_CAT_4}

// TIMEOUT
#define TIMEOUT_CAT_1 "    /\\_/\\"
#define TIMEOUT_CAT_2 "   ( -_- )   Zzz... Sorry, temporarily unavailable."
#define TIMEOUT_CAT_3 "    > ^ <"
#define TIMEOUT_CAT {TIMEOUT_CAT_1, TIMEOUT_CAT_2, TIMEOUT_CAT_3}

#endif  // INCLUDES_ASCIICATS_HPP_
