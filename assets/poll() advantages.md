Différence d'efficacité entre l'utilisation de poll() et les appels classiques comme recv() et send()

La différence d'efficacité entre l'utilisation de poll() et les appels classiques comme recv() et send() vient de la manière dont ces fonctions gèrent les sockets et les connexions réseau, en particulier dans les environnements avec plusieurs connexions simultanées. Voici pourquoi poll() est souvent considéré comme plus efficace et scalable dans ces cas-là :

1. Non-blocage et gestion simultanée des connexions
recv() et send() sont des appels bloquants par défaut. Cela signifie que si tu appelles recv() sur un socket et qu'il n'y a pas encore de données à lire, ton programme attendra (il sera bloqué) jusqu'à ce que des données soient disponibles. Cela peut être problématique si tu dois gérer plusieurs connexions à la fois, car tu ne peux pas surveiller plusieurs sockets en même temps avec un seul appel à recv() ou send().

poll(), en revanche, permet de surveiller plusieurs descripteurs de fichiers (y compris des sockets) simultanément. Il ne bloque pas tant qu'aucun des descripteurs surveillés n'est prêt pour l'opération que tu attends (lecture, écriture, etc.). Cela te permet de gérer efficacement plusieurs connexions en même temps, en ne bloquant qu'en attendant un événement sur n'importe lequel des descripteurs surveillés.

2. Scalabilité
Imaginons que tu aies 100 ou 1 000 connexions réseau ouvertes sur ton serveur. Si tu devais utiliser recv() et send() de manière classique, tu devrais faire une boucle sur toutes ces connexions, et chaque appel à recv() bloquerait jusqu'à ce que les données soient prêtes sur ce socket particulier.

Avec poll(), tu peux gérer toutes ces connexions en même temps avec un seul appel à poll(), qui te dira quels descripteurs sont prêts à recevoir ou envoyer des données. Cela réduit le nombre de fois où tu appelles des fonctions bloquantes et permet de ne traiter que les sockets actifs.

3. Évitement du busy-waiting
Si tu utilises recv() sur plusieurs sockets dans une boucle, tu peux être tenté d'utiliser une technique comme le busy-waiting, où tu appelles recv() en boucle pour chaque socket jusqu'à ce que des données arrivent. Cela consomme beaucoup de CPU inutilement, car tu fais des appels en continu sans résultat jusqu'à ce qu'un socket devienne prêt.

Avec poll(), le système te notifie uniquement lorsque des événements (comme POLLIN ou POLLOUT) se produisent sur les descripteurs surveillés. Cela signifie que tu n'as pas besoin de gaspiller du temps CPU à vérifier continuellement des sockets inactifs.

4. Gestion des événements multiplexés
poll() permet la multiplexion d'événements, c'est-à-dire que tu peux surveiller plusieurs types d'événements sur plusieurs descripteurs à la fois. Par exemple, tu peux surveiller à la fois l'événement POLLIN (prêt pour la lecture) et POLLOUT (prêt pour l'écriture) sur le même descripteur, ou surveiller différents événements sur différents descripteurs en une seule opération.

Avec recv() et send(), tu ne peux traiter qu'un seul événement à la fois (lecture ou écriture) par descripteur de fichier, et tu devrais continuellement alterner entre les deux si tu utilises ces appels de manière classique.

5. Réduction des appels système
poll() te permet de faire un seul appel système pour surveiller de nombreux descripteurs de fichiers à la fois, ce qui est plus efficace en termes de performances que de faire un appel système individuel pour chaque descripteur avec recv() ou send().

Moins d'appels système signifie moins d'overhead, ce qui améliore la performance globale lorsque tu gères un grand nombre de connexions simultanées.

Exemple comparatif :
Avec recv()/send() bloquants : Si tu gères 100 connexions, tu devrais appeler recv() pour chaque socket, ce qui pourrait bloquer indéfiniment si aucune donnée n'est disponible, empêchant ton programme de répondre rapidement aux nouvelles connexions ou à d'autres événements.

Avec poll() : Tu appelles poll() une seule fois en surveillant tous les 100 sockets. Dès qu'un ou plusieurs sockets ont des données prêtes (lecture ou écriture), poll() te le signale, et tu peux alors appeler recv() ou send() uniquement sur les descripteurs prêts, réduisant ainsi le temps d'attente et améliorant l'efficacité.

Conclusion
poll() est plus efficace que recv() et send() dans les environnements où tu gères plusieurs connexions réseau simultanées, car il te permet de surveiller plusieurs descripteurs de fichiers en même temps sans bloquer, et il réduit l'overhead en limitant les appels système. Il est également plus scalable pour les serveurs qui doivent gérer un grand nombre de connexions simultanées, car il évite le busy-waiting et permet une gestion non bloquante des événements réseau.