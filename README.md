# vrs_assignment_intelligent_garage

## Popis projektu

Cieľom projektu je návrh a realizácia inteligentného systému garáže, ktorý umožňuje **automatické otváranie a zatváranie garážových dverí** na základe prítomnosti vozidla. Systém zároveň poskytuje **spätnú väzbu používateľovi** prostredníctvom displeja a LED signalizácie.

Riešenie podporuje:

* automatický režim ovládania,
* manuálny režim (ovládanie tlačidlom),
* záznam dát o vjazdoch a výjazdoch vozidla do/z garáže.

---

## Hardvérové riešenie (HW)

* **Servo motor**
  Umiestnený na garážových dverách, zabezpečuje ich fyzické otváranie a zatváranie.

* **Displej**
  Poskytuje spätnú väzbu pre používateľa a zobrazuje textové informácie o stave garáže.

* **Senzor polohy garážových dverí**
  Slúži na detekciu stavu dverí (otvorené / zatvorené).

* **Senzory prítomnosti vozidla**
  Dva senzory – jeden umiestnený **vnútri garáže** a jeden **pred garážou**. Slúžia na detekciu prítomnosti vozidla (predpokladané riešenie: ultrazvukové senzory).

* **Tlačidlo (Manual mode)**
  Umožňuje manuálne otvorenie alebo zatvorenie garáže nezávisle od automatickej logiky systému.

* **SD karta a čítačka**
  Slúži na ukladanie dát o vjazdoch a výjazdoch vozidla do/z garáže (kapacita do 32 GB).

---

## Logika systému

### Inicializácia

Po zapnutí systému prebehne inicializačná fáza, počas ktorej:

* sa nastaví počiatočný stav systému,
* na displeji sa zobrazí úvodná správa,
* signalizuje sa zatvorený stav garážových dverí (červená LED).

---

### Prevádzková logika

Systém nepretržite sleduje stav senzorov a reaguje podľa aktuálnej situácie:

#### Detekcia vozidla vo vnútri garáže

* Ak je vozidlo detegované vo vnútri garáže:

  * na displeji sa zobrazí hlásenie **„Prejazd obsadený“**,
  * aktivuje sa červená LED,
  * automatické otváranie garáže nie je povolené.

#### Detekcia vozidla pred garážou

* Ak je vozidlo detegované pred garážou a garáž nie je obsadená:

  * systém začne proces otvárania garážových dverí,
  * na displeji sa zobrazí správa **„Otvára sa…“**.

#### Otvorená garáž

* Po úplnom otvorení dverí:

  * systém zobrazí hlásenie **„Prejazd voľný“**,
  * aktivuje sa zelená LED.

#### Zatváranie garáže

* Ak je garáž otvorená a nie je detegovaný dôvod na jej ponechanie otvorenej:

  * systém začne proces zatvárania garážových dverí.

#### Manuálny režim

* Používateľ môže kedykoľvek otvoriť alebo zatvoriť garáž pomocou tlačidla,
* manuálny režim funguje nezávisle od automatickej logiky systému.

---

## Záznam dát

Každý prejazd vozidla (vjazd alebo výjazd) je zaznamenaný do .csv periodicky posielaneho na SD kartu.


