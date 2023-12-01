/* Copyright (c) 2018, 2023, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef UCA900_ZH_DATA_H
#define UCA900_ZH_DATA_H

#include <cstdint>

/*
  For collation which changes character's primary weight according to its
  tailoring rule, we give this character a extra collation element (see
  comments in my_char_weight_put_900()). Usually the primary weight of this
  extra CE starts from 0x54A4, which is the biggest primary weight of all
  regular characters (non-CJK and non-ignorable) in DUCET. But Chinese is
  special, because to keep assigning single primary weight to character
  groups like Latin, Cyrillic, etc, we used all weight value in [0x1C47,
  0xF643], so we give the primary weight of extra CE starting from 0xF644
  to avoid weight overlapping.
 */
constexpr int ZH_EXTRA_CE_PRI = 0xF644;

static const char zh_cldr_30[] =
    "&[before 2]a<<\\u0101<<<\\u0100<<\\u00E1<<<\\u00C1<<\\u01CE<<<\\u01CD"
    "<<\\u00E0<<<\\u00C0"
    "&[before 2]e<<\\u0113<<<\\u0112<<\\u00E9<<<\\u00C9<<\\u011B<<<\\u011A"
    "<<\\u00E8<<<\\u00C8"
    "&e<<e\\u0302\\u0304<<<E\\u0302\\u0304<<e\\u0302\\u0301<<<E\\u0302\\u0301"
    "<<e\\u0302\\u030C<<<E\\u0302\\u030C<<e\\u0302\\u0300<<<E\\u0302\\u0300"
    "&[before 2]i<<\\u012B<<<\\u012A<<\\u00ED<<<\\u00CD<<\\u01D0<<<\\u01CF"
    "<<\\u00EC<<<\\u00CC"
    "&[before 2]m<<m\\u0304<<<M\\u0304<<\\u1E3F<<<\\u1E3E<<m\\u030C"
    "<<<M\\u030C<<m\\u0300<<<M\\u0300"
    "&[before 2]n<<n\\u0304<<<N\\u0304<<\\u0144<<<\\u0143<<\\u0148<<<\\u0147"
    "<<\\u01F9<<<\\u01F8"
    "&[before 2]o<<\\u014D<<<\\u014C<<\\u00F3<<<\\u00D3<<\\u01D2<<<\\u01D1"
    "<<\\u00F2<<<\\u00D2"
    "&[before 2]u<<\\u016B<<<\\u016A<<\\u00FA<<<\\u00DA<<\\u01D4<<<\\u01D3"
    "<<\\u00F9<<<\\u00D9"
    "&U<<\\u01D6<<<\\u01D5<<\\u01D8<<<\\u01D7<<\\u01DA<<<\\u01D9<<\\u01DC"
    "<<<\\u01DB<<\\u00FC<<<\\u00DC"
    "&(\\u4E00)<<<\\u3220"
    "&(\\u4E03)<<<\\u3226"
    "&(\\u4E09)<<<\\u3222"
    "&(\\u4E5D)<<<\\u3228"
    "&(\\u4E8C)<<<\\u3221"
    "&(\\u4E94)<<<\\u3224"
    "&(\\u4EE3)<<<\\u3239"
    "&(\\u4F01)<<<\\u323D"
    "&(\\u4F11)<<<\\u3241"
    "&(\\u516B)<<<\\u3227"
    "&(\\u516D)<<<\\u3225"
    "&(\\u52B4)<<<\\u3238"
    "&(\\u5341)<<<\\u3229"
    "&(\\u5354)<<<\\u323F"
    "&(\\u540D)<<<\\u3234"
    "&(\\u547C)<<<\\u323A"
    "&(\\u56DB)<<<\\u3223"
    "&(\\u571F)<<<\\u322F"
    "&(\\u5B66)<<<\\u323B"
    "&(\\u65E5)<<<\\u3230"
    "&(\\u6708)<<<\\u322A"
    "&(\\u6709)<<<\\u3232"
    "&(\\u6728)<<<\\u322D"
    "&(\\u682A)<<<\\u3231"
    "&(\\u6C34)<<<\\u322C"
    "&(\\u706B)<<<\\u322B"
    "&(\\u7279)<<<\\u3235"
    "&(\\u76E3)<<<\\u323C"
    "&(\\u793E)<<<\\u3233"
    "&(\\u795D)<<<\\u3237"
    "&(\\u796D)<<<\\u3240"
    "&(\\u81EA)<<<\\u3242"
    "&(\\u81F3)<<<\\u3243"
    "&(\\u8CA1)<<<\\u3236"
    "&(\\u8CC7)<<<\\u323E"
    "&(\\u91D1)<<<\\u322E"
    "&0\\u70B9<<<\\u3358"
    "&10\\u65E5<<<\\u33E9"
    "&10\\u6708<<<\\u32C9"
    "&10\\u70B9<<<\\u3362"
    "&11\\u65E5<<<\\u33EA"
    "&11\\u6708<<<\\u32CA"
    "&11\\u70B9<<<\\u3363"
    "&12\\u65E5<<<\\u33EB"
    "&12\\u6708<<<\\u32CB"
    "&12\\u70B9<<<\\u3364"
    "&13\\u65E5<<<\\u33EC"
    "&13\\u70B9<<<\\u3365"
    "&14\\u65E5<<<\\u33ED"
    "&14\\u70B9<<<\\u3366"
    "&15\\u65E5<<<\\u33EE"
    "&15\\u70B9<<<\\u3367"
    "&16\\u65E5<<<\\u33EF"
    "&16\\u70B9<<<\\u3368"
    "&17\\u65E5<<<\\u33F0"
    "&17\\u70B9<<<\\u3369"
    "&18\\u65E5<<<\\u33F1"
    "&18\\u70B9<<<\\u336A"
    "&19\\u65E5<<<\\u33F2"
    "&19\\u70B9<<<\\u336B"
    "&1\\u65E5<<<\\u33E0"
    "&1\\u6708<<<\\u32C0"
    "&1\\u70B9<<<\\u3359"
    "&20\\u65E5<<<\\u33F3"
    "&20\\u70B9<<<\\u336C"
    "&21\\u65E5<<<\\u33F4"
    "&21\\u70B9<<<\\u336D"
    "&22\\u65E5<<<\\u33F5"
    "&22\\u70B9<<<\\u336E"
    "&23\\u65E5<<<\\u33F6"
    "&23\\u70B9<<<\\u336F"
    "&24\\u65E5<<<\\u33F7"
    "&24\\u70B9<<<\\u3370"
    "&25\\u65E5<<<\\u33F8"
    "&26\\u65E5<<<\\u33F9"
    "&27\\u65E5<<<\\u33FA"
    "&28\\u65E5<<<\\u33FB"
    "&29\\u65E5<<<\\u33FC"
    "&2\\u65E5<<<\\u33E1"
    "&2\\u6708<<<\\u32C1"
    "&2\\u70B9<<<\\u335A"
    "&30\\u65E5<<<\\u33FD"
    "&31\\u65E5<<<\\u33FE"
    "&3\\u65E5<<<\\u33E2"
    "&3\\u6708<<<\\u32C2"
    "&3\\u70B9<<<\\u335B"
    "&4\\u65E5<<<\\u33E3"
    "&4\\u6708<<<\\u32C3"
    "&4\\u70B9<<<\\u335C"
    "&5\\u65E5<<<\\u33E4"
    "&5\\u6708<<<\\u32C4"
    "&5\\u70B9<<<\\u335D"
    "&6\\u65E5<<<\\u33E5"
    "&6\\u6708<<<\\u32C5"
    "&6\\u70B9<<<\\u335E"
    "&7\\u65E5<<<\\u33E6"
    "&7\\u6708<<<\\u32C6"
    "&7\\u70B9<<<\\u335F"
    "&8\\u65E5<<<\\u33E7"
    "&8\\u6708<<<\\u32C7"
    "&8\\u70B9<<<\\u3360"
    "&9\\u65E5<<<\\u33E8"
    "&9\\u6708<<<\\u32C8"
    "&9\\u70B9<<<\\u3361"
    "&\\u3014\\u4E09\\u3015<<<\\u01F241"
    "&\\u3014\\u4E8C\\u3015<<<\\u01F242"
    "&\\u3014\\u52DD\\u3015<<<\\u01F247"
    "&\\u3014\\u5B89\\u3015<<<\\u01F243"
    "&\\u3014\\u6253\\u3015<<<\\u01F245"
    "&\\u3014\\u6557\\u3015<<<\\u01F248"
    "&\\u3014\\u672C\\u3015<<<\\u01F240"
    "&\\u3014\\u70B9\\u3015<<<\\u01F244"
    "&\\u3014\\u76D7\\u3015<<<\\u01F246"
    "&\\u4E00<<<\\u2F00<<<\\u3192<<<\\u3280<<<\\u01F229"
    "&\\u4E01<<<\\u319C"
    "&\\u4E03<<<\\u3286"
    "&\\u4E09<<<\\u3194<<<\\u3282<<<\\u01F22A"
    "&\\u4E0A<<<\\u3196<<<\\u32A4"
    "&\\u4E0B<<<\\u3198<<<\\u32A6"
    "&\\u4E19<<<\\u319B"
    "&\\u4E28<<<\\u2F01"
    "&\\u4E2D<<<\\u3197<<<\\u32A5<<<\\u01F22D"
    "&\\u4E36<<<\\u2F02"
    "&\\u4E3F<<<\\u2F03"
    "&\\u4E59<<<\\u2F04<<<\\u319A"
    "&\\u4E5D<<<\\u3288"
    "&\\u4E85<<<\\u2F05"
    "&\\u4E8C<<<\\u2F06<<<\\u3193<<<\\u3281<<<\\u01F214"
    "&\\u4E94<<<\\u3284"
    "&\\u4EA0<<<\\u2F07"
    "&\\u4EA4<<<\\u01F218"
    "&\\u4EBA<<<\\u2F08<<<\\u319F"
    "&\\u4F01<<<\\u32AD"
    "&\\u4F11<<<\\u32A1"
    "&\\u512A<<<\\u329D"
    "&\\u513F<<<\\u2F09"
    "&\\u5165<<<\\u2F0A"
    "&\\u516B<<<\\u2F0B<<<\\u3287"
    "&\\u516D<<<\\u3285"
    "&\\u5182<<<\\u2F0C"
    "&\\u518D<<<\\u01F21E"
    "&\\u5196<<<\\u2F0D"
    "&\\u5199<<<\\u32A2"
    "&\\u51AB<<<\\u2F0E"
    "&\\u51E0<<<\\u2F0F"
    "&\\u51F5<<<\\u2F10"
    "&\\u5200<<<\\u2F11"
    "&\\u521D<<<\\u01F220"
    "&\\u524D<<<\\u01F21C"
    "&\\u5272<<<\\u01F239"
    "&\\u529B<<<\\u2F12"
    "&\\u52B4<<<\\u3298"
    "&\\u52F9<<<\\u2F13"
    "&\\u5315<<<\\u2F14"
    "&\\u531A<<<\\u2F15"
    "&\\u5338<<<\\u2F16<<<\\u32A9"
    "&\\u5341<<<\\u2F17<<<\\u3038<<<\\u3289"
    "&\\u5344<<<\\u3039"
    "&\\u5345<<<\\u303A"
    "&\\u5354<<<\\u32AF"
    "&\\u535C<<<\\u2F18"
    "&\\u5369<<<\\u2F19"
    "&\\u5370<<<\\u329E"
    "&\\u5382<<<\\u2F1A"
    "&\\u53B6<<<\\u2F1B"
    "&\\u53C8<<<\\u2F1C"
    "&\\u53CC<<<\\u01F212"
    "&\\u53E3<<<\\u2F1D"
    "&\\u53EF<<<\\u01F251"
    "&\\u53F3<<<\\u32A8<<<\\u01F22E"
    "&\\u5408<<<\\u01F234"
    "&\\u540D<<<\\u3294"
    "&\\u5439<<<\\u01F225"
    "&\\u554F<<<\\u3244"
    "&\\u55B6<<<\\u01F23A"
    "&\\u56D7<<<\\u2F1E"
    "&\\u56DB<<<\\u3195<<<\\u3283"
    "&\\u571F<<<\\u2F1F<<<\\u328F"
    "&\\u5730<<<\\u319E"
    "&\\u58EB<<<\\u2F20"
    "&\\u58F0<<<\\u01F224"
    "&\\u5902<<<\\u2F21"
    "&\\u590A<<<\\u2F22"
    "&\\u5915<<<\\u2F23"
    "&\\u591A<<<\\u01F215"
    "&\\u591C<<<\\u32B0"
    "&\\u5927<<<\\u2F24"
    "&\\u5927\\u6B63<<<\\u337D"
    "&\\u5929<<<\\u319D<<<\\u01F217"
    "&\\u5973<<<\\u2F25<<<\\u329B"
    "&\\u5B50<<<\\u2F26"
    "&\\u5B57<<<\\u01F211"
    "&\\u5B66<<<\\u32AB"
    "&\\u5B80<<<\\u2F27"
    "&\\u5B97<<<\\u32AA"
    "&\\u5BF8<<<\\u2F28"
    "&\\u5C0F<<<\\u2F29"
    "&\\u5C22<<<\\u2F2A"
    "&\\u5C38<<<\\u2F2B"
    "&\\u5C6E<<<\\u2F2C"
    "&\\u5C71<<<\\u2F2D"
    "&\\u5DDB<<<\\u2F2E"
    "&\\u5DE5<<<\\u2F2F"
    "&\\u5DE6<<<\\u32A7<<<\\u01F22C"
    "&\\u5DF1<<<\\u2F30"
    "&\\u5DFE<<<\\u2F31"
    "&\\u5E72<<<\\u2F32"
    "&\\u5E73\\u6210<<<\\u337B"
    "&\\u5E7A<<<\\u2F33"
    "&\\u5E7C<<<\\u3245"
    "&\\u5E7F<<<\\u2F34"
    "&\\u5EF4<<<\\u2F35"
    "&\\u5EFE<<<\\u2F36"
    "&\\u5F0B<<<\\u2F37"
    "&\\u5F13<<<\\u2F38"
    "&\\u5F50<<<\\u2F39"
    "&\\u5F61<<<\\u2F3A"
    "&\\u5F73<<<\\u2F3B"
    "&\\u5F8C<<<\\u01F21D"
    "&\\u5F97<<<\\u01F250"
    "&\\u5FC3<<<\\u2F3C"
    "&\\u6208<<<\\u2F3D"
    "&\\u6236<<<\\u2F3E"
    "&\\u624B<<<\\u2F3F<<<\\u01F210"
    "&\\u6253<<<\\u01F231"
    "&\\u6295<<<\\u01F227"
    "&\\u6307<<<\\u01F22F"
    "&\\u6355<<<\\u01F228"
    "&\\u652F<<<\\u2F40"
    "&\\u6534<<<\\u2F41"
    "&\\u6587<<<\\u2F42<<<\\u3246"
    "&\\u6597<<<\\u2F43"
    "&\\u6599<<<\\u01F21B"
    "&\\u65A4<<<\\u2F44"
    "&\\u65B0<<<\\u01F21F"
    "&\\u65B9<<<\\u2F45"
    "&\\u65E0<<<\\u2F46"
    "&\\u65E5<<<\\u2F47<<<\\u3290"
    "&\\u660E\\u6CBB<<<\\u337E"
    "&\\u6620<<<\\u01F219"
    "&\\u662D\\u548C<<<\\u337C"
    "&\\u66F0<<<\\u2F48"
    "&\\u6708<<<\\u2F49<<<\\u328A<<<\\u01F237"
    "&\\u6709<<<\\u3292<<<\\u01F236"
    "&\\u6728<<<\\u2F4A<<<\\u328D"
    "&\\u682A<<<\\u3291"
    "&\\u682A\\u5F0F\\u4F1A\\u793E<<<\\u337F"
    "&\\u6B20<<<\\u2F4B"
    "&\\u6B62<<<\\u2F4C"
    "&\\u6B63<<<\\u32A3"
    "&\\u6B79<<<\\u2F4D"
    "&\\u6BB3<<<\\u2F4E"
    "&\\u6BCB<<<\\u2F4F"
    "&\\u6BCD<<<\\u2E9F"
    "&\\u6BD4<<<\\u2F50"
    "&\\u6BDB<<<\\u2F51"
    "&\\u6C0F<<<\\u2F52"
    "&\\u6C14<<<\\u2F53"
    "&\\u6C34<<<\\u2F54<<<\\u328C"
    "&\\u6CE8<<<\\u329F"
    "&\\u6E80<<<\\u01F235"
    "&\\u6F14<<<\\u01F226"
    "&\\u706B<<<\\u2F55<<<\\u328B"
    "&\\u7121<<<\\u01F21A"
    "&\\u722A<<<\\u2F56"
    "&\\u7236<<<\\u2F57"
    "&\\u723B<<<\\u2F58"
    "&\\u723F<<<\\u2F59"
    "&\\u7247<<<\\u2F5A"
    "&\\u7259<<<\\u2F5B"
    "&\\u725B<<<\\u2F5C"
    "&\\u7279<<<\\u3295"
    "&\\u72AC<<<\\u2F5D"
    "&\\u7384<<<\\u2F5E"
    "&\\u7389<<<\\u2F5F"
    "&\\u74DC<<<\\u2F60"
    "&\\u74E6<<<\\u2F61"
    "&\\u7518<<<\\u2F62"
    "&\\u751F<<<\\u2F63<<<\\u01F222"
    "&\\u7528<<<\\u2F64"
    "&\\u7530<<<\\u2F65"
    "&\\u7532<<<\\u3199"
    "&\\u7533<<<\\u01F238"
    "&\\u7537<<<\\u329A"
    "&\\u758B<<<\\u2F66"
    "&\\u7592<<<\\u2F67"
    "&\\u7676<<<\\u2F68"
    "&\\u767D<<<\\u2F69"
    "&\\u76AE<<<\\u2F6A"
    "&\\u76BF<<<\\u2F6B"
    "&\\u76E3<<<\\u32AC"
    "&\\u76EE<<<\\u2F6C"
    "&\\u77DB<<<\\u2F6D"
    "&\\u77E2<<<\\u2F6E"
    "&\\u77F3<<<\\u2F6F"
    "&\\u793A<<<\\u2F70"
    "&\\u793E<<<\\u3293"
    "&\\u795D<<<\\u3297"
    "&\\u7981<<<\\u01F232"
    "&\\u79B8<<<\\u2F71"
    "&\\u79BE<<<\\u2F72"
    "&\\u79D8<<<\\u3299"
    "&\\u7A74<<<\\u2F73"
    "&\\u7A7A<<<\\u01F233"
    "&\\u7ACB<<<\\u2F74"
    "&\\u7AF9<<<\\u2F75"
    "&\\u7B8F<<<\\u3247"
    "&\\u7C73<<<\\u2F76"
    "&\\u7CF8<<<\\u2F77"
    "&\\u7D42<<<\\u01F221"
    "&\\u7F36<<<\\u2F78"
    "&\\u7F51<<<\\u2F79"
    "&\\u7F8A<<<\\u2F7A"
    "&\\u7FBD<<<\\u2F7B"
    "&\\u8001<<<\\u2F7C"
    "&\\u800C<<<\\u2F7D"
    "&\\u8012<<<\\u2F7E"
    "&\\u8033<<<\\u2F7F"
    "&\\u807F<<<\\u2F80"
    "&\\u8089<<<\\u2F81"
    "&\\u81E3<<<\\u2F82"
    "&\\u81EA<<<\\u2F83"
    "&\\u81F3<<<\\u2F84"
    "&\\u81FC<<<\\u2F85"
    "&\\u820C<<<\\u2F86"
    "&\\u821B<<<\\u2F87"
    "&\\u821F<<<\\u2F88"
    "&\\u826E<<<\\u2F89"
    "&\\u8272<<<\\u2F8A"
    "&\\u8278<<<\\u2F8B"
    "&\\u864D<<<\\u2F8C"
    "&\\u866B<<<\\u2F8D"
    "&\\u8840<<<\\u2F8E"
    "&\\u884C<<<\\u2F8F"
    "&\\u8863<<<\\u2F90"
    "&\\u897E<<<\\u2F91"
    "&\\u898B<<<\\u2F92"
    "&\\u89D2<<<\\u2F93"
    "&\\u89E3<<<\\u01F216"
    "&\\u8A00<<<\\u2F94"
    "&\\u8C37<<<\\u2F95"
    "&\\u8C46<<<\\u2F96"
    "&\\u8C55<<<\\u2F97"
    "&\\u8C78<<<\\u2F98"
    "&\\u8C9D<<<\\u2F99"
    "&\\u8CA1<<<\\u3296"
    "&\\u8CA9<<<\\u01F223"
    "&\\u8CC7<<<\\u32AE"
    "&\\u8D64<<<\\u2F9A"
    "&\\u8D70<<<\\u2F9B<<<\\u01F230"
    "&\\u8DB3<<<\\u2F9C"
    "&\\u8EAB<<<\\u2F9D"
    "&\\u8ECA<<<\\u2F9E"
    "&\\u8F9B<<<\\u2F9F"
    "&\\u8FB0<<<\\u2FA0"
    "&\\u8FB5<<<\\u2FA1"
    "&\\u904A<<<\\u01F22B"
    "&\\u9069<<<\\u329C"
    "&\\u9091<<<\\u2FA2"
    "&\\u9149<<<\\u2FA3"
    "&\\u914D<<<\\u01F23B"
    "&\\u91C6<<<\\u2FA4"
    "&\\u91CC<<<\\u2FA5"
    "&\\u91D1<<<\\u2FA6<<<\\u328E"
    "&\\u9577<<<\\u2FA7"
    "&\\u9580<<<\\u2FA8"
    "&\\u961C<<<\\u2FA9"
    "&\\u96B6<<<\\u2FAA"
    "&\\u96B9<<<\\u2FAB"
    "&\\u96E8<<<\\u2FAC"
    "&\\u9751<<<\\u2FAD"
    "&\\u975E<<<\\u2FAE"
    "&\\u9762<<<\\u2FAF"
    "&\\u9769<<<\\u2FB0"
    "&\\u97CB<<<\\u2FB1"
    "&\\u97ED<<<\\u2FB2"
    "&\\u97F3<<<\\u2FB3"
    "&\\u9801<<<\\u2FB4"
    "&\\u9805<<<\\u32A0"
    "&\\u98A8<<<\\u2FB5"
    "&\\u98DB<<<\\u2FB6"
    "&\\u98DF<<<\\u2FB7"
    "&\\u9996<<<\\u2FB8"
    "&\\u9999<<<\\u2FB9"
    "&\\u99AC<<<\\u2FBA"
    "&\\u9AA8<<<\\u2FBB"
    "&\\u9AD8<<<\\u2FBC"
    "&\\u9ADF<<<\\u2FBD"
    "&\\u9B25<<<\\u2FBE"
    "&\\u9B2F<<<\\u2FBF"
    "&\\u9B32<<<\\u2FC0"
    "&\\u9B3C<<<\\u2FC1"
    "&\\u9B5A<<<\\u2FC2"
    "&\\u9CE5<<<\\u2FC3"
    "&\\u9E75<<<\\u2FC4"
    "&\\u9E7F<<<\\u2FC5"
    "&\\u9EA5<<<\\u2FC6"
    "&\\u9EBB<<<\\u2FC7"
    "&\\u9EC3<<<\\u2FC8"
    "&\\u9ECD<<<\\u2FC9"
    "&\\u9ED1<<<\\u2FCA"
    "&\\u9EF9<<<\\u2FCB"
    "&\\u9EFD<<<\\u2FCC"
    "&\\u9F0E<<<\\u2FCD"
    "&\\u9F13<<<\\u2FCE"
    "&\\u9F20<<<\\u2FCF"
    "&\\u9F3B<<<\\u2FD0"
    "&\\u9F4A<<<\\u2FD1"
    "&\\u9F52<<<\\u2FD2"
    "&\\u9F8D<<<\\u2FD3"
    "&\\u9F9C<<<\\u2FD4"
    "&\\u9F9F<<<\\u2EF3"
    "&\\u9FA0<<<\\u2FD5"
    "&\\u02342F<\\u91CD\\u5E86/\\u5E86"
    "&\\u5F1E<\\u6C88\\u9633/\\u9633"
    "&\\u92BA<\\u85CF\\u6587/\\u6587";

/*
  Below variables are defined in separate .cc file, generated by uca9dump at
  build-time for the Chinese collation.
 */
extern uint16_t *zh_han_pages[];
extern const int MIN_ZH_HAN_PAGE;
extern const int MAX_ZH_HAN_PAGE;
extern int zh_han_to_single_weight[];
extern const int ZH_HAN_WEIGHT_PAIRS;
#endif
