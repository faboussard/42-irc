/* MODE

 

MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
5
ft_irc Internet Relay Chat
· l: Set/remove the user limit to channel


If the client cannot join this channel because they would be over their limit, they will receive an ERR_TOOMANYCHANNELS (405) reply and the command will fail.

*/