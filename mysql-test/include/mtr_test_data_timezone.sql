-- Copyright (c) 2014, 2022, Oracle and/or its affiliates.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License, version 2.0,
-- as published by the Free Software Foundation.
--
-- This program is also distributed with certain software (including
-- but not limited to OpenSSL) that is licensed under separate terms,
-- as designated in a particular file or component or in included license
-- documentation.  The authors of MySQL hereby grant you an additional
-- permission to link the program and your derivative works with the
-- separately licensed software that they have included with MySQL.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License, version 2.0, for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

INSERT INTO time_zone_name (Name, Time_Zone_id) VALUES   ('MET', 1), ('UTC', 2), ('Universal', 2),    ('Europe/Moscow',3), ('leap/Europe/Moscow',4),    ('Japan', 5), ('CET', 6), ('US/Pacific', 7);
INSERT INTO time_zone (Time_zone_id, Use_leap_seconds)   VALUES (1,'N'), (2,'N'), (3,'N'), (4,'Y'), (5,'N'), (6, 'N'), (7, 'N');
INSERT INTO time_zone_transition   (Time_zone_id, Transition_time, Transition_type_id) VALUES   (1, -1693706400, 0) ,(1, -1680483600, 1)  ,(1, -1663455600, 2) ,(1, -1650150000, 3)  ,(1, -1632006000, 2) ,(1, -1618700400, 3)  ,(1, -938905200, 2) ,(1, -857257200, 3)  ,(1, -844556400, 2) ,(1, -828226800, 3)  ,(1, -812502000, 2) ,(1, -796777200, 3)  ,(1, 228877200, 2) ,(1, 243997200, 3)  ,(1, 260326800, 2) ,(1, 276051600, 3)  ,(1, 291776400, 2) ,(1, 307501200, 3)  ,(1, 323830800, 2) ,(1, 338950800, 3)  ,(1, 354675600, 2) ,(1, 370400400, 3)  ,(1, 386125200, 2) ,(1, 401850000, 3)  ,(1, 417574800, 2) ,(1, 433299600, 3)  ,(1, 449024400, 2) ,(1, 465354000, 3)  ,(1, 481078800, 2) ,(1, 496803600, 3)  ,(1, 512528400, 2) ,(1, 528253200, 3)  ,(1, 543978000, 2) ,(1, 559702800, 3)  ,(1, 575427600, 2) ,(1, 591152400, 3)  ,(1, 606877200, 2) ,(1, 622602000, 3)  ,(1, 638326800, 2) ,(1, 654656400, 3)  ,(1, 670381200, 2) ,(1, 686106000, 3)  ,(1, 701830800, 2) ,(1, 717555600, 3)  ,(1, 733280400, 2) ,(1, 749005200, 3)  ,(1, 764730000, 2) ,(1, 780454800, 3)  ,(1, 796179600, 2) ,(1, 811904400, 3)  ,(1, 828234000, 2) ,(1, 846378000, 3)  ,(1, 859683600, 2) ,(1, 877827600, 3)  ,(1, 891133200, 2) ,(1, 909277200, 3)  ,(1, 922582800, 2) ,(1, 941331600, 3)  ,(1, 954032400, 2) ,(1, 972781200, 3)  ,(1, 985482000, 2) ,(1, 1004230800, 3)  ,(1, 1017536400, 2) ,(1, 1035680400, 3)  ,(1, 1048986000, 2) ,(1, 1067130000, 3)  ,(1, 1080435600, 2) ,(1, 1099184400, 3)  ,(1, 1111885200, 2) ,(1, 1130634000, 3)  ,(1, 1143334800, 2) ,(1, 1162083600, 3)  ,(1, 1174784400, 2) ,(1, 1193533200, 3)  ,(1, 1206838800, 2) ,(1, 1224982800, 3)  ,(1, 1238288400, 2) ,(1, 1256432400, 3)  ,(1, 1269738000, 2) ,(1, 1288486800, 3)  ,(1, 1301187600, 2) ,(1, 1319936400, 3)  ,(1, 1332637200, 2) ,(1, 1351386000, 3)  ,(1, 1364691600, 2) ,(1, 1382835600, 3)  ,(1, 1396141200, 2) ,(1, 1414285200, 3)  ,(1, 1427590800, 2) ,(1, 1445734800, 3)  ,(1, 1459040400, 2) ,(1, 1477789200, 3)  ,(1, 1490490000, 2) ,(1, 1509238800, 3)  ,(1, 1521939600, 2) ,(1, 1540688400, 3)  ,(1, 1553994000, 2) ,(1, 1572138000, 3)  ,(1, 1585443600, 2) ,(1, 1603587600, 3)  ,(1, 1616893200, 2) ,(1, 1635642000, 3)  ,(1, 1648342800, 2) ,(1, 1667091600, 3)  ,(1, 1679792400, 2) ,(1, 1698541200, 3)  ,(1, 1711846800, 2) ,(1, 1729990800, 3)  ,(1, 1743296400, 2) ,(1, 1761440400, 3)  ,(1, 1774746000, 2) ,(1, 1792890000, 3)  ,(1, 1806195600, 2) ,(1, 1824944400, 3)  ,(1, 1837645200, 2) ,(1, 1856394000, 3)  ,(1, 1869094800, 2) ,(1, 1887843600, 3)  ,(1, 1901149200, 2) ,(1, 1919293200, 3)  ,(1, 1932598800, 2) ,(1, 1950742800, 3)  ,(1, 1964048400, 2) ,(1, 1982797200, 3)  ,(1, 1995498000, 2) ,(1, 2014246800, 3)  ,(1, 2026947600, 2) ,(1, 2045696400, 3)  ,(1, 2058397200, 2) ,(1, 2077146000, 3)  ,(1, 2090451600, 2) ,(1, 2108595600, 3)  ,(1, 2121901200, 2) ,(1, 2140045200, 3)  ,(3, -1688265000, 2) ,(3, -1656819048, 1)  ,(3, -1641353448, 2) ,(3, -1627965048, 3)  ,(3, -1618716648, 1) ,(3, -1596429048, 3)  ,(3, -1593829848, 5) ,(3, -1589860800, 4)  ,(3, -1542427200, 5) ,(3, -1539493200, 6)  ,(3, -1525323600, 5) ,(3, -1522728000, 4)  ,(3, -1491188400, 7) ,(3, -1247536800, 4)  ,(3, 354920400, 5) ,(3, 370728000, 4)  ,(3, 386456400, 5) ,(3, 402264000, 4)  ,(3, 417992400, 5) ,(3, 433800000, 4)  ,(3, 449614800, 5) ,(3, 465346800, 8)  ,(3, 481071600, 9) ,(3, 496796400, 8)  ,(3, 512521200, 9) ,(3, 528246000, 8)  ,(3, 543970800, 9) ,(3, 559695600, 8)  ,(3, 575420400, 9) ,(3, 591145200, 8)  ,(3, 606870000, 9) ,(3, 622594800, 8)  ,(3, 638319600, 9) ,(3, 654649200, 8)  ,(3, 670374000, 10) ,(3, 686102400, 11)  ,(3, 695779200, 8) ,(3, 701812800, 5)  ,(3, 717534000, 4) ,(3, 733273200, 9)  ,(3, 748998000, 8) ,(3, 764722800, 9)  ,(3, 780447600, 8) ,(3, 796172400, 9)  ,(3, 811897200, 8) ,(3, 828226800, 9)  ,(3, 846370800, 8) ,(3, 859676400, 9)  ,(3, 877820400, 8) ,(3, 891126000, 9)  ,(3, 909270000, 8) ,(3, 922575600, 9)  ,(3, 941324400, 8) ,(3, 954025200, 9)  ,(3, 972774000, 8) ,(3, 985474800, 9)  ,(3, 1004223600, 8) ,(3, 1017529200, 9)  ,(3, 1035673200, 8) ,(3, 1048978800, 9)  ,(3, 1067122800, 8) ,(3, 1080428400, 9)  ,(3, 1099177200, 8) ,(3, 1111878000, 9)  ,(3, 1130626800, 8) ,(3, 1143327600, 9)  ,(3, 1162076400, 8) ,(3, 1174777200, 9)  ,(3, 1193526000, 8) ,(3, 1206831600, 9)  ,(3, 1224975600, 8) ,(3, 1238281200, 9)  ,(3, 1256425200, 8) ,(3, 1269730800, 9)  ,(3, 1288479600, 8) ,(3, 1301180400, 9)  ,(3, 1319929200, 8) ,(3, 1332630000, 9)  ,(3, 1351378800, 8) ,(3, 1364684400, 9)  ,(3, 1382828400, 8) ,(3, 1396134000, 9)  ,(3, 1414278000, 8) ,(3, 1427583600, 9)  ,(3, 1445727600, 8) ,(3, 1459033200, 9)  ,(3, 1477782000, 8) ,(3, 1490482800, 9)  ,(3, 1509231600, 8) ,(3, 1521932400, 9)  ,(3, 1540681200, 8) ,(3, 1553986800, 9)  ,(3, 1572130800, 8) ,(3, 1585436400, 9)  ,(3, 1603580400, 8) ,(3, 1616886000, 9)  ,(3, 1635634800, 8) ,(3, 1648335600, 9)  ,(3, 1667084400, 8) ,(3, 1679785200, 9)  ,(3, 1698534000, 8) ,(3, 1711839600, 9)  ,(3, 1729983600, 8) ,(3, 1743289200, 9)  ,(3, 1761433200, 8) ,(3, 1774738800, 9)  ,(3, 1792882800, 8) ,(3, 1806188400, 9)  ,(3, 1824937200, 8) ,(3, 1837638000, 9)  ,(3, 1856386800, 8) ,(3, 1869087600, 9)  ,(3, 1887836400, 8) ,(3, 1901142000, 9)  ,(3, 1919286000, 8) ,(3, 1932591600, 9)  ,(3, 1950735600, 8) ,(3, 1964041200, 9)  ,(3, 1982790000, 8) ,(3, 1995490800, 9)  ,(3, 2014239600, 8) ,(3, 2026940400, 9)  ,(3, 2045689200, 8) ,(3, 2058390000, 9)  ,(3, 2077138800, 8) ,(3, 2090444400, 9)  ,(3, 2108588400, 8) ,(3, 2121894000, 9)  ,(3, 2140038000, 8)  ,(4, -1688265000, 2) ,(4, -1656819048, 1)  ,(4, -1641353448, 2) ,(4, -1627965048, 3)  ,(4, -1618716648, 1) ,(4, -1596429048, 3)  ,(4, -1593829848, 5) ,(4, -1589860800, 4)  ,(4, -1542427200, 5) ,(4, -1539493200, 6)  ,(4, -1525323600, 5) ,(4, -1522728000, 4)  ,(4, -1491188400, 7) ,(4, -1247536800, 4)  ,(4, 354920409, 5) ,(4, 370728010, 4)  ,(4, 386456410, 5) ,(4, 402264011, 4)  ,(4, 417992411, 5) ,(4, 433800012, 4)  ,(4, 449614812, 5) ,(4, 465346812, 8)  ,(4, 481071612, 9) ,(4, 496796413, 8)  ,(4, 512521213, 9) ,(4, 528246013, 8)  ,(4, 543970813, 9) ,(4, 559695613, 8)  ,(4, 575420414, 9) ,(4, 591145214, 8)  ,(4, 606870014, 9) ,(4, 622594814, 8)  ,(4, 638319615, 9) ,(4, 654649215, 8)  ,(4, 670374016, 10) ,(4, 686102416, 11)  ,(4, 695779216, 8) ,(4, 701812816, 5)  ,(4, 717534017, 4) ,(4, 733273217, 9)  ,(4, 748998018, 8) ,(4, 764722818, 9)  ,(4, 780447619, 8) ,(4, 796172419, 9)  ,(4, 811897219, 8) ,(4, 828226820, 9)  ,(4, 846370820, 8) ,(4, 859676420, 9)  ,(4, 877820421, 8) ,(4, 891126021, 9)  ,(4, 909270021, 8) ,(4, 922575622, 9)  ,(4, 941324422, 8) ,(4, 954025222, 9)  ,(4, 972774022, 8) ,(4, 985474822, 9)  ,(4, 1004223622, 8) ,(4, 1017529222, 9)  ,(4, 1035673222, 8) ,(4, 1048978822, 9)  ,(4, 1067122822, 8) ,(4, 1080428422, 9)  ,(4, 1099177222, 8) ,(4, 1111878022, 9)  ,(4, 1130626822, 8) ,(4, 1143327622, 9)  ,(4, 1162076422, 8) ,(4, 1174777222, 9)  ,(4, 1193526022, 8) ,(4, 1206831622, 9)  ,(4, 1224975622, 8) ,(4, 1238281222, 9)  ,(4, 1256425222, 8) ,(4, 1269730822, 9)  ,(4, 1288479622, 8) ,(4, 1301180422, 9)  ,(4, 1319929222, 8) ,(4, 1332630022, 9)  ,(4, 1351378822, 8) ,(4, 1364684422, 9)  ,(4, 1382828422, 8) ,(4, 1396134022, 9)  ,(4, 1414278022, 8) ,(4, 1427583622, 9)  ,(4, 1445727622, 8) ,(4, 1459033222, 9)  ,(4, 1477782022, 8) ,(4, 1490482822, 9)  ,(4, 1509231622, 8) ,(4, 1521932422, 9)  ,(4, 1540681222, 8) ,(4, 1553986822, 9)  ,(4, 1572130822, 8) ,(4, 1585436422, 9)  ,(4, 1603580422, 8) ,(4, 1616886022, 9)  ,(4, 1635634822, 8) ,(4, 1648335622, 9)  ,(4, 1667084422, 8) ,(4, 1679785222, 9)  ,(4, 1698534022, 8) ,(4, 1711839622, 9)  ,(4, 1729983622, 8) ,(4, 1743289222, 9)  ,(4, 1761433222, 8) ,(4, 1774738822, 9)  ,(4, 1792882822, 8) ,(4, 1806188422, 9)  ,(4, 1824937222, 8) ,(4, 1837638022, 9)  ,(4, 1856386822, 8) ,(4, 1869087622, 9)  ,(4, 1887836422, 8) ,(4, 1901142022, 9)  ,(4, 1919286022, 8) ,(4, 1932591622, 9)  ,(4, 1950735622, 8) ,(4, 1964041222, 9)  ,(4, 1982790022, 8) ,(4, 1995490822, 9)  ,(4, 2014239622, 8) ,(4, 2026940422, 9)  ,(4, 2045689222, 8) ,(4, 2058390022, 9)  ,(4, 2077138822, 8) ,(4, 2090444422, 9)  ,(4, 2108588422, 8) ,(4, 2121894022, 9)  ,(4, 2140038022, 8)  ,(5, -1009875600, 1), (6, -1693706400, 1),(6, -1680483600, 0),(6, -1663455600, 2),(6, -1650150000, 3),(6, -1632006000, 2),(6, -1618700400, 3),(6, -938905200, 2),(6, -857257200, 3),(6, -844556400, 2),(6, -828226800, 3),(6, -812502000, 2),(6, -796777200, 3),(6, -781052400, 2),(6, -766623600, 3),(6, 228877200, 2),(6, 243997200, 3),(6, 260326800, 2),(6, 276051600, 3),(6, 291776400, 2),(6, 307501200, 3),(6, 323830800, 2),(6, 338950800, 3),(6, 354675600, 2),(6, 370400400, 3),(6, 386125200, 2),(6, 401850000, 3),(6, 417574800, 2),(6, 433299600, 3),(6, 449024400, 2),(6, 465354000, 3),(6, 481078800, 2),(6, 496803600, 3),(6, 512528400, 2),(6, 528253200, 3),(6, 543978000, 2),(6, 559702800, 3),(6, 575427600, 2),(6, 591152400, 3),(6, 606877200, 2),(6, 622602000, 3),(6, 638326800, 2),(6, 654656400, 3),(6, 670381200, 2),(6, 686106000, 3),(6, 701830800, 2),(6, 717555600, 3),(6, 733280400, 2),(6, 749005200, 3),(6, 764730000, 2),(6, 780454800, 3),(6, 796179600, 2),(6, 811904400, 3),(6, 828234000, 2),(6, 846378000, 3),(6, 859683600, 2),(6, 877827600, 3),(6, 891133200, 2),(6, 909277200, 3),(6, 922582800, 2),(6, 941331600, 3),(6, 954032400, 2),(6, 972781200, 3),(6, 985482000, 2),(6, 1004230800, 3),(6, 1017536400, 2),(6, 1035680400, 3),(6, 1048986000, 2),(6, 1067130000, 3),(6, 1080435600, 2),(6, 1099184400, 3),(6, 1111885200, 2),(6, 1130634000, 3),(6, 1143334800, 2),(6, 1162083600, 3),(6, 1174784400, 2),(6, 1193533200, 3),(6, 1206838800, 2),(6, 1224982800, 3),(6, 1238288400, 2),(6, 1256432400, 3),(6, 1269738000, 2),(6, 1288486800, 3),(6, 1301187600, 2),(6, 1319936400, 3),(6, 1332637200, 2),(6, 1351386000, 3),(6, 1364691600, 2),(6, 1382835600, 3),(6, 1396141200, 2),(6, 1414285200, 3),(6, 1427590800, 2),(6, 1445734800, 3),(6, 1459040400, 2),(6, 1477789200, 3),(6, 1490490000, 2),(6, 1509238800, 3),(6, 1521939600, 2),(6, 1540688400, 3),(6, 1553994000, 2),(6, 1572138000, 3),(6, 1585443600, 2),(6, 1603587600, 3),(6, 1616893200, 2),(6, 1635642000, 3),(6, 1648342800, 2),(6, 1667091600, 3),(6, 1679792400, 2),(6, 1698541200, 3),(6, 1711846800, 2),(6, 1729990800, 3),(6, 1743296400, 2),(6, 1761440400, 3),(6, 1774746000, 2),(6, 1792890000, 3),(6, 1806195600, 2),(6, 1824944400, 3),(6, 1837645200, 2),(6, 1856394000, 3),(6, 1869094800, 2),(6, 1887843600, 3),(6, 1901149200, 2),(6, 1919293200, 3),(6, 1932598800, 2),(6, 1950742800, 3),(6, 1964048400, 2),(6, 1982797200, 3),(6, 1995498000, 2),(6, 2014246800, 3),(6, 2026947600, 2),(6, 2045696400, 3),(6, 2058397200, 2),(6, 2077146000, 3),(6, 2090451600, 2),(6, 2108595600, 3),(6, 2121901200, 2),(6, 2140045200, 3) ,(7, -1633269600, 0) ,(7, -1615129200, 1) ,(7, -1601820000, 0) ,(7, -1583679600, 1) ,(7, -880207200, 2) ,(7, -769395600, 3) ,(7, -765385200, 1) ,(7, -687967140, 0) ,(7, -662655600, 1) ,(7, -620838000, 0) ,(7, -608137200, 1) ,(7, -589388400, 0) ,(7, -576082800, 1) ,(7, -557938800, 0) ,(7, -544633200, 1) ,(7, -526489200, 0) ,(7, -513183600, 1) ,(7, -495039600, 0) ,(7, -481734000, 1) ,(7, -463590000, 0) ,(7, -450284400, 1) ,(7, -431535600, 0) ,(7, -418230000, 1) ,(7, -400086000, 0) ,(7, -386780400, 1) ,(7, -368636400, 0) ,(7, -355330800, 1) ,(7, -337186800, 0) ,(7, -323881200, 1) ,(7, -305737200, 0) ,(7, -292431600, 1) ,(7, -273682800, 0) ,(7, -260982000, 1) ,(7, -242233200, 0) ,(7, -226508400, 1) ,(7, -210783600, 0) ,(7, -195058800, 1) ,(7, -179334000, 0) ,(7, -163609200, 1) ,(7, -147884400, 0) ,(7, -131554800, 1) ,(7, -116434800, 0) ,(7, -100105200, 1) ,(7, -84376800, 0) ,(7, -68655600, 1) ,(7, -52927200, 0) ,(7, -37206000, 1) ,(7, -21477600, 0) ,(7, -5756400, 1) ,(7, 9972000, 0) ,(7, 25693200, 1) ,(7, 41421600, 0) ,(7, 57747600, 1) ,(7, 73476000, 0) ,(7, 89197200, 1) ,(7, 104925600, 0) ,(7, 120646800, 1) ,(7, 126698400, 0) ,(7, 152096400, 1) ,(7, 162381600, 0) ,(7, 183546000, 1) ,(7, 199274400, 0) ,(7, 215600400, 1) ,(7, 230724000, 0) ,(7, 247050000, 1) ,(7, 262778400, 0) ,(7, 278499600, 1) ,(7, 294228000, 0) ,(7, 309949200, 1) ,(7, 325677600, 0) ,(7, 341398800, 1) ,(7, 357127200, 0) ,(7, 372848400, 1) ,(7, 388576800, 0) ,(7, 404902800, 1) ,(7, 420026400, 0) ,(7, 436352400, 1) ,(7, 452080800, 0) ,(7, 467802000, 1) ,(7, 483530400, 0) ,(7, 499251600, 1) ,(7, 514980000, 0) ,(7, 530701200, 1) ,(7, 544615200, 0) ,(7, 562150800, 1) ,(7, 576064800, 0) ,(7, 594205200, 1) ,(7, 607514400, 0) ,(7, 625654800, 1) ,(7, 638964000, 0) ,(7, 657104400, 1) ,(7, 671018400, 0) ,(7, 688554000, 1) ,(7, 702468000, 0) ,(7, 720003600, 1) ,(7, 733917600, 0) ,(7, 752058000, 1) ,(7, 765367200, 0) ,(7, 783507600, 1) ,(7, 796816800, 0) ,(7, 814957200, 1) ,(7, 828871200, 0) ,(7, 846406800, 1) ,(7, 860320800, 0) ,(7, 877856400, 1) ,(7, 891770400, 0) ,(7, 909306000, 1) ,(7, 923220000, 0) ,(7, 941360400, 1) ,(7, 954669600, 0) ,(7, 972810000, 1) ,(7, 986119200, 0) ,(7, 1004259600, 1) ,(7, 1018173600, 0) ,(7, 1035709200, 1) ,(7, 1049623200, 0) ,(7, 1067158800, 1) ,(7, 1081072800, 0) ,(7, 1099213200, 1) ,(7, 1112522400, 0) ,(7, 1130662800, 1) ,(7, 1143972000, 0) ,(7, 1162112400, 1) ,(7, 1173607200, 0) ,(7, 1194166800, 1) ,(7, 1205056800, 0) ,(7, 1225616400, 1) ,(7, 1236506400, 0) ,(7, 1257066000, 1) ,(7, 1268560800, 0) ,(7, 1289120400, 1) ,(7, 1300010400, 0) ,(7, 1320570000, 1) ,(7, 1331460000, 0) ,(7, 1352019600, 1) ,(7, 1362909600, 0) ,(7, 1383469200, 1) ,(7, 1394359200, 0) ,(7, 1414918800, 1) ,(7, 1425808800, 0) ,(7, 1446368400, 1) ,(7, 1457863200, 0) ,(7, 1478422800, 1) ,(7, 1489312800, 0) ,(7, 1509872400, 1) ,(7, 1520762400, 0) ,(7, 1541322000, 1) ,(7, 1552212000, 0) ,(7, 1572771600, 1) ,(7, 1583661600, 0) ,(7, 1604221200, 1) ,(7, 1615716000, 0) ,(7, 1636275600, 1) ,(7, 1647165600, 0) ,(7, 1667725200, 1) ,(7, 1678615200, 0) ,(7, 1699174800, 1) ,(7, 1710064800, 0) ,(7, 1730624400, 1) ,(7, 1741514400, 0) ,(7, 1762074000, 1) ,(7, 1772964000, 0) ,(7, 1793523600, 1) ,(7, 1805018400, 0) ,(7, 1825578000, 1) ,(7, 1836468000, 0) ,(7, 1857027600, 1) ,(7, 1867917600, 0) ,(7, 1888477200, 1) ,(7, 1899367200, 0) ,(7, 1919926800, 1) ,(7, 1930816800, 0) ,(7, 1951376400, 1) ,(7, 1962871200, 0) ,(7, 1983430800, 1) ,(7, 1994320800, 0) ,(7, 2014880400, 1) ,(7, 2025770400, 0) ,(7, 2046330000, 1) ,(7, 2057220000, 0) ,(7, 2077779600, 1) ,(7, 2088669600, 0) ,(7, 2109229200, 1) ,(7, 2120119200, 0) ,(7, 2140678800, 1);
INSERT INTO time_zone_transition_type (Time_zone_id,  Transition_type_id, Offset, Is_DST, Abbreviation) VALUES   (1, 0, 7200, 1, 'MEST') ,(1, 1, 3600, 0, 'MET')  ,(1, 2, 7200, 1, 'MEST') ,(1, 3, 3600, 0, 'MET')  ,(2, 0, 0, 0, 'UTC')  ,(3, 0, 9000, 0, 'MMT') ,(3, 1, 12648, 1, 'MST')  ,(3, 2, 9048, 0, 'MMT') ,(3, 3, 16248, 1, 'MDST')  ,(3, 4, 10800, 0, 'MSK') ,(3, 5, 14400, 1, 'MSD')  ,(3, 6, 18000, 1, 'MSD') ,(3, 7, 7200, 0, 'EET')  ,(3, 8, 10800, 0, 'MSK') ,(3, 9, 14400, 1, 'MSD')  ,(3, 10, 10800, 1, 'EEST') ,(3, 11, 7200, 0, 'EET')  ,(4, 0, 9000, 0, 'MMT') ,(4, 1, 12648, 1, 'MST')  ,(4, 2, 9048, 0, 'MMT') ,(4, 3, 16248, 1, 'MDST')  ,(4, 4, 10800, 0, 'MSK') ,(4, 5, 14400, 1, 'MSD')  ,(4, 6, 18000, 1, 'MSD') ,(4, 7, 7200, 0, 'EET')  ,(4, 8, 10800, 0, 'MSK') ,(4, 9, 14400, 1, 'MSD')  ,(4, 10, 10800, 1, 'EEST') ,(4, 11, 7200, 0, 'EET')  ,(5, 0, 32400, 0, 'CJT') ,(5, 1, 32400, 0, 'JST'), (6, 0, 3600,0, 'CET'),(6, 1, 7200,1, 'CEST'),(6, 2, 7200,1, 'CEST'),(6, 3, 3600,0, 'CET') ,(7, 0, -25200, 1, 'PDT') ,(7, 1, -28800, 0, 'PST') ,(7, 2, -25200, 1, 'PWT') ,(7, 3, -25200, 1, 'PPT');
INSERT INTO time_zone_leap_second   (Transition_time, Correction) VALUES   (78796800, 1) ,(94694401, 2) ,(126230402, 3) ,(157766403, 4) ,(189302404, 5) ,(220924805, 6) ,(252460806, 7) ,(283996807, 8) ,(315532808, 9) ,(362793609, 10) ,(394329610, 11) ,(425865611, 12) ,(489024012, 13) ,(567993613, 14) ,(631152014, 15) ,(662688015, 16) ,(709948816, 17) ,(741484817, 18) ,(773020818, 19) ,(820454419, 20) ,(867715220, 21) ,(915148821, 22);
