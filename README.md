# 4I4U-Team-1
Tricycle avec deux roues motrices. Allumage automatique des feux et système de clignotants.

## Embedded ESP32
Code embarqué sur le vélo
24/11 :  
- 2 LED chaînées pour les clignotants. LED numéro 1 pour cligno gauche, numéro 2 pour cligno droit (constantes pré-processeurs). Clignotement normé à 90 par minutes.  
Fonctionne en 5V, pin 7 fil jaune (clock) et pin 8 fil blanc (data)
- Capteur de luminosité. Fonctionne en 5V, pin 0 pour un canal d'ADC. Valeurs de 0 (noir complet) à 4095 (luminosité ambiante). Sensibilité à la luminosité assez faible, sûrement à changer pour le prototype. 

