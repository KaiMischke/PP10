# PP10

## Goal

In this exercise you will:

* Explore custom `struct` types and `typedef` in C.
* Link against existing system libraries (e.g., `-lm`).
* Create and evolve a custom C library from header-only to a precompiled static archive and install it system-wide.
* Install and use a third-party JSON library (`jansson`) via your package manager.
* Download, build, and install a GitHub-hosted library with a Makefile into standard include/lib paths.

**Important:** Start a stopwatch when you begin and work uninterruptedly for **90 minutes**. When time is up, stop immediately and record where you paused.

---

## Workflow

1. **Fork** this repository on GitHub.
2. **Clone** your fork locally.
3. Create a `solutions/` directory in the project root:

   ```bash
   mkdir solutions
   ```
4. For each task, add or modify source files under `solutions/`.
5. **Commit** and **push** your solutions.
6. **Submit** your GitHub repo link for review.

---

## Prerequisites

* GNU C compiler (`gcc`) and linker (`ld`).
* Make utility (`make`).
* `apt` or your distro’s package manager.

---

## Tasks

### Task 0: Exploring `typedef` and `struct`

**Objective:** Define and use a custom struct type with `typedef`.

1. Create `solutions/point.h` with:

   ```c
   typedef struct {
       double x;
       double y;
   } Point;
   ```
2. Create `solutions/point_main.c` that includes `point.h`, declares a `Point p = {3.0, 4.0}`, and prints its distance from origin using `sqrt(p.x*p.x + p.y*p.y)` (linking `-lm`).

#### Reflection Questions

1. **What does `typedef struct { ... } Point;` achieve compared to `struct Point { ... };`?**
   **Lösung zu 1:**
   Der Unterschied liegt in der Verwendung des Typnamens im Code.
   Mit:
   
   `typedef struct {
    double x;
    double y;
} Point;`

wird ein neuer Typname `Point` definiert, der direkt verwendet werden kann ohne das Schlüsselwort `struct`.

Mit:

   `struct Point {
    double x;
    double y;
    };`

   wird nur eine Struktur mit dem Namen `Point` definiert, aber kein Typalias.
   Somit muss man immer `Struct Point` schreiben.
   
2. **How does the compiler lay out a `Point` in memory?**
   **Lösung zu 2:**

   `Point` besteht aus zwei `double`-Werten (je 8 Byte).
   Der Compiler legt die Struktur wie folgt im Speicher ab:
   - `x`: Offset 0
   - `y`: Offset 8
   Gesamtgröße: 16 Byte.
   

---

### Task 1: Linking the Math Library (`-lm`)

**Objective:** Compile and link a program against the math library.

1. In `solutions/`, compile `point_main.c` with:

   ```bash
   gcc -o solutions/point_main solutions/point_main.c -lm
   ```
2. Run `./solutions/point_main` and verify it prints `5.0`.

#### Reflection Questions

1. **Why is the `-lm` flag necessary to resolve `sqrt`?**
   **Lösung zu 1:**
   Die Funktion `sqrt` gehört zur Mathematikbibliothek `libm`.
   Das Flag `-lm` sagt dem Linker, dass er diese Bibliothek einbinden soll.
   Ohne `-lm` findet der Linker die Implementierung von `sqrt` nicht.
   
2. **What happens if you omit `-lm` when calling math functions?**
   **Lösung zu 2:**
   Der Compiler akzeptiert den Code, aber der Linker gibt einen Fehler aus.
   Das Programm lässt sich nicht erstellen, weil die Funktion nicht gefunden wird.

---

### Task 2: Header-Only Library

**Objective:** Create a simple header-only utility library.

1. Create `solutions/libutil.h` with an inline function:

   ```c
   #ifndef LIBUTIL_H
   #define LIBUTIL_H
   static inline int clamp(int v, int lo, int hi) {
       if (v < lo) return lo;
       if (v > hi) return hi;
       return v;
   }
   #endif
   ```
2. Create `solutions/util_main.c` that includes `libutil.h`, calls `clamp(15, 0, 10)` and prints `10`.
3. Compile and run:

   ```bash
   gcc -o solutions/util_main solutions/util_main.c
   ./solutions/util_main
   ```

#### Reflection Questions

1. **What are the advantages and drawbacks of a header-only library?**
   **Lösung zu 1:**
   Vorteile:
   - Keine separate Kompilierung oder Linker-Schritte nötig - einfach Header einbinden.
   - Einfaches Verteilungs- und Versionsmanagement.
   - Funktionen können als inline vom Compiler optimiert werden.

   Nachteile:
   - Längere Kompilierzeiten, da Code mehrfach in allen Übersetzungseinheiten verarbeitet wird.
   - Größere Binärdateien möglich, wenn Funktionen nicht richtig inlined werden.
   - Änderungen am Header erfordern Neukompilierung aller abhängigen Dateien.
     
2. **How does `static inline` affect linkage and code size?**
   **Lösung zu 2:**
   - `static inline` sorgt dafür, dass die Funktion nur in der jeweiligen Übersetzungseinheit sichtbar ist (keine globale Verlinkung).
   - Der Compiler kann die Funktion inline ersetzen, um Funktionsaufruf-Overhead zu vermeiden.
   - Falls nicht inlined, erzeugt jede Übersetzungseinheit eine eigene Funktion, was zu größerem Codeumfang führen kann.
   - Dadurch vermeidet man Linker-Konflikte bei mehrfachen Definitionen.

---

### Task 3: Precompiled Static Library

**Objective:** Convert the header-only utility into a compiled static library and link it manually.

