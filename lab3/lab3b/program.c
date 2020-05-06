#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#include "slucajni_prosti_broj.h"

#define MASKA(bitova)			(-1 + (1<<(bitova)) )
#define UZMIBITOVE(broj,prvi,bitova) 	( ( (broj) >> prvi ) & MASKA(bitova) )

struct gmp_pomocno p;
uint64_t velicina_grupe;
int kraj = 0;
pthread_mutex_t m;
pthread_cond_t red[2];
uint64_t br_punih = 0;
uint64_t br_praznih = 5;

struct Buffer
{
        uint64_t MS[5];
        uint64_t ulaz,izlaz;
        uint64_t brojac;
};

void inicijaliziraj_buffer(struct Buffer* b){
        b->ulaz=0;
        b->izlaz=0;
        b->brojac = 0;
        return;
}

void stavi_u_MS(struct Buffer* b,uint64_t broj){
        b->MS[b->ulaz]=broj;
        if (b->ulaz==4) b->ulaz=0;
        else ++(b->ulaz);
        ++(b->brojac);
        if (b->brojac > 5){
            --(b->brojac);
            if (b->izlaz==4) b->izlaz=0;
            else ++(b->izlaz);
        }
        return;
}

uint64_t uzmi_iz_MS(struct Buffer* b){
        uint64_t br=b->MS[b->izlaz];
        if (b->brojac > 0){
            if (b->izlaz == 4) b->izlaz=0;
            else ++(b->izlaz);
            --(b->brojac);
        }
        return br;
}

struct Buffer Main_Buffer;

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

uint64_t generiraj_dobar_broj(struct gmp_pomocno *g){
        uint64_t najbolji_broj=0, broj;
    	uint64_t najbolja_zbrckanost=0, z;
    	uint64_t i;

    	for (i=0;i < velicina_grupe; ++i){
        	broj=daj_novi_slucajan_prosti_broj(g);
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
            		broj=generiraj_dobar_broj(&p);
            		stavi_u_MS(&buffer,broj);
        	}
    	}
    	float brojeva_u_sekundi=(k*M)/sekundi;
    	velicina_grupe=brojeva_u_sekundi*((float)2/(float)5);
    	return;
}
/*void provjera_zbrckanosti(){
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
}*/

void *radna_dretva(void *rbr){
        int *d = rbr;
        struct gmp_pomocno p;
        inicijaliziraj_generator (&p, *d);
        do {
            uint64_t broj = generiraj_dobar_broj(&p);
            pthread_mutex_lock(&m);
            while (br_praznih == 0)
				pthread_cond_wait(&red[0], &m);

            //Ako je kraj, izbjegavamo koristenje MS-a
            if(kraj){
                pthread_cond_signal(&red[0]);
                pthread_cond_signal(&red[1]);
                pthread_mutex_unlock(&m);
                break;
            }

            stavi_u_MS(&Main_Buffer, broj);
            printf("stavio %" PRIx64 "\n", broj);

            ++br_punih;
            --br_praznih;
            pthread_cond_signal(&red[1]);
            pthread_mutex_unlock(&m);
        } while (kraj != 1);

        obrisi_generator(&p);

		return NULL;
}

void *neradna_dretva(void *rbr){

        do{
            sleep(3);

            pthread_mutex_lock(&m);
            while (br_punih == 0)
                pthread_cond_wait(&red[1], &m);

            //Ako je kraj, izbjegavamo koristenje MS-a
            if(kraj){
                pthread_cond_signal(&red[0]);
                pthread_cond_signal(&red[1]);
                pthread_mutex_unlock(&m);
                break;
            }

            //printf("Ulaz %" PRIx64 "\n", Main_Buffer.ulaz);
            //printf("Izlaz %" PRIx64 "\n", Main_Buffer.izlaz);
            uint64_t broj = uzmi_iz_MS(&Main_Buffer);
            printf("uzeo %" PRIx64 "\n", broj);

            ++br_praznih;
            --br_punih;
            pthread_cond_signal(&red[0]);
            pthread_mutex_unlock(&m);
        } while (kraj != 1);

        return NULL;
}

int main(int argc, char *argv[])
{
        int i, BR[6];
        pthread_t t[6];

        inicijaliziraj_buffer(&Main_Buffer);
        inicijaliziraj_generator(&p, 0);
        printf("Procjena velicine grupe (~10sec)...\n");
        procjeni_velicinu_grupe();
        printf("Velicina grupe: %ld\n",velicina_grupe);

        pthread_mutex_init (&m, NULL);
        pthread_cond_init (&red[0], NULL);
        pthread_cond_init (&red[1], NULL);

        for (i = 0; i < 3; i++) {
            BR[i] = i;
            if (pthread_create (&t[i], NULL, radna_dretva, &BR[i])) {
                printf("Ne mogu stvoriti novu radnu dretvu!\n");
                exit(1);
            }
        }

        for (i = 3; i < 6; i++) {
            BR[i] = i;
            if (pthread_create (&t[i], NULL, neradna_dretva, &BR[i])) {
                printf("Ne mogu stvoriti novu neradnu dretvu!\n");
                exit(1);
            }
        }

        sleep(20);
        kraj = 1;

        pthread_cond_broadcast(&red[0]);
        pthread_cond_broadcast(&red[1]);
        //Osiguravamo da dretve izadju iz petlji ako je kraj i da prestanu s radom
        br_punih=1;
        br_praznih=1;

        for (i = 0; i < 6; i++)
            pthread_join(t[i], NULL);

        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&red[0]);
        pthread_cond_destroy(&red[1]);

        obrisi_generator(&p);
        return 0;
}

