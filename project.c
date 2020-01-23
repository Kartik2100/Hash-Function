#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>

#define BUFFER_SIZE	1024

int collisions;
int duplicates;

struct record
{
  char *last_name;
  char *first_name;
  char *license_type;
};

struct array
{
  struct record *arr;
  int nelements;
  struct record **hash;
  int hash_size;
};

int char2int( unsigned char c )
{
  if ( isupper(c) )
  {
    return (int)(c - 'A');
  }
  if ( islower(c) )
  {
    return (int)(c - 'a');
  }
  return 26;
}

int str2int( char *string, int hash_size )
{
  char *c;
  unsigned long number, column, new, val;
  int num;

  column = 1;
  number = 0;
  for (c=string;(*c);c++)
  {
    if ( isupper(*c) )
    {
      num = (int)(*c - 'A');
    }
    else if ( islower(*c) )
    {
      num = (int)(*c - 'a');
    }
    else{
      num = 26;
    }

    if(*c == 'a'){
     val = num + 2;
    }
    else if(isupper(*c)){
     val = num + 5;
    }
    else if(*c == '"'){
     val = num + 2;
    }
    else if(*c == '1'){
     val = num + 1;
    }
    else if(*c == '2'){
     val = num + 2;
    }
    else if(*c == '3'){
     val = num + 3;
    }
    else if(*c == '4'){
     val = num + 4;
    }
    else if(*c == '5'){
     val = num + 5*2;
    }
    else if(*c == '6'){
     val = num + 6*2;
    }
    else if(*c == '7'){
     val = num + 7*2;
    }
    else if(*c == '8'){
     val = num + 8;
    }
    else if(*c == '9'){
     val = num + 9*2;
    }
    else{
     val = num;
    }

    number += val * column;
    column *= 27;
    number = number << 1;
  }

  new = 0;
  while (number)
  {
    new =( number + (new % hash_size)) % hash_size;
    number = (number / hash_size);
  }



  return (int)new;

}

struct array *read_records()
{
  char buffer[BUFFER_SIZE];

  struct array *arrptr;

  FILE *fp;

  int line, start, end;

  arrptr = malloc( sizeof( struct array ) );
  arrptr->nelements = 0;

  fp = fopen( "Professional_and_Occupational_Licensing.csv", "r" );
  fgets( buffer, BUFFER_SIZE, fp );

  while (!feof(fp))
  {
    if ( fgets( buffer, BUFFER_SIZE, fp )==NULL )
    {
      break;
    }

    if ( strlen(buffer)==BUFFER_SIZE-1 )
    {
      fprintf( stderr, "Error:  BUFFER TOO SMALL\n" );
      exit(-1);
    }

    (arrptr->nelements)++;
  }

  arrptr->arr = malloc( sizeof( struct record ) * (arrptr->nelements) );

  fseek( fp, 0, SEEK_SET );

  fgets( buffer, BUFFER_SIZE, fp );
  for (line=0;line<arrptr->nelements;line++)
  {
    fgets( buffer, BUFFER_SIZE, fp );

    start = 0;
    for (end=start;buffer[end]!=',';end++);	/* find next comma */

    (arrptr->arr)[line].last_name = malloc( end - start + 1 );
    strncpy( (arrptr->arr)[line].last_name, buffer+start, end-start );
    (arrptr->arr)[line].last_name[end-start] = '\0';

    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    (arrptr->arr)[line].first_name = malloc( end - start + 1 );
    strncpy( (arrptr->arr)[line].first_name, buffer+start, end-start );
    (arrptr->arr)[line].first_name[end-start] = '\0';


    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    start = end+1;
    for (end=start;buffer[end]!=',';end++); /* find next comma */

    (arrptr->arr)[line].license_type = malloc( end - start + 1 );
    strncpy( (arrptr->arr)[line].license_type, buffer+start, end-start );
    (arrptr->arr)[line].license_type[end-start] = '\0';
    
  }


  return arrptr;
}


void build_hash( struct array *arrptr, int hash_size )
{
  int idx, line;
  int duplicate;

  arrptr->hash_size = hash_size;
  arrptr->hash = malloc( sizeof( struct record * ) * arrptr->hash_size );

  for (idx=0;idx<arrptr->hash_size;idx++)
  {
    (arrptr->hash)[idx] = NULL;
  }

  for (line=0;line<arrptr->nelements;line++)
  {
    printf( "%d Adding %s\n", line, (arrptr->arr)[line].last_name );
    idx = str2int( (arrptr->arr)[line].last_name, arrptr->hash_size );

    duplicate = 0;
    while ( (arrptr->hash)[idx]!=NULL )
    {
      if ( strcmp( ((arrptr->hash)[idx])->last_name,
	            (arrptr->arr)[line].last_name ) == 0 )
      {
	printf( "  Skipping duplicate\n" );
	duplicates++;
	duplicate = 1;
	break;
      }
      printf( "  collision at %d %s\n", idx, ((arrptr->hash)[idx])->last_name );
      collisions++;
      idx++;
      if (idx>=arrptr->hash_size)
      {
	idx = 0;
      }
      
    }
    if (!duplicate)
    {
      printf("  inserting at %d\n", idx );
      (arrptr->hash)[idx] = (arrptr->arr)+line;
    }
  }

}

void free_array_ptr( struct array *ptr )
{
  int i;

  for (i=0;i<ptr->nelements;i++)
  {
    free( ptr->arr[i].last_name );
    free( ptr->arr[i].first_name );
    free( ptr->arr[i].license_type );
  }

  free( ptr->arr );
  free( ptr->hash );

  free( ptr );
}

struct record *find( char *key, struct array *arrptr )
{
  int idx;

  idx = str2int( key, arrptr->hash_size );
  while ( (arrptr->hash)[idx] != NULL )
  {
    if ( strcmp( key, ((arrptr->hash)[idx])->last_name )==0 )
    {
      return (arrptr->hash)[idx];
    }
    idx++;

    if (idx>=(arrptr->hash_size))
    {
      idx = 0;
    }
  }
  return NULL;

}


int main()
{
  struct array *arrptr;
/*  int line; */
  struct record *r;

  collisions = 0;
  duplicates = 0;

  arrptr = read_records();

  build_hash( arrptr, 500000 );
/*
  for (line=0;line<arrptr->nelements;line++)
  {
    printf( " %d %s, %s: %s %d\n", line,
	                    (arrptr->arr)[line].last_name,
	                    (arrptr->arr)[line].first_name,
			    (arrptr->arr)[line].license_type,
       			    str2int( (arrptr->arr)[line].last_name, 100 ) );
  }
*/

  printf( "Duplicates: %d\n", duplicates );
  printf( "Collisions: %d\n", collisions );

  r = find( "Kremalskdfjalsdkfjer", arrptr );
  if (r==NULL)
  {
    printf( "Not found\n" );
  }
  else
  {
    printf( "%s, %s : %s\n", r->last_name,
                             r->first_name,
			     r->license_type );
  }

  free_array_ptr( arrptr );
  return 0;
}

