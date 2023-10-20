# Spustenie:
C - `comp.bat`(windows) <br>
# Progress:

## 7.10.2023
Edit Python scriptu pre generovanie test. vektorov (pre modl_l_inverse).<br>
Script je spraveny tak, aby po jeho spusteni automaticky zmenil hodnoty v subore `test_config.h`.<br>
Script sa spusta klasicky pomocou cmd prikazu `python modl_l_inverse.py`.<br>
Cistenie kodu  + pridanie komentarov ku kodu.<br>
PS:
  V hlavickach nie je zatial zmeneny datu poslednej upravy/ verzie, pracujem na zautomatizovani.<br>
<br>

## 7.10.2023
Dodany komentar k premennym `r` a `k`.<br>
<br>

## 6.10.2023
Pridany pokec ku kodom<br>
Pridanie python impl. pre modL inverse<br>
TODO:
	- Nie je mi celkom jasne ako autori kniznice monocypher<br>
	dostali hodnoty k premennym [`r`](https://github.com/Alg0ritmus/ristretto255_cyclone/blob/main/modl.c#L111) a [`k`](https://github.com/Alg0ritmus/ristretto255_cyclone/blob/main/modl.c#L195), resp. nieco mi unika...<br>

## 24.9.2023
Pridany pokec + info o hlbke zasobnika (mozno prediskutovat)<br>
Dodany test na inverziu.<br>
Select pouziva mensi stack-size ako swap (\~4B), nechceme radsej select?<br>


## 22.9.2023
Nedokoncene!!!<br>
Spravil som basic makefile + nahradil select swapom<br>
Musim dopisat pokec ku config.h<br>
Dopisat commenty podls RFC + docistit kod<br>


## 18.9.2023
Pridany pokec + zabudol som na MEMORY ALLOCATION kt. bude pridane v dalsej verzii dokumentu spolu s pripadnymi opravami.<br>

## 17.9.2023
Precisteny kod + premenovane subory/funkcie. <br>
Vyhodena donna impl. pre `pow2523` ako aj stara impl. `inv_sqrt`.<br>
Pridana nova impl. `inv_sqr` inspirovana kniznicou cyclone.<br>
Upraveny prepinac pre big/little endian. <br>
Upravena `crypto_modl_inverse` na jej funkcnu verziu.<br>
Pridany prepinac na podmienenu kompilaciu modl bud to pomocou<br>
Barretovej redukcie alebo Montgomeryho. <br>
Doplnenie komentarov ku kodom. <br>
<br>

---
## Mozno stoji za uvahu
Je vazne potrebne vyuzivat `gf25519Swap` a aj `gf25519Select` z cyclonu? Vhodnym nahradenim by nam malo stacit pouzit `gf25519Swap`  (vice versa) a tym by sme zmensili velkost zdrojaku. Ma to ale zmysel? Ak by sme isli tymto smerom, ktora funkcia je pre nas vyhodnejsia?




## 11.9.2023
Vyhodene nepotrebne funkcie zo cyclone.<br>
Pridana vypoctovo vyhodnejsia donna impl. pre pow2523.<br>
Pridane "cistenie premennych" -> WIPE, WIPE_PT. <br>
Pridany prepinac pre big endian (prva verzia). <br>
Pridana funkcia "crypto_modl_inverse" -> modl inverzia - Montgomery (funkcne?!). <br>

## 24.7.2023
Fungujuca verzia ristretta255 s vyuzitim kniznice cyclone.<br>
Vykonal som množstvo mensich zmien, kt. bolo potrebne vykonat vzhladom na "hybridnost" kodu, momentalne kod nie je v stave kedy by bolo mozne cyclone jednoducho zameniť  za tweetNaCl. Kod je teda potrebne este trosku poupravit, co planujem spravit v blizkej buducnosti.<br>