1. Split `clamp` into `solutions/util.c` & `solutions/util.h` (remove `inline` and `static`).
2. Compile:

   ```bash
   gcc -c solutions/util.c -o solutions/util.o
   ```
3. Create the executable linking manually:

   ```bash
   gcc -o solutions/util_main_pc solutions/util.o solutions/util_main.c
   ```
4. Run `./solutions/util_main_pc` to verify output.

#### Reflection Questions

1. **Why must you include `solutions/util.o` when linking instead of just the header?**
**Lösung zu 1:**
- Der Header enthält nur Deklarationen (Schnittstellen), keine Implementierungen.
- Die eigentlichen Code-Definitionen (Funktionen wie `clamp`) stehen in `util.o`
- Ohne `util.o` fehlen beim Linken die Definitionen.

2. **What symbol resolution occurs at compile vs. link time?**
   **Lösung zu 2:**
   Kompilieren:
   - Überprüft Syntax, Typen und verwendet Deklarationen aus Headern.
   - Erzeugt Objektdateien mit unaufgelösten Symbolen (Funktionennamen).

   Linken:
   - Verbindet Objektdateien und Bibliotheken.
   - Sucht Definitionen aller Symbole (Funktionen, Variablen).
   - Ohne gefundene Definitionen gibt es Linker-Fehler.



---

### Task 4: Packaging into `.a` and System Installation

**Objective:** Archive the static library and install it to system paths.

1. Create `libutil.a`:

   ```bash
   ar rcs libutil.a solutions/util.o
   ```
2. Move headers and archive:

   ```bash
   sudo cp solutions/util.h /usr/local/include/libutil.h
   sudo cp libutil.a /usr/local/lib/libutil.a
   sudo ldconfig
   ```
3. Compile a test program using system-installed lib:

   ```bash
   gcc -o solutions/util_sys solutions/util_main.c -lutil
   ```

   (assumes `#include <libutil.h>`)

#### Reflection Questions

1. **How does `ar` create an archive, and how does the linker find `-lutil`?**
**Lösung zu 1:**
- `ar` fasst mehrere Objektdateien (`.o`) zu einer einzigen Archivdatei (`.a`) zusammen.
- Das Archiv ist im Grunde ein gepacktes Format, das viele `.o` Dateien enthält und einen Index für schnellen Zugriff.
- Beim Linken durchsucht der Linker die Standardbibliothekspfade (z.B. `/usr/lib`, `/usr/local/lib`) nach einer Datei namens `libutil.a` (für `lutil`)
- Wenn `-lutil` angegeben ist, sucht der Linker also nach `libutil.a` in den bekannten Pfaden und bindet die enthaltenen Objektdateien ein.


   
2. **What is the purpose of `ldconfig`?**
   **Lösung zu 2:**
   - `ldconfig` aktualisiert den Cache der dynamischen Linker, der die Verzeichnisse mit gemeinsamen Bibliotheken enthält.
   - So sorgt `ldconfig` dafür, dass der dynamische Linker schnell und korrekt die benötigten Shared Libraries findet.
   - Obwohl bei statischen Bibliotheken (`.a`) normalerweise nicht zwingend nötig, wird es oft als Standard-Schritt beim Installieren neuer Libraries ausgeführt.

---

### Task 5: Installing and Using `jansson`

**Objective:** Install a third-party JSON library and link against it.

1. Install via `apt`:

   ```bash
   sudo apt update && sudo apt install libjansson-dev
   ```
2. Create `solutions/json_main.c`:

   ```c
   #include <jansson.h>
   #include <stdio.h>
   int main(void) {
       json_t *root = json_pack("{s:i, s:s}", "id", 1, "name", "Alice");
       char *dump = json_dumps(root, 0);
       printf("%s\n", dump);
       free(dump);
       json_decref(root);
       return 0;
   }
   ```
3. Compile and run:

   ```bash
   gcc -o solutions/json_main solutions/json_main.c -ljansson
   ./solutions/json_main
   ```

#### Reflection Questions

1. **What files does `libjansson-dev` install, and where?**
   **Lösung zu 1:**
   `libjansson-dev` installiert:
   - Head-Dateien, z. B. `/usr/inlcude/jansson.h`
   - Die statische Bibliothek `libjansson.a`
   - Die dynamische Bibliothek `libjansson.so`
   - Manchmal auch pkg-config Dateien (`.pc`)
   
2. **How does the linker know where to find `-ljansson`?**
   **Lösung zu 2:**
   - Der Linker sucht standardmäßig in den Systembibliothekspfaden wie `/usr/lib`, `/usr/local/lib` oder distributionsspezifischen Unterordnern.
   - Da `libjansson.so` oder `libjansson.a` dort installiert sind, kann der Linker sie automatisch finden.
   - 

---

### Task 6: Building and Installing a GitHub Library

**Objective:** Download, build, and install a library from GitHub using its Makefile.

1. Choose a small C library on GitHub (e.g., `sesh/strbuf`).
2. Clone and build:

   ```bash
   git clone https://github.com/sesh/strbuf.git
   cd strbuf
   make
   ```
3. Install to system paths:

   ```bash
   sudo make install PREFIX=/usr/local
   sudo ldconfig
   ```
4. Write `solutions/strbuf_main.c` that includes `strbuf.h`, uses its API, and prints a test string.
5. Compile and link:

   ```bash
   gcc -o solutions/strbuf_main solutions/strbuf_main.c -lstrbuf
   ./solutions/strbuf_main
   ```

#### Reflection Questions

1. **What does `make install` do, and how does `PREFIX` affect installation paths?**
2. **How can you inspect a library’s exported symbols to verify installation?**

---

**Remember:** Stop after **90 minutes** and record where you stopped.
