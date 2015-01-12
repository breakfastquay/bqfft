/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */
/* Copyright Chris Cannam - All Rights Reserved */

#ifndef TEST_COMPARES_H
#define TEST_COMPARES_H

// These macros are used for comparing generated results, and they
// aren't always going to be exact. Adding 0.1 to each value gives
// us a little more fuzz in qFuzzyCompare (which ultimately does
// the comparison).

#define COMPARE_ZERO(a) \
    QCOMPARE(a + 0.1, 0.1)

#define COMPARE_ZERO_F(a) \
    QCOMPARE(a + 0.1f, 0.1f)

#define COMPARE_FUZZIER(a, b) \
    QCOMPARE(a + 0.1, b + 0.1)

#define COMPARE_FUZZIER_F(a, b) \
    QCOMPARE(a + 0.1f, b + 0.1f)

#define COMPARE_ALL(a, n) \
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        COMPARE_FUZZIER(a[cmp_i], n); \
    }

#define COMPARE_SCALED(a, b, s)						\
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        COMPARE_FUZZIER(a[cmp_i] / s, b[cmp_i]); \
    }

#define COMPARE_ALL_F(a, n) \
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        COMPARE_FUZZIER_F(a[cmp_i], n); \
    }

#define COMPARE_SCALED_F(a, b, s)						\
    for (int cmp_i = 0; cmp_i < (int)(sizeof(a)/sizeof(a[0])); ++cmp_i) { \
        COMPARE_FUZZIER_F(a[cmp_i] / s, b[cmp_i]); \
    }

#endif
