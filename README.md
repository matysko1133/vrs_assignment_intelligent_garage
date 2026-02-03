# vrs_assignment_intelligent_garage

## Popis projektu

Cieľom projektu je návrh a realizácia inteligentného systému garáže, ktorý umožňuje **automatické otváranie a zatváranie garážových dverí** na základe prítomnosti vozidla. Systém zároveň poskytuje **spätnú väzbu používateľovi** prostredníctvom displeja a LED signalizácie.

Riešenie podporuje:

* automatický režim ovládania,
* manuálny režim (ovládanie tlačidlom),
* záznam dát o vjazdoch a výjazdoch vozidla do/z garáže.

---
# Dôležité!

## Build project

Projekt je rozdelený na 2 "podprojekty", 1 je pre STM s displejom (**display_uart_receiver**) a druhý je pre hlavnú logiku (**garage_uart_sender**). Najprv sa vyčistí, zbuilduje a nahrá sender, pričom STM s LCD zostáva nezapojené, potom sa zbuilduje a nahrá kód pre LCD displej.

---

## Hardvérové riešenie (HW)

* **Servo motor**
  Umiestnený na garážových dverách, zabezpečuje ich fyzické otváranie a zatváranie.

* **Displej**
  Poskytuje spätnú väzbu pre používateľa a zobrazuje textové informácie o stave garáže.

* **Senzory prítomnosti vozidla**
  Dva ultrazvukové senzory – jeden umiestnený **vnútri garáže** a jeden **pred garážou**. Slúžia na detekciu prítomnosti vozidla.

* **Tlačidlo (Manual mode)**
  Umožňuje manuálne otvorenie alebo zatvorenie garáže nezávisle od automatickej logiky systému.

* **SD karta a čítačka**
  Slúži na ukladanie dát o vjazdoch a výjazdoch vozidla do/z garáže.

---

## Logika systému

### Inicializácia

Po zapnutí systému prebehne inicializačná fáza, počas ktorej:

* sa nastaví počiatočný stav systému,
* na displeji sa zobrazí úvodná správa,
* signalizuje sa zatvorený stav garážových dverí (červená LED),
* nastaví sa log systémovej inicializácie pre SD kartu.

---

### Prevádzková logika

Systém nepretržite sleduje stav senzorov a reaguje podľa aktuálnej situácie:

#### Detekcia vozidla vo vnútri garáže

* Ak je vozidlo detegované vo vnútri garáže:

  * je aktivovaná červená LED,
  * automatické otváranie garáže nie je povolené,
  * garáž sa zvnútra otvára iba manuálne tlačidlom.

#### Detekcia vozidla pred garážou

* Ak je vozidlo detegované pred garážou a garáž nie je obsadená:

  * systém začne otvárať bránu,
  * na displeji sa zobrazí správa **„IN PROGRESS…“**.

#### Otvorená garáž

* Po úplnom otvorení dverí:

  * displej zobrazí hlásenie **„OPEN“**,
  * aktivuje sa zelená LED.

#### Zatváranie garáže

* Ak je garáž otvorená a auto príde dovnútra, po 2 sekundách sa zavrú dvere
* Ak sa auto rozhodne po otvorení garáže nevôjsť dnu, garáž sa po 20 sekundách zavrie.

#### Manuálny režim

* Používateľ môže kedykoľvek otvoriť alebo zatvoriť garáž pomocou tlačidla,
* manuálny režim funguje nezávisle od automatickej logiky systému.

---

## Záznam dát

Každý prejazd vozidla (vjazd alebo výjazd) je zaznamenaný do RAM frontu a odtiaľ, ak je systém stabilný, sa zapíše na SD kartu v .txt formáte.
