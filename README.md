# logbot
Simple IRC bot, that logs the conversation. Written in C.

It writes the logs into a file, but it can also send it to users, at signing in, using a DCC connection.
It uses MySQL to store the file cursor when an user logs off or logs on.
The users must have registered (/msg <Bot_Nickname> help) for more infos.
