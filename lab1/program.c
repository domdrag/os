#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>

#include "slucajni_prosti_broj.h"

#define MASKA(bitova)			(-1 + (1<<(bitova)) )
#define UZMIBITOVE(broj,prvi,bitova) 	( ( (broj) >> prvi ) & MASKA(bitova) )

struct gmp_pomocno p;
uint64_t velicina_grupe;

struct Buffer
{
    	uint64_t MS[5];
	uint64_t ulaz,izlaz;
};

void inicijaliziraj_buffer(struct Buffer* b){
	b->ulaz=0;
	b->izlaz=0;
	return;
}

void stavi_u_MS(struct Buffer* b,uint64_t broj){
	b->MS[b->ulaz]=broj;
    	if (b->ulaz==4) b->ulaz=0;
    	else b->ulaz++;
    	return;
}

uint64_t uzmi_iz_MS(struct Buffer* b){
    	uint64_t br=b->MS[b->izlaz];
    	if (b->izlaz==4) b->izlaz=0;
    	else b->izlaz++;
    	return br;
}

//1. ideja za zbrckanost na Vjezbe-1
uint64_t zbrckanost (uint64_t x){
	uint64_t i,j,pn,b1=0,b2=0,zbr=0,beg=1;
    	for (i=0;i<64;i+=4){
        	pn=UZMIBITOVE(x,(64-i-4),4);
        	for (j=0;j<4;++j){
            		if ( (1<<j) & pn) b1++;
        	}
        	if (beg){ b2=b1; beg=0; continue;}
        	if (b2!=b1) zbr++;
		b2=b1;
		b1=0;
    	}
    	return zbr;
}

uint64_t generiraj_dobar_broj(){
   	uint64_t najbolji_broj=0, broj;
    	uint64_t najbolja_zbrckanost=0, z;
    	uint64_t i;

    	for (i=0;i < velicina_grupe; ++i){
        	broj=daj_novi_slucajan_prosti_broj(&p);
        	z=zbrckanost(broj);
        	if (z > najbolja_zbrckanost){
            		najbolja_zbrckanost=z;
            		najbolji_broj=broj;
        	}
    	}
    	return najbolji_broj;
}

void procjeni_velicinu_grupe(){
    	struct Buffer buffer;
    	inicijaliziraj_buffer(&buffer);
    	float M=1000,sekundi=10,k=0.0f,i;
    	velicina_grupe=1;
    	uint64_t broj;
    	while ((float)clock()/CLOCKS_PER_SEC < sekundi ){
        	k++;
        	for (i=0;i<M;i++){
            		broj=generiraj_dobar_broj();
            		stavi_u_MS(&buffer,broj);
        	}
    	}
    	float brojeva_u_sekundi=(k*M)/sekundi;
    	velicina_grupe=brojeva_u_sekundi*((float)2/(float)5);
    	return;
}
void provjera_zbrckanosti(){
	printf("Zbrckanost broja 7 = %ld\n",zbrckanost(7));
	printf("Zbrckanost broja 1231 = %ld\n",zbrckanost(1231));
	printf("Zbrckanost broja 223421 = %ld\n",zbrckanost(223421));
	printf("Zbrckanost broja 657655514 = %ld\n",zbrckanost(657655514));
	printf("Zbrckanost broja 9555449 = %ld\n",zbrckanost(9555449));
	return;
}

void provjera_vremena_za_generiranje_jednog_broja(){
	clock_t beginning,end;
    	int i;
    	for (i=0;i<10;++i) {
		beginning=clock();
    		generiraj_dobar_broj(&p);
		end=clock();
		float dif=(float)(end-beginning)/CLOCKS_PER_SEC;
        	printf("TIME: %.6f\n",dif);
		beginning=clock();
    	}
	return;
}

int main()
{
    	inicijaliziraj_generator(&p,0);
	//provjera_zbrckanosti();
    	struct Buffer Main_Buffer;
    	inicijaliziraj_buffer(&Main_Buffer);
    	printf("Procjena velicine grupe (~10sec)...\n");
    	procjeni_velicinu_grupe();
	//provjera_vremena_za_generiranje_jednog_broja();
    	uint64_t start=(uint64_t)clock()/CLOCKS_PER_SEC;
    	uint64_t broj_ispisa=0,broj;
    	while (broj_ispisa<10){
        	broj=generiraj_dobar_broj();
        	stavi_u_MS(&Main_Buffer,broj);
        	uint64_t current = (uint64_t)clock()/CLOCKS_PER_SEC;
        	if (current!=start){
            		broj=uzmi_iz_MS(&Main_Buffer);
	    		printf("%" PRIx64 "\n",broj);
            		broj_ispisa++;
            		start=(uint64_t)clock()/CLOCKS_PER_SEC;
        	}
    	}
    	return 0;

}

