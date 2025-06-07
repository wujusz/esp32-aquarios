# AquariOS

AquariOS to system zarządzania dla ESP32/ESP8266 pozwalający w pełni konfigurować urządzenia przez interfejs WWW. Konfiguracja przechowywana jest w plikach JSON w katalogu `/config` co umożliwia dodawanie modułów i kanałów bez ponownej kompilacji.

## Cechy projektu

* **Dynamiczne kanały** – każdy kanał jest opisany osobnym plikiem `channelXState.json`. Podczas uruchamiania firmware skanuje katalog i tworzy harmonogramy.
* **Moduły** – kilka kanałów można przypisać do jednego modułu (np. oświetlenie, przełącznik). Harmonogram przypisany do modułu propaguje się na wszystkie jego kanały.
* **Harmonogram** – formularz w UI umożliwia zdefiniowanie wielu okresów działania. Moduły typu LED mogą działać zgodnie ze wschodem i zachodem słońca oraz mają ustawienia jasności dla każdego kanału.
* **Integracja z Home Assistant** – dzięki MQTT i plikom w `src/automation/Homeassistant.h` kanały mogą być widoczne jako encje w Home Assistant.
* **REST i WebSocket** – konfigurację kanałów oraz stan można odczytywać i modyfikować przez API REST, a bieżące dane są wysyłane poprzez WebSocket.
* **Moduł termometru** – odczyt temperatury z czujnika DS18B20, zapisywanie historii i automatyczne sterowanie modułem grzałki.

## Obsługiwany sprzęt

Projekt testowano na płytkach ESP32 i ESP8266. Przykładowe urządzenia:

* standardowa płytka **ESP32** – obsługuje cztery kanały i pełny interfejs z WWW w pamięci,
* **LilyGO 4‑channel relay** – ESP32 z wbudowanymi przekaźnikami,
* moduły **ESP8266** oraz gotowe przełączniki Sinilink lub ESP‑01 (dla nich należy wyłączyć `PROGMEM_WWW`).

Domyślne piny sterujące przekaźnikami to:

```
esp32:    GPIO21, GPIO19, GPIO18, GPIO5
esp8266:  GPIO5, GPIO12, GPIO13, GPIO14
esp-01:   GPIO0 (jeden kanał)
```

Do sterowania oświetleniem czy pompami najlepiej użyć przekaźników SSR (np. SSR‑40DA) lub modułów przekaźnikowych.

### Urządzenia o małej pamięci

Jeżeli używasz płytek z 1 MB flash, wyłącz `PROGMEM_WWW` w `platformio.ini` i wybierz tylko jeden kanał. Szczegóły znajdują się w repozytorium [rjwats/esp8266-react](https://github.com/rjwats/esp8266-react).

## Konfiguracja i kompilacja

Najważniejsze opcje definiuje plik `features.ini`. Domyślnie włączone są MQTT, NTP i podstawowe zabezpieczenia. Budowę firmware przeprowadzisz poleceniem:

```bash
platformio run
```

Interfejs React znajduje się w katalogu `interface` i kompiluje się za pomocą:

```bash
npm install
npm run build
```

Po wgraniu na płytkę i restarcie urządzenia interfejs WWW będzie dostępny w sieci lokalnej. Dodane kanały i moduły zapisywane są w `/config`, więc wystarczy ponowne uruchomienie aby odczytać zaktualizowaną konfigurację.

## Co jeszcze można dodać?

Poniżej kilka pomysłów zaczerpniętych z projektu [reef-pi](https://github.com/reef-pi/reef-pi):

* automatyczne dolewanie wody (Auto Top Off),
* sterowanie pompami dozującymi (dosing pump),
* monitorowanie temperatury i pH z alarmami,
* możliwość wykonywania zdjęć akwarium i przesyłania ich do chmury,
* wykresy i powiadomienia e‑mail.
* rejestrowanie danych pomiarowych w plikach `/logs` i możliwość wyświetlania wykresów w interfejsie.

Takie funkcje można zrealizować na bazie istniejącej architektury modułów i harmonogramów.

