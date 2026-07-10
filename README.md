# Weather CLI / Ilmarakendus

A console application written in C for retrieving current weather conditions and forecasts from XML weather feeds. The application supports favorite weather stations, cached downloads, and an interactive menu.

C-keeles kirjutatud konsooliprogramm, mis kuvab XML-ilmaandmete põhjal hetkeilma ja ilmaprognoosi. Programm toetab lemmikjaamu, andmete vahemällu salvestamist ja menüüpõhist kasutajaliidest.

---

# English

## Features

* Downloads current weather data from XML weather feeds.
* Displays weather information for favorite weather stations.
* Displays weather forecasts.
* Uses XML parsing to process downloaded data.
* Supports multiple favorite weather stations.
* Validates station names before adding them.
* Saves favorite stations between program runs.
* Automatically loads favorite stations on startup.
* Caches downloaded weather data and refreshes it no more than once per hour.
* Displays **N/A** when weather information is unavailable.
* Menu-driven console interface.
* Includes input validation and basic error handling.

## Project Structure

```text
weather_cli.c        Main application
weather_cli.h        Function declarations and constants
Makefile             Build configuration
```

## Requirements

* GCC
* GNU Make
* libcurl
* libxml2

## Building

Compile the project using:

```bash
make
```

## Running

```bash
./weather_cli
```

## Program Workflow

1. Favorite weather stations are loaded from a local file.
2. Default stations are used if no favorites file exists.
3. The application displays the weather data source.
4. Current weather data is downloaded and cached.
5. A menu is presented to the user.
6. The user may:

   * View current weather.
   * View weather forecast.
   * Add a favorite station.
   * Remove a favorite station.
   * Exit the application.
7. Modified favorite stations are saved automatically.
8. Cached weather data is reused until one hour has passed.

## Menu

### 1. Current Weather

Displays current weather conditions for all favorite weather stations.

### 2. Weather Forecast

Displays the available weather forecast.

### 3. Add Favorite Station

Adds a new weather station after validating that it exists in the downloaded weather data.

### 4. Remove Favorite Station

Removes a station from the favorites list.

### 0. Exit

Terminates the application and saves updated favorite stations.

## Favorites

When the application is launched for the first time, the following stations are available by default:

* Tallinn-Harku
* Tartu-Tõravere
* Narva

Favorite stations are stored locally and automatically loaded during startup.

## Data Source

Weather information is obtained from XML weather feeds provided by the Estonian Environment Agency.

The application includes a reference to the official weather service in accordance with the provider's usage requirements.

## Technical Details

* Written in C
* XML data processing
* HTTP requests using libcurl
* Menu-driven interface
* Cached weather downloads
* Dynamic memory management
* Input validation
* Compatible with GCC and Make

---

# Eesti

## Funktsioonid

* Laeb hetkeilma XML-andmetest.
* Kuvab hetkeilma lemmikjaamade jaoks.
* Kuvab ilmaprognoosi.
* Töötleb XML-andmeid.
* Toetab mitut lemmikjaama.
* Kontrollib jaama olemasolu enne lisamist.
* Salvestab lemmikjaamad faili.
* Laeb lemmikjaamad automaatselt programmi käivitamisel.
* Salvestab allalaetud ilmaandmed vahemällu ning uuendab neid maksimaalselt kord tunnis.
* Kuvab **N/A**, kui vajalikud ilmaandmed puuduvad.
* Menüüpõhine kasutajaliides.
* Sisaldab sisendi valideerimist ja vigade käsitlemist.

## Projekti struktuur

```text
weather_cli.c        Programmi põhifail
weather_cli.h        Päisefail
Makefile             Kompileerimine
```

## Nõuded

* GCC
* GNU Make
* libcurl
* libxml2

## Kompileerimine

```bash
make
```

## Käivitamine

```bash
./weather_cli
```

## Programmi töövoog

1. Lemmikjaamad loetakse failist.
2. Kui faili ei ole, kasutatakse vaikimisi jaamu.
3. Kuvatakse ilmaandmete allikas.
4. Hetkeilm laaditakse ja salvestatakse vahemällu.
5. Kuvatakse menüü.
6. Kasutaja saab:

   * vaadata hetkeilma;
   * vaadata ilmaprognoosi;
   * lisada lemmikjaama;
   * eemaldada lemmikjaama;
   * lõpetada programmi.
7. Muudetud lemmikud salvestatakse faili.
8. Ilmaandmeid uuendatakse mitte sagedamini kui kord tunnis.

## Menüü

### 1. Hetkeilm

Kuvab hetkeilma kõikidele lemmikjaamadele.

### 2. Ilmaprognoos

Kuvab ilmaprognoosi.

### 3. Lisa lemmikjaam

Lisab uue jaama pärast selle kontrollimist.

### 4. Eemalda lemmikjaam

Eemaldab valitud jaama lemmikute loendist.

### 0. Välju

Lõpetab programmi ning salvestab lemmikjaamad.

## Vaikimisi lemmikud

Esmakordsel käivitamisel kasutatakse järgmisi jaamu:

* Tallinn-Harku
* Tartu-Tõravere
* Narva

## Andmeallikas

Ilmaandmed pärinevad Eesti Keskkonnaagentuuri XML-ilmateenusest.

Programm viitab andmete allikale vastavalt teenuse kasutustingimustele.

## Tehnilised omadused

* Kirjutatud C-keeles
* XML-andmete töötlemine
* HTTP päringud libcurl abil
* Menüüpõhine kasutajaliides
* Andmete vahemällu salvestamine
* Dünaamiline mäluhaldus
* Sisendi valideerimine
* GCC ja Make tugi
