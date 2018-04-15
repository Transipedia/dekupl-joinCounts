#include <stdio.h>  //fprintf
#include <stdlib.h> //free
#include <zlib.h>

#include "kstring.h"
#include "kseq.h"

#define VERSION "0.0.1"

KSEQ_INIT(gzFile, gzread)

typedef struct {
  gzFile  fp;
  kstream_t *ks;
  char    *kmer;
  int     count;
} it_t;

int main(int argc, char *argv[]) {
  int min_recurrence = 1;
  int min_recurrence_abundance = 1;

  int c;
  while ((c = getopt(argc, argv, "r:a:")) >= 0) {
    switch (c) {
      case 'r': min_recurrence = atoi(optarg); break;
      case 'a': min_recurrence_abundance = atoi(optarg); break;
    }
  }

  if (optind == argc) {
		fprintf(stderr, "\n");
		fprintf(stderr, "Usage:   joinCounts [options] <counts.tsv> [<counts.tsv> ...]\n\n");
		fprintf(stderr, "Options: -r INT    min recurrence [%d]\n", min_recurrence);
    fprintf(stderr, "Options: -a INT    min recurrence abundance [%d]\n", min_recurrence_abundance);
		fprintf(stderr, "\n");
		return 1;
	}

  int nb_count_files = argc - optind;
  int dret = 0, i;

  it_t *counts_it = calloc(nb_count_files, sizeof(it_t));
  int  *counts    = calloc(nb_count_files, sizeof(int));
  kstring_t *str;
  kstring_t min_kmer = { 0, 0, NULL }, next_min_kmer = { 0, 0, NULL };
  str = calloc(1, sizeof(kstring_t));

  for(i = 0; i < nb_count_files; i++) {
    char *counts_file = argv[optind++];
    counts_it[i].fp = gzopen(counts_file, "r");
    if(!counts_it[i].fp) { fprintf(stderr, "Failed to open %s\n", counts_file); exit(EXIT_FAILURE); }
    counts_it[i].ks = ks_init(counts_it[i].fp);

    ks_getuntil(counts_it[i].ks, 0, str, &dret);
    counts_it[i].kmer = ks_release(str);

    // Set min_kmer
    if(min_kmer.l == 0 || strcmp(counts_it[i].kmer,min_kmer.s) < 0) {
      min_kmer.l = 0;
      kputs(counts_it[i].kmer,&min_kmer);
    }

    ks_getuntil(counts_it[i].ks, 0, str, &dret);
    counts_it[i].count = atoi(str->s);

    // skip the rest of the line
  	if (dret != '\n') while ((dret = ks_getc(counts_it[i].ks)) > 0 && dret != '\n');
  }

  int read = 1;
  while(read) {
    read = 0;
    next_min_kmer.l = 0;
    int rec = 0;
    for(i = 0; i < nb_count_files; i++) {
      //fprintf(stderr, "%i => %s\n", i, counts_it[i].kmer);
      if(counts_it[i].kmer && strcmp(counts_it[i].kmer,min_kmer.s) == 0) {
        read = 1;
        counts[i] = counts_it[i].count;
        if(counts[i] >= min_recurrence_abundance) {
          rec++;
        }
        // Load next k-mer
        free(counts_it[i].kmer);
        if(ks_getuntil(counts_it[i].ks, 0, str, &dret) > 0) {
          counts_it[i].kmer = ks_release(str);
          ks_getuntil(counts_it[i].ks, 0, str, &dret);
          counts_it[i].count = atoi(str->s);
        } else {
          counts_it[i].kmer = NULL;
        }
      } else {
        counts[i] = 0;
      }
      // Set the next min k-mer
      if(counts_it[i].kmer && (next_min_kmer.l == 0 || strcmp(counts_it[i].kmer,next_min_kmer.s) < 0)) {
        next_min_kmer.l = 0;
        kputs(counts_it[i].kmer,&next_min_kmer);
      }
    }

    // Print k-mer in reccurence is enough
    if(rec >= min_recurrence) {
      fprintf(stdout, "%s", min_kmer.s);
      for(i = 0; i < nb_count_files; i++) {
        fprintf(stdout, "\t%d", counts[i]);
      }
      fprintf(stdout, "\n");
    }

    // Set the new min_kmer
    min_kmer.l = 0;
    kputs(next_min_kmer.s,&min_kmer);
  }

  free(counts_it);
  free(counts);
  return 0;
}
