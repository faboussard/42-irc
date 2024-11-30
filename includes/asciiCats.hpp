/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   asciiCats.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 18:20:53 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/30 23:16:10 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_ASCIICATS_HPP_
#define INCLUDES_ASCIICATS_HPP_

//MENU
#define BOT_MENU1 "  /\\_/\\"
#define BOT_MENU2 " ( o.o )"
#define BOT_MENU3 \
  "─ U───U────────────────────────────────────────────────────────"
#define BOT_MENU4 "         Hello! I'm KawaiiBot, what can I do for you?"
#define BOT_MENU5 "────────────────────────────────────────────────────────── ♥ ──"
#define BOT_MENU6 "🤣 Feeling down? I'll lift you up with a dad joke. 👉!JOKE"
#define BOT_MENU7 "😈 Craving some sass? I can roast you. 👉!INSULTME"
#define BOT_MENU8 "👼 Need guidance? Let me share some wisdom with you. 👉!ADVICE"
#define BOT_MENU9 "🌤️ Wondering about the weather? Ask away. 👉!WEATHER <city name>"
#define BOT_MENU10 "🎲 Bored? Let's spice it up with something fun. 👉!RANDOM"
#define BOT_MENU {BOT_MENU1, BOT_MENU2, BOT_MENU3, BOT_MENU4, BOT_MENU5, BOT_MENU6, BOT_MENU7, BOT_MENU8, BOT_MENU9, BOT_MENU10}

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
#define INSULTME_CAT {INSULTME_CAT_1, INSULTME_CAT_2, INSULTME_CAT_3, INSULTME_CAT_4}

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
#define UNKNOWN_WEATHER_CAT {UNKNOWN_WEATHER_CAT_1, UNKNOWN_WEATHER_CAT_2, UNKNOWN_WEATHER_CAT_3}


// DEFAULT / UNKNOWN
#define DEFAULT_CAT_1 "     /\\_/\\"
#define DEFAULT_CAT_2 "    ( o.o )   I'm confused!"
#define DEFAULT_CAT_3 "     > ^ < 🐾"
#define DEFAULT_CAT_4 " Try !MENU for a list of commands!"
#define DEFAULT_CAT {DEFAULT_CAT_1, DEFAULT_CAT_2, DEFAULT_CAT_3, DEFAULT_CAT_4}

// TIMEOUT
#define TIMEOUT_CAT_1 "    /\\_/\\"
#define TIMEOUT_CAT_2 "   ( -_- )   Zzz... Sorry, temporarily unavailable."
#define TIMEOUT_CAT_3 "    > ^ <"
#define TIMEOUT_CAT {TIMEOUT_CAT_1, TIMEOUT_CAT_2, TIMEOUT_CAT_3}

#endif  // INCLUDES_ASCIICATS_HPP_