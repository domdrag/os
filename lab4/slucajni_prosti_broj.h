#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>

struct gmp_pomocno
{
    	gmp_randstate_t stanje;
    	mpz_t slucajan_broj,prosti_broj;
};

void inicijaliziraj_generator(struct gmp_pomocno *p, unsigned id);
void obrisi_generator(struct gmp_pomocno *p);
uint64_t daj_novi_slucajan_prosti_broj(struct gmp_pomocno *p);








