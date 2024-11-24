PART 3

# 💻 **FD**: 

## 🛑 Pour fermer proprement des ports : 
- Close de **1 à 1024** car cest le nombre de fd dun systeme de base linux. command limit pour trouver si besoin

---

# se connecter a inspIRCd et hexchat
```bash

➜  inspircd-3.14.0 ./bin/inspircd  --config ./conf/examples/inspircd.conf.example --nofork
InspIRCd - Internet Relay Chat Daemon
See /INFO for contributors & authors

InspIRCd Process ID: 83146

Loading core modules ....................
InspIRCd is now running as 'penguin.omega.example.org'[422] with 1048576 max open sockets
```

ca va tourner sur local host et on pourra y acceder en desactivant le ssl
---


# **Plan**
Gestion des connexions et de l'authentification (Personne A)

Implémenter la gestion des connexions des clients (écoute des connexions sur le port).
Authentifier les utilisateurs (nom d'utilisateur, mot de passe).
Gérer la création et la destruction des sockets pour chaque client.
Gestion des canaux et des utilisateurs (Personne B)

Implémenter la gestion des canaux (création, suppression, rejoindre/quitter un canal).
Gérer les utilisateurs dans chaque canal (ajouter/supprimer des utilisateurs).
Mettre en œuvre les rôles des utilisateurs (opérateurs vs utilisateurs réguliers).
Communication et gestion des messages (Personne C)

Implémenter l'envoi et la réception des messages privés entre utilisateurs.
Gérer l'envoi et la réception des messages de groupe dans les canaux.
Assurer la transmission des messages entre clients de manière asynchrone.


# **General**

- IRC n'est pas limité à une seule norme de codage comme ASCII ou UTF-8.
- certains octets ont des valeurs spécifiques réservées pour des codes de contrôle. Ces codes de contrôle sont utilisés pour délimiter les messages, c'est-à-dire pour indiquer où commence et où se termine un message. Par exemple, des caractères comme newline (nouvelle ligne) peuvent être utilisés pour marquer la fin d'un message.

## 🔗 **Server**:
- Les serveurs sont identifiés de manière unique par leur nom, avec une longueur maximale de **63 caractères**.
- Chaque serveur a un identifiant unique à l'échelle du réseau (dont le format dépend du type de client) et le serveur qui a introduit le client.

---

## 👮 **Operators Client**:
### Pouvoirs spéciaux :
- Les opérateurs peuvent effectuer des tâches de réseau de base telles que déconnecter et reconnecter des serveurs pour éviter l'utilisation prolongée de mauvaises routes réseau.
- Un pouvoir plus controversé des opérateurs est la capacité de **forcer la déconnexion** d'un utilisateur du réseau connecté en fermant la connexion entre n'importe quel client et serveur.

---

## 🛠️ **Service Client**:
- Chaque client doit choisir un surnom unique, ou **"nick"**, d'une longueur maximale de **9 caractères**.
- Une fois connecté, le client peut communiquer individuellement avec d'autres utilisateurs, ou exécuter des commandes pour interroger l'état du serveur (par exemple, obtenir une liste des utilisateurs connectés ou obtenir des détails supplémentaires sur un surnom spécifique).
- Les **clients service** ont un accès plus limité aux fonctions de discussion du protocole, mais peuvent avoir accès à des **données privées des serveurs**.

---

## 📢 **Channels**:
- Le canal est créé **implicitement** lorsque le premier client le rejoint, et il cesse d'exister lorsque le dernier client le quitte.
- Pendant que le canal existe, n'importe quel client peut se référer à lui en utilisant son **nom**.
- Les noms de canaux sont des chaînes commençant par un **'&'** ou **'#'** et peuvent avoir une longueur allant jusqu'à **200 caractères**.
- Restrictions de nom de canal : 
   - Le nom ne doit pas contenir d'espaces (`' '`), de contrôle G (`^G` ou ASCII 7), ou de virgule (`,`).
   
### 🔒 **Local Channel**:
- Ces canaux sont marqués par le premier caractère étant un **'!'**, et seuls les clients du serveur où ils existent peuvent les rejoindre.

### **Actions**:
    JOIN
    -  If the channel doesn't exist prior
   to joining, the channel is created and the creating user becomes a
   channel operator. 
- if the channel is invite-only, (+i),
   then you may only join if invited
   -  a limit of ten (10)
   channels is recommended as being ample for both experienced and
   novice users.

### **Channel Operators**
  The channel operator (also referred to as a "chop" or "chanop") on a
   given channel is considered to 'own' that channel. 
   The commands which may only be used by channel operators are:

        KICK    - Eject a client from the channel
        MODE    - Change the channel's mode
        INVITE  - Invite a client to an invite-only channel (mode +i)
        TOPIC   - Change the channel topic in a mode +t channel

    -  A channel operator is identified by the '@' symbol next to their
   nickname whenever it is associated with a channel (ie replies to the
   NAMES, WHO and WHOIS commands).

---

## 👤 **User Client**:
### Utilise HexChat
- Les clients utilisateur sont généralement des programmes fournissant une interface **texte** pour communiquer via IRC. Ce type de client est souvent appelé **"utilisateurs"**.

---

## 🔍 **Voc**:
- **Spanning Tree Protocol (STP)** : Un protocole de réseau utilisé pour éviter les boucles dans les réseaux locaux (LAN). Il crée une **topologie arborescente** qui connecte tous les commutateurs tout en désactivant les liens redondants.
- Un **tunnel** est un mécanisme qui permet de transmettre des données d'un protocole à un autre en encapsulant ces données.
 - **WebSocket** est un protocole de communication qui permet d'établir une connexion bidirectionnelle persistante entre un client (comme un navigateur ou une application) et un serveur. Contrairement à HTTP, où le client doit envoyer une requête pour obtenir une réponse du serveur, WebSocket permet au serveur d'envoyer des données au client à tout moment, ce qui est idéal pour des applications en temps réel.
 - **IP** = Internet protocol

 - **Sockets** : Un socket est un point de terminaison de communication bidirectionnelle entre deux programmes qui s'exécutent sur un réseau. Il est utilisé pour la communication entre les processus sur la même machine ou sur différentes machines dans un réseau.

 - **Active socket** : Un socket actif est un socket qui est en cours d'utilisation pour la communication. Il est ouvert et prêt à envoyer ou à recevoir des données.

 - **Passive socket** : Un socket passif est un socket qui est en attente de demandes de connexion entrantes. Il est utilisé pour écouter les connexions entrantes et accepter les demandes de connexion.

 Active Socket (IRC Client):

**An active socket in IRC** represents the client-side connection to an IRC server. The IRC client initiates communication by connecting to the IRC server using a TCP/IP connection. Once connected, the client can send commands and messages to the server and receive responses. The IRC client socket handles user input, sends messages to the server, and processes server responses.
Passive Socket (IRC Server):

**A passive socket in IRC** represents the server-side listening socket that accepts incoming connections from IRC clients. The IRC server listens for incoming connections on a specific port, When a connection request is received from an IRC client, the server socket accepts the connection, creating a new socket for communication with that client. The IRC server socket manages multiple client connections simultaneously, handling commands and messages from each connected client and broadcasting messages to all clients as needed.


---

### 🖥️ Référence :
- [RFC 2810](https://datatracker.ietf.org/doc/html/rfc2810)

---

## **Beej’s Guide to Network Programming !** 👋
2.1 Two Types of Internet Sockets
- “**SOCK_STREAM** = two-way connected communication streams : envoie les packets dans l'ordre et sans perte
- **SOCK_DGRAM** = connectionless sockets” : envoie les packets sans garantie de réception
- **Data Encapsulation** =  a packet is born, the packet is wrapped (“encapsulated”) in a header (and rarely a footer) by the first
protocol (say, the TFTP protocol), then the whole thing (TFTP header included) is encapsulated again by
the next protocol (say, UDP), then again by the next (IP), then again by the final protocol on the hardware
(physical) layer (say, Ethernet).

Layered Network Model
Application
• Presentation
• Session
• Transport
• Network
• Data Link
• Physical

Layered Network Model with unix : 
• Application Layer (telnet, ftp, etc.)
• Host-to-Host Transport Layer (TCP, UDP)
• Internet Layer (IP and routing)
• Network Access Layer (Ethernet, wi-fi, or whatever)

3.1 IP Addresses, versions 4 and 6

loopback address = “this machine I’m running on now” =  127.0.0.1 = localhost
Adresse IP privée : Utilisée pour la communication sur un réseau local (local AreaNetwork).
Adresse IP publique : Utilisée pour la communication sur Internet.

Think of the IP address as the street address of a hotel, and the port number as the room number. 

3.4.1 Private (Or Disconnected) Networks

often times, the firewall translates “internal” IP addresses to “external” (that everyone else in the
world knows) IP addresses using a process called Network Address Translation, or NAT

9.17 poll()
This function is very similar to select() in that they both watch sets of file descriptors for events, such
as incoming data ready to recv(), socket ready to send() data to, out-of-band data ready to recv(),
errors, etc.
The poll() function is used to monitor changes in the status of file descriptors. It blocks until an event occurs on one or more of the monitored file descriptors, or until the specified timeout expires.

raccourci cpplint : ctrl shift i

---

NExT PR 
**Summary:**
_**Implemented Features:**_


---
PR pour feature mode : 

**1 Création des vecteurs modes et paramètres** :
Les arguments sont séparés en deux parties : un vecteur modes pour les caractères de mode (+ ou - suivis de i, t, k, o, l) et un vecteur parameters pour leurs valeurs associées. On utilise des boucles pour parcourir la chaîne de modes et associer chaque paramètre à son mode si nécessaire.

**2 Vérification des chaînes de modes :**
La fonction checkModeString valide la syntaxe des modes. Elle vérifie que chaque caractère correspond à un mode autorisé (i, t, k, o, l) et que les signes + ou - précèdent les modes correctement. Si une erreur est détectée (mode invalide ou mal formé), une réponse d’erreur est envoyée au client.

**3 Validation des arguments des modes :**
La fonction checkArguments s'assure que chaque mode nécessitant un argument (comme +k pour le mot de passe ou +l pour la limite d'utilisateurs) reçoit une valeur appropriée. Si un argument est absent ou invalide (par exemple, une valeur non numérique pour +l), une erreur ERR_NEEDMOREPARAMS ou un message explicite est renvoyé au client.

**4 Application des modes :**
La fonction switchMode applique les changements de modes au canal en fonction des arguments valides. Les fonctions spécifiques des objets Channel sont appelées pour activer/désactiver les modes (activateInviteOnlyMode, updateKey, etc.). Les erreurs comme ERR_NOSUCHNICK (utilisateur inexistant pour +o) ou des indices invalides sont également gérées ici.

**5 Retour d'état et logs :**
Les modifications effectuées sont confirmées par des messages MODE envoyés aux clients. Les logs internes (activés avec DEBUG) permettent de suivre chaque étape du traitement : analyse des arguments, validation des modes, et application des changements au canal.

**Tests for Mode**

**Commandes qui fonctionnent**
MODE #channel +i -tk +o user1 
Le canal est en mode "invite-only" (+i), sans sujet (-t), sans mdp (-k) et l'utilisateur user1 est opérateur (+o).

`MODE #channel +ilkto 10 key user`
canal est en mode "invite-only" (+i), avec un sujet restreint (+t), un mot de passe de 10 caractères (+k key), et une limite d'utilisateurs (+l), avec user comme utilisateur invité.
`MODE #channel +k password `
Le canal est protégé par un mot de passe (+k password).
`MODE #channel +o user1 -o user1`
er1 est ajouté comme opérateur (+o), puis enlevé (-o).
`MODE #channel +o user1 +o user2 +o user3 -o user4 +i -t `
er1, user2, et user3 deviennent opérateurs (+o), user4 est enlevé comme opérateur (-o), le canal devient "invite-only" (+i), et le sujet est supprimé (-t).
`MODE #channel +o user1 +o user2 +o user3 -o user4 `
er1, user2, et user3 deviennent opérateurs (+o), et user4 est enlevé comme opérateur (-o).

MODE #channel +itk +o user1 
Le canal est en mode "invite-only" (+i), avec un sujet restreint (+t), sans mot de passe (-k), et l'utilisateur user1 est opérateur (+o). cependant pour simplifier, dans notre irc, la commande prendra user1 comme mdp car il traite +k et pas -k (alors que dans le vrai irc, s'il n y a pas de key,+k devient -k). => ne fonctionne pas chez nous

