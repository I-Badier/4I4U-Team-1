# Donk-e
Tricycle avec deux roues motrices. Réalisé dans le cadre du projet innovant de la 5ème année spécialité Innovative Smart Systems à l'INSA Toulouse. Répond aux projets 4I4U et Extrême Défi de l'ADEME.

Fonctionnalités : 
- Allumage automatique des feux
- Système de clignotants
- Application d'accompagnement pour montrer à disposition les statistiques du tricycle (niveau de batterie, kilométrage, usure des consommables...), l'historique des trajets ainsi que des notifications.

## Application mobile
Développement de l'application effectué sur un autre répertoire, disponible [ici](https://github.com/emmanuel-pastor/tricycle-iot-app).

## Firmware
### Composants
- Micro-contrôleur ESP32-C3-DevKit-M1
- 4 clignotants : Grove Chainable LED
- 2 boutons pour activer/désactiver les clignotants : Grove Yellow LED Button
- 3 barres de LED pour les feux avants et arrière : Grove LED stick
- 1 capteur de luminosité : Grove Light Sensor

### Architecture
![Architecture](https://github.com/I-Badier/4I4U-Team-1/blob/main/Pictures/archiElec.jpg)

## Documentation
Espressif documentation for [ESP32 C3 devKit M1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html)  
![Pinout](https://github.com/I-Badier/4I4U-Team-1/blob/main/Pictures/pinout-esp32-c3-devkit-m1.jpg)
