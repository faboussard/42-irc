# 💻 **FD**: 

## 🛑 Pour fermer proprement des ports : 
- Close de **1 à 1024**.

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

---

### 🖥️ Référence :
- [RFC 2810](https://datatracker.ietf.org/doc/html/rfc2810)

---

## **Au revoir !** 👋
