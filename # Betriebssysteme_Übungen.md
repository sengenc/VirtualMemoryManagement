# Betriebssysteme
## Übung 1
### Aufgabe 1.1 Umgang mit der Shell
#### Aufgabe 1.1.1 Shell Grundlagen und einfache Kommandos
- Lesen Sie den Eintrag zum Befehl `pwd` komplett (`man üwd`).
  - `pwd` dient der Ausgabe des vollständigen Namens des aktuellen Arbeitsverzeichnisses
  - Optionen:
    - `-L, --logical`: gilt auch für Verknüpfungen
    - `-P, --physical`: ohne Verknüpfungen (default option)
    - `--help`: Anzeige Hilfe und Beenden
    - `--version`: Anzeige der Version und Beenden

- Lesen Sie, was in der Beschreibung zum Befehl `man man` über die Einteilung des System-Manuals in verschiedene Sektionen steht. Wie wählt man eine Sektion aus?

`man` entspricht einem Handbuch, dass in Abschnitte unterteilt ist, die die Beschreibung der Programme und Funktionen enthalten.
Wird die Eingabe von `man` um einen Abschnitt ergänzt, wird lediglich in diesem Abschnitt nach einem Eintrag für den dann folgenden Befehl gesucht (Bsp.: `man 1 echo` zeigt den Handbucheintrag zum Befehl `echo` im Abschnitt für ausführbare Programme oder Shell-Befehle an). 

- Lesen Sie die Kurzbeschreibung zum Befehl `apropos` und probieren Sie den Befehl.

`apropos` dient der Suche von Schlagwörtern in den `man` pages

- grundlegende Verwendung folgender Befehle
  - `echo`: display line of text
  - `pwd`: print name of current/Working directory
  - `ls`: list directory contents
  - `ps`: Schnappschuss der aktuellen Prozesse
  - `mkdir`: Erstellen neuer Verzeichnisse
  - `rmdir`: Entfernen **leerer** Verzeichnisse
  - `cat`: Aneinanderreihen von Dateien und Ausgabe in der Standartausgabe
  - `rm`: Entfernen von Dateien oder Verzeichnissen
  - `cp`: Kopieren von Dateien oder Verzeichnissen
  - `mv`: Verschieben oder Umbenennen von Dateien
  - `head`: Ausgabe des Anfangs einer Datei
  - `tail`: Ausgabe des Endes einer Datei
  - `grep`: Suche nach vorgegebenem Muster in einer Datei und Ausgabe der gefundenen Zeilen
  - `touch`: Erstellen einer leeren Datei

- Unterschied zwischen `ls -l` und `ls -la`?
  - `ls`: dient der Anzeige des Inhalts des Verzeichnisses
  - `ls -l`: Option führt zu einer detaillierteren Ausgabe mit weiteren Informationen zu den Inhalten
  - `ls -la`: Wie `ls -l`, nur werden nun ALLE Inhalte des Verzeichnisses angezeigt; auch solche, die mit "." beginnen 

- Welche Ausgaben erwarten Sie am Bildschirm in der oben beschriebenen Situation bei den unten angegebenen Kommandos?
  - `ls -l e`:  
  `ls: Zugriff auf 'e' nicht möglich: Datei oder Verzeichnis nicht gefunden`
  - `ls -l d e`:  
  `ls: Zugriff auf 'e' nicht möglich: Datei oder Verzeichnis nicht gefunden`  
  `-rw-rw-r-- 1 timrpt timrpt 0 Apr 19 09:15 d`
  - `ls -l d e > /dev/null`:  
  `ls: Zugriff auf 'e' nicht möglich: Datei oder Verzeichnis nicht gefunden`  
    (dabei wird der Standardausgabe Kanal nach /dev/null umgeleitet und der Standard Fehlerkanal bleibt bestehen)
  - `ls -l d e 2> /dev/null`:  
  `-rw-rw-r-- 1 timrpt timrpt 0 Apr 19 09:15 d`  
    (dabei wird der Standard Fehlerkanal (2) nach /dev/null umgeleitet und der Standardausgabe Kanal bleibt bestehen)
  - `ls -l d e >/dev/null 2>/dev/null`:  
  \-  
  (dabei werden beide Kanäle nach /dev/null umgeleitet)
  - `ls -l d e > /dev/null 2>&1`:  
  \-  
    (der Standardausgabe Kanal wird nach /dev/null umgeleitet und der Standard Fehlerkanal wird auf den Standardausgabe Kanal (&1) umgeleitet, der ja wiederum nach /dev/null umgeleitet wird)  
