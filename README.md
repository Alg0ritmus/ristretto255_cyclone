# Spustenie:
C - `comp.bat`(windows) <br>
# Progress:

## 11.9.2023
Vyhodene nepotrebne funkcie zo cyclone.<br>
Pridana vypoctovo vyhodnejsia donna impl. pre pow2523.<br>
Pridane "cistenie premennych" -> WIPE, WIPE_PT. <br>
Pridany prepinac pre big endian (prva verzia). <br>
Pridana funkcia "crypto_modl_inverse" -> modl inverzia - Montgomery (funkcne?!). <br>

## 24.7.2023
Fungujuca verzia ristretta255 s vyuzitim kniznice cyclone.<br>
Vykonal som množstvo mensich zmien, kt. bolo potrebne vykonat vzhladom na "hybridnost" kodu, momentalne kod nie je v stave kedy by bolo mozne cyclone jednoducho zameniť  za tweetNaCl. Kod je teda potrebne este trosku poupravit, co planujem spravit v blizkej buducnosti.<br>
