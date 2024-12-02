
**after celia s review ,  i fixed the following bugs :**

join:
- pas de caractères spéciaux : join #hey’ -> invalid , bad mask
- si je suis déjà dedans j'ai juste ce message qui pop, il faudrait que ça envoie rien dans le meilleur des cas
![image](https://github.com/user-attachments/assets/0619841f-453a-4216-b804-14c15d06c41d)
- quand j'ai join trop de channels et que je remet un channel dans lequel je suis déjà, je devrait pas avoir cette erreur
![image](https://github.com/user-attachments/assets/28752c66-6ec2-42bd-9a76-2848e8929f93)
- broken pipe 

Part:
- commande ajoutée
- un channel vide doit se suprimer automatiquement ==> pouvez vous rester, j'ai l'impression que c'est ok 

mode:
- si on entre juste + ou - , ça retourne
- le uint8 de limit faisait que la limite int 65 se transformait en A
- si on etnre 0 en limite, ça désactive la limite. 
- overflow contrôlé 

parsing :  
- les messages de plus de 512 char sont trunked
- les empty messages ignored (le /r a été trim en plus du / n) 


je n'ai pas regardé les autres erreurs sur lauthentification qu a relevée Célia, puisque le fonctionnement me parait bien. 

et je ne vois plus de messages apparaître en double c'est cool aha 

