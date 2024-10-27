#include "kmer_filter.h"
#include "bloom_filter.h"
#include <assert.h>
#include <stdio.h>

bool kmerf_should_contain(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_hash(&kf->insertion_hash, kmer) % kf->insertion_param == 0;
}

bool kmerf_query(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_query(kf->bloom, kmer);
}

int32_t kmerf_add(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_insert(kf->bloom, kmer);
}

kmerf_populate_result_t kmerf_populate(kmer_filter_t *kf, Text T) {
    kmerf_populate_result_t result = {
        .kmers_inserted = 0,
        .bits_set = 0,
    };

    // setting this to NULL initially so that we get full reading
    kf->_cache_source_string = NULL;

    for (size_t i = 0; i < T.len - kf->kmer_size; i++) {
        kmer_int_t kmer = kmerf_as_int(kf, T.T, i);

        if (kmerf_should_contain(kf, kmer) && !kmerf_query(kf, kmer)) {
            result.bits_set += kmerf_add(kf, kmer);
            result.kmers_inserted++;
        }
    }

    return result;
}

// Switch symbol to corresponding integer
// NOTE: Assumption of ACGT
int _ssym(char sym) {
    switch (sym) {
    case 'A':
        return 1;
    case 'C':
        return 2;
    case 'G':
        return 3;
    case 'T':
        return 4;
    }
    return 0;
}

kmer_int_t kmerf_as_int(kmer_filter_t *kf, char *str, size_t start) {
    kmer_int_t kmer = 0;
    size_t diff = (start > kf->_cache_window_start)
                      ? (start - kf->_cache_window_start)
                      : (kf->_cache_window_start - start);

    // NOTE: Assumption of AGCT
    if (kf->_cache_source_string != str || diff > (kf->kmer_size / 2) + 1) {
        // Init fresh k-mer if cache is stale or if diff is too large
        // I've set the min diff to be k/2 + 1 so that if we need to make more
        // than half shifts anyway, we can just calc fresh.
        for (int i = 0; i < kf->kmer_size; i++) {
            kmer *= 5;
            kmer += _ssym(str[start + i]);
        }
    } else if (start < kf->_cache_window_start) {
        // start has moved left
        size_t cs = kf->_cache_window_start;
        kmer = kf->_cache_kmer;
        for (int i = 0; i < diff; i++) {
            // remove the last character
            kmer -= _ssym(str[cs + kf->kmer_size - i - 1]);
            // shift the current characters right
            kmer /= 5;
            // add the new character
            kmer += kf->_five_to_pow_km1 * _ssym(str[cs - i - 1]);
        }
    } else if (start > kf->_cache_window_start) {
        // start has moved right
        size_t cs = kf->_cache_window_start;
        kmer = kf->_cache_kmer;
        for (int i = 0; i < diff; i++) {
            // remove the first character
            kmer -= _ssym(str[cs + i]) * kf->_five_to_pow_km1;
            // shift the current characters left
            kmer *= 5;
            // add the new character
            kmer += _ssym(str[cs + kf->kmer_size + i]);
        }
    } else {
        // start is the same
        kmer = kf->_cache_kmer;
    }

    kf->_cache_source_string = str;
    kf->_cache_kmer = kmer;
    kf->_cache_window_start = start;

    return kmer;
}

int kmerf_load_file(kmer_filter_t *kf, char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return EXIT_FAILURE;
    }

    fread(&kf->kmer_size, sizeof(int), 1, file);
    fread(&kf->filter_size, sizeof(size_t), 1, file);
    fread(&kf->insertion_param, sizeof(int), 1, file);

    // write hash function used for insertion parameter
    fread(&kf->insertion_hash.A, sizeof(size_t), 1, file);
    fread(&kf->insertion_hash.B, sizeof(size_t), 1, file);
    fread(&kf->insertion_hash.P, sizeof(size_t), 1, file);

    // write the hash functions used for the bloom filter
    size_t hash_func_count;
    fread(&hash_func_count, sizeof(size_t), 1, file);
    hash_function_t *hfs = malloc(sizeof(hash_function_t) * hash_func_count);

    for (int i = 0; i < hash_func_count; i++) {
        fread(&hfs[i].A, sizeof(size_t), 1, file);
        fread(&hfs[i].B, sizeof(size_t), 1, file);
        fread(&hfs[i].P, sizeof(size_t), 1, file);
    }

    size_t bytes_allocated;
    fread(&bytes_allocated, sizeof(size_t), 1, file);

    kf->bloom = bf_create(kf->filter_size, hfs, hash_func_count);

    printf("%ld %ld\n", bytes_allocated, kf->bloom->bv->bytes_allocated);
    assert(kf->bloom->bv->bytes_allocated == bytes_allocated);

    fread(kf->bloom->bv->bit_array, sizeof(int8_t), bytes_allocated, file);

    fclose(file);

    return EXIT_SUCCESS;
}

int kmerf_save_file(kmer_filter_t *kf, char *filename) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        return EXIT_FAILURE;
    }

    fwrite(&kf->kmer_size, sizeof(int), 1, file);
    fwrite(&kf->filter_size, sizeof(size_t), 1, file);
    fwrite(&kf->insertion_param, sizeof(int), 1, file);

    // write hash function used for insertion parameter
    fwrite(&kf->insertion_hash.A, sizeof(size_t), 1, file);
    fwrite(&kf->insertion_hash.B, sizeof(size_t), 1, file);
    fwrite(&kf->insertion_hash.P, sizeof(size_t), 1, file);

    // write the hash functions used for the bloom filter
    fwrite(&kf->bloom->hash_functions_count, sizeof(size_t), 1, file);
    for (int i = 0; i < kf->bloom->hash_functions_count; i++) {
        fwrite(&kf->bloom->hash_functions[i].A, sizeof(size_t), 1, file);
        fwrite(&kf->bloom->hash_functions[i].B, sizeof(size_t), 1, file);
        fwrite(&kf->bloom->hash_functions[i].P, sizeof(size_t), 1, file);
    }

    fwrite(&kf->bloom->bv->bytes_allocated, sizeof(size_t), 1, file);
    fwrite(kf->bloom->bv->bit_array, sizeof(int8_t),
           kf->bloom->bv->bytes_allocated, file);

    fclose(file);

    printf("%ld %ld\n", kf->bloom->bv->bytes_allocated, kf->filter_size);
    return EXIT_SUCCESS;
}
