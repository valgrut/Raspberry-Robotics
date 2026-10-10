# README

- AEG = Airsoft Electric Guns
- ETU = Electronic trigger unit


## Gearbox resources, links

- https://www.airsoftmegastore.com/index.php?route=product/product&product_id=37859&iid=41067&search=high+speed+steel+gear
- https://www.lancertactical.com/product-page/lancer-tactical-complete-m4-proline-gearbox-set
- https://airsoftpro.cz/cz/dily-zbrani-upgrade/pro-elektricke-zbrane-vnitrni/kabelaze-mosfety-konektory/procesorove-jednotky/upgrade-kit-etu-pro-gg-detail
- 


## Projekt 1 - Gearbox replica

- Cil: Extrahovat mechanismus z levne elektricke airsoft zbrane
- Problem: Levna elektricka nemela mechanismus, ona je mechanismus.
    - Drahe airsoft zbrane jej v sobe fakt maji separatne.
    - Soucasti je i ovladaci jednotka, umoznujici treba i prepinat mezi davkou, jednotlive a pod.
- Reseni: Okopirovat, Vymodelovat a vytisknout pouze ty casti nutne pro funkcni mechanismus
- Problem 2: **Overspin**: Jak udelat, aby motor udelal pouze 1 rotaci ciste softwarove??? (origo mel mechanicky switch a vypinal power do motoru, a asi to docela pekne vychazelo. Ale ne vzdy. Obcas to nevystrelilo).



### Overspin

Your gearbox cycles twice because of the excess in power, and your mosfet can’t do much about it; you’ll need an ETU, such as an Aster or a Perun, or something with Active brake, such as Gate’s Nano AAB.

Check if your replica’s mosfet has active break, and if so, how to activate it.



### Jak se resi overspin? Jak obecne kontrolovat gearbox?

**TLDR**: ETU (mosfet + aktivni brzda) / Aktivni brzda / mosfet / mosfet + aktivni brzda


#### Mechanicky

- physical cut-off lever (proste switch, ktery se nejakym tahlem vypne hned po tom, co vyleti kulicka)


#### Aktivni brzdy

- Znacky:
    - Gate's Nano AAB


#### MOSFET (Tranzistor)

- MOSFET je elektronický systém, ve kterém je nejdůležitějším prvkem tak zvaný Metal-Oxide Semiconductor Field-Effect Transistor, zkráceně tedy MOSFET. Po příslušné úpravě je možné jej nainstalovat do každé elektrické

- Meni zpusob, jakym proud prochazi ruznymi castmi zbrane [1].
- 

[1]: https://gunfire.com/cs/blog/co-to-je-mosfet-1640270838.html


#### ETU (Electronic Trigger Unit)

- = Procesorova jednotka pro presne rizeni strelby
- Nahrazuje tradicni mechanicky spoustovy system
- Narozdil od bezneho MOSFET a Aktivni brzdy nabizi sirsi moznosti, viz 'vlastnosti' dole.
- Obvykle obsahuje prave i integrovany mosfet i aktivni brzdu

- Obvykle vlastnosti:
    - **Programovani rezimu strelby** (safe, semi, burst, auto, ...)
    - **Aktivni brzda** - Zastavi motor po kazdem vystrelu. Resi tak problem dvojiteho vystrelu
    - **Binarni spoust** - vystreli pri stisku i povoleni spouste
    - **Snizeni kadence** - muze zmensit RoF (Rate of Fire)
    - **DRM mod** - Nastavi interval mezi jednotlivymi vystrely
    - **Diagnostika** - Kontroluje stav zbrane, i samotny MOSFET.
    - **Elektricka pojistka**

- Znacky:  Gate, Jefftron, Perun







