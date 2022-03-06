# pattern-scanning

## usage:
gcc scan.c -o scan
./scan filename "00 00 00 ?? ..."

## example:
gcc hookme.c -o hookme
./scan hookme "55 48 89 e5 89 7d fc 89"
