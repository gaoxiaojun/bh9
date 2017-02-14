#ifndef CURRENCY_H
#define CURRENCY_H

#include <cstdint>
#include <string>

namespace h9 {

#define CURRENCY_DEF(_) \
  _(NONE, 0)            \
  _(AED, 784)           \
  _(AFN, 971)           \
  _(ALL, 8)             \
  _(AMD, 51)            \
  _(ANG, 532)           \
  _(AOA, 973)           \
  _(ARS, 32)            \
  _(AUD, 36)            \
  _(AWG, 533)           \
  _(AZN, 944)           \
  _(BAM, 977)           \
  _(BBD, 52)            \
  _(BDT, 50)            \
  _(BGN, 975)           \
  _(BHD, 48)            \
  _(BIF, 108)           \
  _(BMD, 60)            \
  _(BND, 96)            \
  _(BOB, 68)            \
  _(BOV, 984)           \
  _(BRL, 986)           \
  _(BSD, 44)            \
  _(BTN, 64)            \
  _(BWP, 72)            \
  _(BYR, 974)           \
  _(BZD, 84)            \
  _(CAD, 124)           \
  _(CDF, 976)           \
  _(CHE, 947)           \
  _(CHF, 756)           \
  _(CHW, 948)           \
  _(CLF, 990)           \
  _(CLP, 152)           \
  _(CNH, 156)           \
  _(CNY, 156)           \
  _(COP, 170)           \
  _(COU, 970)           \
  _(CRC, 188)           \
  _(CUC, 931)           \
  _(CUP, 192)           \
  _(CVE, 132)           \
  _(CZK, 203)           \
  _(DJF, 262)           \
  _(DKK, 208)           \
  _(DOP, 214)           \
  _(DZD, 12)            \
  _(EGP, 818)           \
  _(ERN, 232)           \
  _(ETB, 230)           \
  _(EUR, 978)           \
  _(FJD, 242)           \
  _(FKP, 238)           \
  _(GBP, 826)           \
  _(GEL, 981)           \
  _(GHS, 936)           \
  _(GIP, 292)           \
  _(GMD, 270)           \
  _(GNF, 324)           \
  _(GTQ, 320)           \
  _(GYD, 328)           \
  _(HKD, 344)           \
  _(HNL, 340)           \
  _(HRK, 191)           \
  _(HTG, 332)           \
  _(HUF, 348)           \
  _(IDR, 360)           \
  _(ILS, 376)           \
  _(INR, 356)           \
  _(IQD, 368)           \
  _(IRR, 364)           \
  _(ISK, 352)           \
  _(JMD, 388)           \
  _(JOD, 400)           \
  _(JPY, 392)           \
  _(KES, 404)           \
  _(KGS, 417)           \
  _(KHR, 116)           \
  _(KMF, 174)           \
  _(KPW, 408)           \
  _(KRW, 410)           \
  _(KWD, 414)           \
  _(KYD, 136)           \
  _(KZT, 398)           \
  _(LAK, 418)           \
  _(LBP, 422)           \
  _(LKR, 144)           \
  _(LRD, 430)           \
  _(LSL, 426)           \
  _(LTL, 440)           \
  _(LYD, 434)           \
  _(MAD, 504)           \
  _(MDL, 498)           \
  _(MGA, 969)           \
  _(MKD, 807)           \
  _(MMK, 104)           \
  _(MNT, 496)           \
  _(MOP, 446)           \
  _(MRO, 478)           \
  _(MUR, 480)           \
  _(MVR, 462)           \
  _(MWK, 454)           \
  _(MXN, 484)           \
  _(MXV, 979)           \
  _(MYR, 458)           \
  _(MZN, 943)           \
  _(NAD, 516)           \
  _(NGN, 566)           \
  _(NIO, 558)           \
  _(NOK, 578)           \
  _(NPR, 524)           \
  _(NZD, 554)           \
  _(OMR, 512)           \
  _(PAB, 590)           \
  _(PEN, 604)           \
  _(PGK, 598)           \
  _(PHP, 608)           \
  _(PKR, 586)           \
  _(PLN, 985)           \
  _(PYG, 600)           \
  _(QAR, 634)           \
  _(RON, 946)           \
  _(RSD, 941)           \
  _(RUB, 643)           \
  _(RWF, 646)           \
  _(SAR, 682)           \
  _(SBD, 90)            \
  _(SCR, 690)           \
  _(SDG, 938)           \
  _(SEK, 752)           \
  _(SGD, 702)           \
  _(SHP, 654)           \
  _(SLL, 694)           \
  _(SOS, 706)           \
  _(SRD, 968)           \
  _(SSP, 728)           \
  _(STD, 678)           \
  _(SYP, 760)           \
  _(SZL, 748)           \
  _(THB, 764)           \
  _(TJS, 972)           \
  _(TMT, 934)           \
  _(TND, 788)           \
  _(TOP, 776)           \
  _(TRY, 949)           \
  _(TTD, 780)           \
  _(TWD, 901)           \
  _(TZS, 834)           \
  _(UAH, 980)           \
  _(UGX, 800)           \
  _(USD, 840)           \
  _(USN, 997)           \
  _(USS, 998)           \
  _(UYI, 940)           \
  _(UYU, 858)           \
  _(UZS, 860)           \
  _(VEF, 937)           \
  _(VND, 704)           \
  _(VUV, 548)           \
  _(WST, 882)           \
  _(XAF, 950)           \
  _(XAG, 961)           \
  _(XAU, 959)           \
  _(XBA, 955)           \
  _(XBB, 956)           \
  _(XBC, 957)           \
  _(XBD, 958)           \
  _(XCD, 951)           \
  _(XDR, 960)           \
  _(XOF, 952)           \
  _(XPD, 964)           \
  _(XPF, 953)           \
  _(XPT, 962)           \
  _(XSU, 994)           \
  _(XTS, 963)           \
  _(XUA, 965)           \
  _(XXX, 999)           \
  _(YER, 886)           \
  _(ZAR, 710)           \
  _(ZMW, 967)           \
  _(ZWD, 932)

enum class Currency : std::uint16_t {
    #define H9_CURRENCY_DEFINE(name, index) name = index,
        CURRENCY_DEF(H9_CURRENCY_DEFINE)
    #undef H9_CURRENCY_DEFINE
};

static Currency currency_from_string(const std::string &name);
static const std::string &currency_to_string(Currency currency);

} // namespace h9

#endif // CURRENCY_H
