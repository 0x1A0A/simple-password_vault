#include "file.h"
#include "crypto/sha256.h"
#include "crypto/chacha20.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern account_list_t *account_list;
extern tag_list_t *tag_list;
extern email_list_t *email_list;
extern struct df_sha256context *sha;
extern struct chacha20 *cypher;
extern byte password[256];
extern byte nonce[12];
extern byte hash[32];

static void chachaCipher(byte *datain, size_t length)
{
	int *index = &cypher->index;
	int i=0;

	while ( i < length ) {
		if (*index % 128 == 0 ) {
			chacha20_key_stream_gen(cypher);
			++(cypher->block.counter);
		}

		//printf("%d %02x ^ %02x -> ", i, datain[i], ((byte*)(cypher->stream))[(*index)%128]);

		datain[i] ^= ((byte*)(cypher->stream))[(*index)%128];

		//printf("%02x\n", datain[i]);
		++(*index);
		++i;
	}
}

static tag_t* take_tag(FILE *file)
{
	tag_t *neww = (tag_t*)malloc( sizeof(tag_t) );

	fread( &neww->id, 1, 1, file );
	fread( &neww->nl, 1, 1, file );

	chachaCipher( &neww->id, 1 );
	chachaCipher( &neww->nl, 1 );

	neww->name = (char*)malloc(neww->nl);
	fread( neww->name, 1, neww->nl, file);

	chachaCipher( neww->name, neww->nl );

	return neww;
}

static email_t* take_email(FILE *file)
{
	email_t *neww = (email_t*)malloc(sizeof(email_t));
	fread(&neww->id, 1, 1, file);
	fread(&neww->nl, 1, 1, file); 
	fread(&neww->dl, 1, 1, file); 
	fread(&neww->pl, 1, 1, file);

	chachaCipher(&neww->id,1);
	chachaCipher(&neww->nl,1);
	chachaCipher(&neww->dl,1);
	chachaCipher(&neww->pl,1);

	neww->local = (char*)malloc(neww->nl + neww->dl + neww->pl +1);
	neww->domain = neww->local + neww->nl + 1;
	neww->password = neww->local + neww->nl + neww->dl + 1;
	fread(neww->local, 1, neww->nl + neww->dl + neww->pl +1, file);

	chachaCipher(neww->local, neww->nl + neww->dl + neww->pl +1);

	return neww;
}

static account_t* take_account(FILE *file)
{
	account_t *neww = (account_t*)malloc(sizeof(account_t));

	fread(&neww->email_id, 1, 1, file);
	fread(&neww->tag_id, 1, 1, file);
	fread(&neww->nl, 1, 1, file);
	fread(&neww->ul, 1, 1, file);
	fread(&neww->pl, 1, 1, file);

	chachaCipher(&neww->email_id, 1);
	chachaCipher(&neww->tag_id, 1);
	chachaCipher(&neww->nl, 1);
	chachaCipher(&neww->ul, 1);
	chachaCipher(&neww->pl, 1);

	neww->name = (char*)malloc(neww->nl + neww->ul + neww->pl);
	neww->user = neww->name + neww->nl;
	neww->password = neww->user + neww->ul;
	fread(neww->name, 1, neww->nl + neww->ul + neww->pl, file);

	chachaCipher(neww->name, neww->nl + neww->ul + neww->pl);

	return neww;
}

void load_file( const char *path )
{
	FILE *file = fopen( path, "r" );
	
	if (!file) {
		fprintf(stderr, "can't open file for reading: %s\n", path);
		return;
	}

	cypher->index = 0;	
	size_t i = 0;
	while (i++ < 8) fgetc(file);
	
	byte data[32];	
	uint8_t BUFF;
	
	// hash password
	fread(data, 1, 32, file);
	chachaCipher(data, 32);

	// for ( int j=0; j<32; ++j ) printf("%02x", data[j]);
	// puts("");
	// for (int j=0; j<32; ++j) printf("%02x", hash[j]);
	// puts("");

	if ( strncmp( hash, data, 32 ) ) {
		fprintf( stderr, "password incorrect\n" );
		return ;
	}

	// tag
	fread(&BUFF, 1, 1, file);
	chachaCipher(&BUFF, 1);
	while (BUFF--) tag_add( take_tag(file), tag_list );
	// email
	fread(&BUFF, 1, 1, file);
	chachaCipher(&BUFF, 1);
	while (BUFF--) email_add( take_email(file), email_list );
	// account
	fread(&BUFF, 1, 1, file);
	chachaCipher(&BUFF, 1);
	while (BUFF--) account_add( take_account(file), account_list );

	fclose(file);
}

