#include "kmer_filter.h"
#include "bloom_filter.h"

bool kmerf_should_contain(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_hash(&kf->insertion_hash, kmer) % kf->insertion_param == 0;
}

bool kmerf_query(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_query(kf->bloom, kmer);
}

int32_t kmerf_add(kmer_filter_t *kf, kmer_int_t kmer) {
    return bf_insert(kf->bloom, kmer);
}

kmerf_populate_result_t kmerf_populate_filter(Text T, kmer_filter_t *kf) {
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
    size_t diff = 0;

    if (start < kf->_cache_window_start) {
        diff = kf->_cache_window_start - start;
    } else {
        diff = start - kf->_cache_window_start;
    }

    // NOTE: Assumption of AGCT
    if (kf->_cache_source_string != str || diff > (kf->kmer_size / 2) + 1) {
        // I've set the min diff to be k/2 + 1 so that if we need to make more
        // than half shifts anyway, we can just calc fresh.
        for (int i = 0; i < kf->kmer_size; i++) {
            kmer *= 5;
            kmer += _ssym(str[i]);
        }
    } else if (start < kf->_cache_window_start) {
        // start has moved left
        size_t cs = kf->_cache_window_start;
        kmer = kf->_cache_window_start;
        for (int i = 0; i < diff; i++) {
            // remove the last character
            kmer -= _ssym(str[cs + kf->kmer_size - i - 1]);
            // shift the current characters right
            kmer /= 5;
            // add the new character
            kmer += kf->_five_to_pow_k * _ssym(str[cs - i - 1]);
        }
    } else if (start > kf->_cache_window_start) {
        // start has moved right
        size_t cs = kf->_cache_window_start;
        kmer = kf->_cache_window_start;
        for (int i = 0; i < diff; i++) {
            // remove the first character
            kmer -= _ssym(str[cs + i]);
            // shift the current characters left
            kmer *= 5;
            // add the new character
            kmer += kf->_five_to_pow_k * _ssym(str[cs + kf->kmer_size + i]);
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
