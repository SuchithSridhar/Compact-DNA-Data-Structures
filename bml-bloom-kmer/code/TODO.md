# TODO List

## HIGH

- [ ] BUG: Fix problem in `move_mem_finder` where if you give it large string
      first and then a shorter string then it prints duplicate mems from the larger
      string as mems in the shorter string. Eg: If I give pattern of length 100 and
      then pattern of length 10 then it prints mems from pattern of 10 and pattern of
      20 to 100.

- [x] BUG: Fix `kmer_as_int` function to work properly when shifting right.
      Current known to not work for certain strings. Unknown if moving left works.
      Calcuating raw each time works: Bug was because the `_five_to_pow_km1` was
      not initilized when calling `kmerf_load_file`.

## MEDIUM

- [ ] write code to build move table, adapt `suchi-move-table`

## LOW