<br></br>
  #### Aufgabe 1.1.2 Statuscode
  Statuscodes zur vorherigen Ausgabe mittels `echo $?`:
  - `ls -l e`: 127 (Ausgabe Standard Fehlerkanal)
  - `ls -l d`: 0 (Ausgabe Standardausgabe Kanal ohne Fehler)
  - `ls -l d e`: 2 (Ausgabe beider Kanäle)
<br></br>
  #### Aufgabe 1.1.3 Filter und Pipelines
  **A**
  - `cat`: Aneinanderreihen der Eingaben
  - `wc`: Ausgabe von newline, word and byte count der gesamten Eingaben
  - `tr e x`: Ersetzt alle "e" durch "x"
  - `tr -d e`: Entfernt alle "e"
  - `grep[0-9]`: Markieren aller Ziffern von 0-9 in rot
  - `grep ^[0-9]`: Findet den Anfang einer Zeile (^) und prüft auf die gegebenen Ziffern

**B**
  - `ls -l | grep ^d` : Listet den Inhalt des Verzeichnisses mit zusätzlichen Informationen bei dem der Anfang der Zeilen der Buchstabe "d" ist (=directory)

  - `ls -l | grep ^d | wc -l`: gibt die Anzahl der Zeilen aus, bei denen der Anfangsbuchstabe ein "d" ist (=directory)(s.o.)

  - `ls -l | grep ^d | awk '{print $9, $1}'`:
    - Auflisten des Inhalts des Verzeichnisses mit zusätzlichen Informationen
    - Prüfen des Zeilenanfangs auf "d" (=directory)
    - Ausgabe des 9. und des 1. Elementes

  - `echo 4 + 3 | bc`: Führt die Berechnung des Ausdruckes aus und gibt das Ergebnis zurück

  - `echo "sqrt(2)" | bc -l`: Definiert die Standard Mathematik Bibliothek, berechnet den Ausdruck und gibt das Ergebnis zurück
<br></br>
#### Aufgabe 1.1.4 Kommandosubstitution
**A**
  - `date "+%Y-%m-%d"`: Ausgabe des aktuellen Datums im Format YYYY-MM-DD

  - `DATE='date "+%Y-%m-%d"'; echo $DATE`: Speichern des aktuellen Datums mit Format YYYY-MM-DD in der Variablen DATE und anschließende Ausgabe

  - `DATE=$(date "+%Y-%m-%d"); echo $DATE`: s.o.

  - `date`: gibt das aktuelle Datum mit Wochentag, Uhrzeit und Zeitzohne aus

  - `TAG=$(date|awk '{print $1}'); echo $TAG`:
  gibt das erste Element des aktuellen Datums (date) zurück

**B**
  - Wie kann man an die Variable WURZEL2 die Quadratwurzel von 2 zuweisen?
  ``WURZEL2=`echo "sqrt(2)" | bc -l`; echo $WURZEL2`` (legt jedoch neue Datei mit Namen `WURZEL2` an)

#### Aufgabe 1.1.5 Einfaches Shellscripting
**A**
- Funktion:  
`function anzahlsubdirs { ls -l $1 | grep ^d | wc -l; }`; `anzahlsubdirs <dir> `
- Shell-Script: 
  ```console
  >#!/bin/bash
  ># anzahlsubdirs.sh
  ># Return number of subdirectories of directory, given as 
  >parameter
  >
  >anzahlsubdirs() {
  >	echo $((ls -l $1)| grep ^d | wc -l)
  >}
  >
  >anzahlsubdirs $1
  ```

**B**  
Shell-Script
```console
#!/bin/bash
# showargs.sh
# Just returns the parameters

showargs() {
    for var in "$@"
    do
        echo "$var"
    done
}

showargs $@
```

Output:
```console
$ ./showargs.sh hallo welt wie geht es dir
hallo
welt
wie
geht
es
dir
```

**C**  
```console
#!/bin/bash
# showargs.sh
# Returns the parameters with numbers

showargs() {
    i=1
    for var in "$@"
    do
        echo $i. $var
        let i++
    done
}

showargs $@
```

Output:  
```console
$ ./showargs.sh hallo welt wie geht es dir
1. hallo
2. welt
3. wie
4. geht
5. es
6. dir
```

### Aufgabe 1.2 Verkettete Listen, Dynamischer Speicher, Ein und Ausgabe
