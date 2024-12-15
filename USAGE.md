# Tema 2 - Schelet de cod

Scheletul cuprinde următoarele directoare:

- `scripts/`, `checker/`, `tests/` - necesare pentru validarea temei
- `libchunk/` - conține fișierele în care trebuie să completați funcțiile
- `src/` - conține un fișier `main.c` în care puteți testa funcțiile din `libchunk`
    - acest fișier nu face parte din temă și nu este obligatoriu să îl folosiți

De asemenea, următoarele instrumente:

- `setup.sh` - instalează pachetele necesare pentru celelalte scripturi
- `check.sh` - rulează checkerul local
- `view3d.sh` - vizualizează un chunk
- `archive.sh` - arhivează tema în vederea trimiterii pe Moodle

Și un fișier `Makefile` pentru a compila `libchunk` și `src/main.c`.

**Important!** Înainte de a folosi alte instrumente, trebuie să instalați pachetele necesare executând scriptul `setup.sh`:

```bash
./setup.sh
```

Scriptul de setup cere parolă pentru `sudo`.

### Compilare și testare manuală

Pentru a vă testa manual implementarea cu propriile date de intrare, puteți folosi în fișierul `src/main.c` funcțiile din `libchunk`.

Fișierul `src/main.c` **nu** va fi încărcat pe Moodle, există doar pentru a vă ajuta să apelați cu ce date doriți funcțiile implementate. Puteți scrie orice în acest fișier.

Pentru a compila programul scris în `src/main.c` (precum și biblioteca `libchunk`), rulați următoarea comandă:

```bash
make
```

Programul rezultat poate fi executat astfel:

```bash
build/main <chunk.in | ./view3d.sh
```

## Utilizare checker

Puteți valida local toate taskurile folosind comanda:

```bash
./check.sh
```

## Utilizare 3D Viewer

Pentru a vizualiza în interfața grafică un fișier ce conține un chunk, puteți executa:

```bash
./view3d.sh chunk.txt
```

Dacă omiteți fișierul, scriptul va citi datele unui chunk de la intrarea standard.

Dacă folosiți opțiunea `-o`, scriptul de 3D view va salva o imagine în loc să deschidă interfața:

```bash
./view3d.sh chunk.txt -o figure.png
```

Puteți analiza cu `view3d.sh` datele de intrare și ieșire ale testelor. Le găsiți în directorul `tests/`.

## Vizualizare cod binar

În cadrul taskurilor 9 și 10 veți lucra cu șiruri de byți într-un format binar. Ca atare, testele sunt fișiere binare și pot fi vizualizate astfel:

```bash
xxd -b cod_binar.txt
```

## Utilizare arhivator

Pentru a trimite tema, puteți genera arhiva executând:

```bash
./archive.sh
```

Va fi creată o arhivă ZIP numită `tema2.zip` pe care o puteți încărca direct pe Moodle, conform instrucțiunilor din cerință.