MODE #channel +i user1 -tk +o => user1 est utilisé pour correspondre à +o

**Commandes en erreur (adapted numeric replies)**
/mode
MODE :Not enough parameter
MODE #channel +i +tk +o user1 
MODE #channel +o =>missing argument
MODE #channel -o=>missing argument
MODE #channel +k=>missing argument
MODE #channel +l=>missing argument
MODE #channel +m =>m : unknown mode
MODE #channel ++ => + : unknown mode
/mode https://github.com/faboussard/42-irc/issues/1 +i =>#1:You're not a channel operator (si pas op)
MODE #channel +imst =>bloqué car ne connait pas m

j'ai également fait une petit modif dans broadcast channel car si on changeait le topic on ne voyait pas son changement car je limitais le broadcast message : seul les receveurs voyaient la modif. 
jai ajouté une condition que sur privmsg pour que les messages ne soit pas dupliquées, mais que les autres actions soient bien reçues par tous.


```
void Server::broadcastInChannel(const Client &sender, const Channel &channel,
                                const std::string &command,
                                const std::string &content) {
  std::string message = ":" + sender.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " :" + content + "\r\n";
  const clientPMap &allClients = channel.getChannelClients();
  clientPMap::const_iterator itEnd = allClients.end();
  for (clientPMap::const_iterator it = allClients.begin(); it != itEnd; ++it) {
    if (it->first == sender.getFd() && command == "PRIVMSG")
      continue;
    it->second->receiveMessage(message);
  }
}
```



DEBUG GDB


gdb ./bin/ircserv -tui
b function_name + tab
run 6667 pass 
say no to debuging info 
connect to NC
enter joining info ( pass, nick ...) 
enter commands : if i want to process it i type c 
n to continue 
enter + emptyline will process the previous command 
 