void save_file( const char *path )
{
	FILE *file = fopen( path, "wb" );

	if (!file) {
		fprintf(stderr, "can't open file for writing: %s\n", path);
		return;
	}

	cypher->index = 0;
	size_t i = 0;

	// empty first 8 byte with no reason
	while (i++ < 8) fputc(0, file);
	
	byte data[256];
	size_t BUFF;
	// add 32 byte for hashed core password
	memcpy(data, hash, 32);
	chachaCipher(data, 32);
	fwrite(data, 1, 32, file);

	// for ( int j=0; j<64; ++j ) printf("%02x", cypher->stream[j]);
	// puts("");
	// for (int j=0; j<32; ++j) printf("%02x", hash[j]);
	// puts("");
	// for (int j=0; j<32; ++j) printf("%02x", data[j]);

	// fwrite(hash, 1, 32, file);
	
	// for (int j=0; j<32; ++j) printf("%0x", hash[j]);

	// data tag >> email >> account

	BUFF = data[0] = 0;//tl->count;
	chachaCipher(data, 1);
	fwrite(data, 1, 1, file);

	if (BUFF) { 
		tag_t **trace = &tag_list->head;
		while (*trace) {
			// fwrite( &(*trace)->id, 1, 1, file );
			// fwrite( &(*trace)->nl, 1, 1, file );
			// fwrite( (*trace)->name, 1, (*trace)->nl, file );
			
			data[0] =  (*trace)->id;
			data[1] =  (*trace)->nl;
			memcpy( data+2, (*trace)->name, data[1]);

			chachaCipher(data, 2+data[1]);

			fwrite( data, 1, 2+(*trace)->nl, file );

			trace = &(*trace)->next;
		}
	}

	BUFF = data[0] = email_list->count;
	chachaCipher(data, 1);
	fwrite(data, 1, 1, file);

	if (BUFF) { 
		email_t **trace = &email_list->head;
		while (*trace) {
			// fwrite( &(*trace)->id, 1, 1, file );
			// fwrite( &(*trace)->nl, 1, 1, file );
			// fwrite( &(*trace)->dl, 1, 1, file );
			// fwrite( &(*trace)->pl, 1, 1, file );
			// fwrite( (*trace)->local, 1, (*trace)->nl + (*trace)->dl + (*trace)->pl + 1, file );

			data[0] =  (*trace)->id;
			data[1] =  (*trace)->nl;
			data[2] =  (*trace)->dl;
			data[3] =  (*trace)->pl;			

			memcpy( data+4, (*trace)->local, data[1] + data[2] + data[3] + 1);
			chachaCipher(data, 4 + data[1] + data[2] + data[3] + 1);

			fwrite( data, 1, 4 + (*trace)->nl + (*trace)->dl + (*trace)->pl + 1, file );

			trace = &(*trace)->next;
		}
	}

	BUFF = data[0] = 0;//al->count;
	chachaCipher(data, 1);
	fwrite(data, 1, 1, file);

	if (BUFF) { 
		account_t **trace = &account_list->head;
		while (*trace) {
			// fwrite( &(*trace)->email_id, 1, 1, file );
			// fwrite( &(*trace)->tag_id, 1, 1, file );
			// fwrite( &(*trace)->nl, 1, 1, file );
			// fwrite( &(*trace)->ul, 1, 1, file );
			// fwrite( &(*trace)->pl, 1, 1, file );
			// fwrite( (*trace)->name, 1, (*trace)->nl + (*trace)->ul + (*trace)->pl, file );

			data[0] = (*trace)->email_id;
			data[1] = (*trace)->tag_id;
			data[2] = (*trace)->nl;
			data[3] = (*trace)->ul;
			data[4] = (*trace)->pl;

			memcpy( data+5, (*trace)->name, (*trace)->nl + (*trace)->ul + (*trace)->pl );

			fwrite( data, 1, 5 + (*trace)->nl + (*trace)->ul + (*trace)->pl, file );

			trace = &(*trace)->next;
		}
	}

	fclose(file);
}