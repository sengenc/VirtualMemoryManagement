THM Betriebssysteme Hausaufgabe 2 - Virtual Memory Manager

Dieses Repository enthält die Implementierung eines Virtual Memory Managers für die zweite Hausaufgabe im Fach Betriebssysteme an der Technischen Hochschule Mittelhessen (THM). Ziel der Aufgabe ist es, die Übersetzung von logischen in physische Adressen mittels eines Translation Lookaside Buffers (TLB) und Page Tables zu simulieren und zu verstehen.
Übersicht

Der Virtual Memory Manager simuliert die Adressübersetzung in einem System mit virtuellem Speicher. Er behandelt auch Page Faults durch Demand Paging und implementiert eine FIFO-Seitenaustauschstrategie.

Funktionsweise

    Das Programm liest aus addresses.txt logische Adressen und übersetzt diese in physische Adressen unter Nutzung eines TLB und eines Page Tables.
    Behandlung von Page Faults durch das Laden von Seiten aus BACKING_STORE.bin in den physikalischen Speicher.

Beispielausgabe

Die Ausgabe des Programms zeigt für jede übersetzte Adresse:

    Logische Adresse
    Übersetzte physische Adresse
    Wert des Bytes an der physischen Adresse
    TLB Hit/Miss
    Page Table Hit/Miss

Beispiel:
    
Virtual: 16916, Physical: 20, Value: 0, TLB hit: false, PT hit: false
Virtual: 62493, Physical: 285, Value: 0, TLB hit: false, PT hit: false

