# DE-kupl joinCounts

DE-kupl joinCounts is part of the DE-kupl package, and join multiple k-mer abundance files into a single matrix.

joinCounts input files must be tabulated files (possibly gziped) with the k-mer in the first column and the abundance in the second column. The input files must be sorted lexicographically by k-mer, and the files are to expected to containe a header line.

## Installation

1. Clone this repository : `git clone https://github.com/Transipedia/dekupl-joinCounts.git`
2. Compile joinCounts : `make`
3. Place the joinCounts binary somewhere acessible in your `$PATH`

## Example

**Counting and sorting 32-mer with Jellyfish**

```
jellyfish count -m 32 -s 10000 -o sample.jf <(zcat sample.fastq.gz)
jellyfish dump  -c sample.jf | sort -k 1 > counts.tsv
```

Consider using the sort command with `-S {resources.ram}G` and `--parallel {threads}` parameters to speed-up the sorting for large k-mer libraries.

**Join counts from multiple libraries with joinCounts**

```
joinCounts counts1.tsv counts2.tsv > counts-matrix.tsv
```
