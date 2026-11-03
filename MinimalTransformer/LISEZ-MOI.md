# Minimal Transformer : plugin VST3 pour Ableton

Interface style diner américain : cadre chromé, enseigne néon (HIM IS BACK NO PANIC), plaques crème, boutons chromés, colonnes d'ampoules pour les séquenceurs.

Filtre résonant "morphing" (LP → BP → HP) piloté par une enveloppe, un LFO et **deux séquenceurs 16 pas**, avec bitcrusher, delay synchronisé au tempo et 37 presets.

## Obtenir le fichier .vst3 (sans rien installer sur ton ordi)

1. Crée un compte gratuit sur github.com, puis un **nouveau dépôt** (New repository), nom : `MinimalTransformer`.
2. Dans le dépôt : **Add file > Upload files**. Glisse-y `CMakeLists.txt` et le dossier `Source` (avec ses 6 fichiers), puis **Commit changes**.
3. **Add file > Create new file**. Dans le champ du nom, tape exactement `.github/workflows/build.yml`. Colle le contenu du fichier `build.yml` de ce zip, puis **Commit changes**.
4. Onglet **Actions** : la compilation démarre toute seule (5 à 10 minutes).
5. Quand elle est verte, clique dessus, puis télécharge en bas de page, dans **Artifacts**, `MinimalTransformer-Windows-VST3` ou `MinimalTransformer-macOS-VST3`.
6. Si la compilation est rouge : copie le message d'erreur et envoie-le moi, je corrige.

## Installer

**Windows** : copie le dossier `MinimalTransformer.vst3` dans `C:\Program Files\Common Files\VST3`.

**Mac** : copie `MinimalTransformer.vst3` dans `/Library/Audio/Plug-Ins/VST3`, puis dans le Terminal :
`xattr -dr com.apple.quarantine /Library/Audio/Plug-Ins/VST3/MinimalTransformer.vst3`

## Dans Ableton

Préférences > Plug-ins > active **VST3 Plug-in System Folders**, puis **Rescan**. Le plugin apparaît sous le nom **Minimal Transformer**.

## Les réglages

**Filtre** : Cutoff, Resonance, Morph (fondu LP → BP → HP), Drive (saturation avant le filtre).
**Enveloppe** : Env Amount (négatif = inversé), Attack, Release. Le volume du son entrant fait bouger la coupure.
**LFO** : Depth, Rate, Shape, Sync (libre ou calé sur le tempo), Stereo Spread.
**Effets** : Bitcrush, Delay (temps, feedback, envoi).
**Sortie** : Mix et Output.

## Les deux séquenceurs (SEQ A et SEQ B)

Chacun a 16 colonnes d'ampoules : clique ou glisse sur une colonne pour choisir sa hauteur (6 niveaux, clique tout en bas pour couper le pas). Le pas en cours s'allume en blanc pendant la lecture. Ils suivent le tempo d'Ableton (et tournent aussi au tempo du set quand le transport est à l'arrêt).

- **Rate** : vitesse d'un pas (1/4, 1/8, 1/16, 1/32, triolets).
- **Length** : nombre de pas utilisés (1 à 16).
- **Glide** : lissage entre deux pas.
- **Depth** : intensité, négative pour inverser.
- **Destination** : Cutoff, Resonance, Morph, Drive, Delay Send, Bitcrush ou Gate (coupe le son en rythme, effet trance gate). Avec un Depth négatif, le Gate devient un effet de pompage "sidechain".

## Les presets

Utilise le menu en haut à droite et les flèches < >. Catégories : filtres et basses (Minimal Transformer, Minimal Pulse, Rubber Bass, Acid Squeeze, Wah Walker…), rythmiques (Trance Gate 16, Offbeat Chop, Sidechain Pump…), delays (Dub Throws, Ping Spiral…), lo-fi (Tape Dust, 8-Bit Squash…) et mouvements (Riser Opener, Random Walk, Glitch Garden…).

Ce sont des points de départ : je n'ai pas pu les écouter, ajuste-les à l'oreille.

## Polices (facultatif)

L'enseigne et les textes utilisent les polices gratuites **Pacifico**, **Fredoka** et **Courier Prime** si elles sont installées sur ton ordinateur (Google Fonts). Sinon, le plugin prend des polices proches déjà présentes sur ton système : tout fonctionne, mais l'enseigne sera un peu différente de la maquette.

## Utilisation de l'interface

- Boutons : glisse vers le haut ou le bas. Double-clic = valeur par défaut. La valeur s'affiche pendant que tu tournes.
- Presets : menu façon étiquette de jukebox en haut à droite, avec les deux boutons chromés < > pour passer au suivant ou au précédent.
