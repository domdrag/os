Situacija 1.
Ukoliko maknemo prve komentare iz main-a dobivamo ispis:

Zbrckanost broja 7 = 1
Zbrckanost broja 1231 = 3
Zbrckanost broja 223421 = 5
Zbrckanost broja 657655514 = 7
Zbrckanost broja 9555449 = 6,


što odgovara prvoj ideji za implementaciju zbrčkanosti (gledamo prozore po 4 bita i zbrajamo jedinice) jer:
7=000...0111 
1231=000...0100 1100 1111
223421=000...0011 0110 1000 1011 1101
657655514=000...0010 0111 0011 0011 0000 0110 1101 1010
9555449=000...1001 0001 1100 1101 1111 1001

Situacija 2.
Ukoliko maknemo druge komentare iz main-a dobivamo ispis:

TIME: 0.396834
TIME: 0.399251
TIME: 0.399443
TIME: 0.401286
TIME: 0.397836
TIME: 0.400145
TIME: 0.398281
TIME: 0.404069
TIME: 0.394514
TIME: 0.398103

što znači da, nakon procjene velicine grupe, za generiranje jednog broja je potrebno ~0.4 sekunde što pak odgovara činjenici da je potrebno u jednoj sekundi izgenerirati 2-3 broja.

Situacija default.
Velicina grupe: 11 480. Dobivamo ispis 10 prostih brojeva u heksadekadskom obliku. Za ispis svakog broja je potrebno ~1 sekunda.
