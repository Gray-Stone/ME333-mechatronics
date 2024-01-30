#ifndef INVEST_TYPE_H__
#define INVEST_TYPE_H__

#define MAX_YEARS 100 // constant indicating max number of years to track

typedef struct {
  double inv0;                    // initial investment
  double growth;                  // growth rate, where 1.0 = zero growth
  int years;                      // number of years to track
  double invarray[MAX_YEARS + 1]; // investment array   ==SecA.4.9==
} Investment;                     // the new data type is called Investment

#endif