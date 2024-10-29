# TODO List

## HIGH

- [x] BUG: Fix `kmer_as_int` function to work properly when shifting right.
      Current known to not work for certain strings. Unknown if moving left works.
      Calculating raw each time works: Bug was because the `_five_to_pow_km1` was
      not initialized when calling `kmerf_load_file`.

- [ ] BUG: Fix problem in `move_mem_finder` where if you give it large string
      first and then a shorter string then it prints duplicate mems from the larger
      string as mems in the shorter string. Eg: If I give pattern of length 100 and
      then pattern of length 10 then it prints mems from pattern of 10 and pattern of
      20 to 100. (`nour`)

- [ ] Integrate MEM finding and substring finding into one program so that we
      don't print the substrings, we just find the mems directly in the
      substring. You can in fact, just call `findMem` once you have a substring.
      (`hamza`)

- [ ] Write a script to automatically search though `k` values and `insparams`
      values and build a tables with the results. Ensure that the mems found
      match a source of truth everytime an experiment is run. Find good ways to
      measure the time taken for each of those. (`anas`)

## MEDIUM

- [ ] print ratios of things when building kmer-bloom-filter. Things like bits
      set to total bits, and total possible kmers (use appox of `4^k` not `5^k`)
      to kmers inserted.

- [ ] Change MEM finding to use BML rather than forward-backward. Right now we
      use forward-backward. Double check results against known MEMs again. Allow
      the user to select whether they want to use BML for forward-backward.
      (`hamza`, `anas`)

- [ ] Allow use to select if they want to use a filter. Make it like a normal
      CLI tool, refer to `man grep` and write a help doc simliar to the 
      `man grep` listing. (`suchi`)

## LOW

- [ ] write code to build move table, adapt `suchi-move-table`
