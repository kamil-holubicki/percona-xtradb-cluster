drop table if exists t1,t2,t3,t4;
CREATE TABLE t1 (
Period smallint(4) unsigned zerofill DEFAULT '0000' NOT NULL,
Varor_period smallint(4) unsigned DEFAULT '0' NOT NULL
);
INSERT INTO t1 VALUES (9410,9412);
select period from t1;
period
9410
select * from t1;
Period	Varor_period
9410	9412
select t1.* from t1;
Period	Varor_period
9410	9412
CREATE TABLE t2 (
auto int not null auto_increment,
fld1 int(6) unsigned zerofill DEFAULT '000000' NOT NULL,
companynr tinyint(2) unsigned zerofill DEFAULT '00' NOT NULL,
fld3 char(30) DEFAULT '' NOT NULL,
fld4 char(35) DEFAULT '' NOT NULL,
fld5 char(35) DEFAULT '' NOT NULL,
fld6 char(4) DEFAULT '' NOT NULL,
UNIQUE fld1 (fld1),
KEY fld3 (fld3),
PRIMARY KEY (auto)
);
select t2.fld3 from t2 where companynr = 58 and fld3 like "%imaginable%";
fld3
imaginable
select fld3 from t2 where fld3 like "%cultivation" ;
fld3
cultivation
select t2.fld3,companynr from t2 where companynr = 57+1 order by fld3;
fld3	companynr
concoct	58
druggists	58
engrossing	58
Eurydice	58
exclaimers	58
ferociousness	58
hopelessness	58
Huey	58
imaginable	58
judges	58
merging	58
ostrich	58
peering	58
Phelps	58
presumes	58
Ruth	58
sentences	58
Shylock	58
straggled	58
synergy	58
thanking	58
tying	58
unlocks	58
select fld3,companynr from t2 where companynr = 58 order by fld3;
fld3	companynr
concoct	58
druggists	58
engrossing	58
Eurydice	58
exclaimers	58
ferociousness	58
hopelessness	58
Huey	58
imaginable	58
judges	58
merging	58
ostrich	58
peering	58
Phelps	58
presumes	58
Ruth	58
sentences	58
Shylock	58
straggled	58
synergy	58
thanking	58
tying	58
unlocks	58
select fld3 from t2 order by fld3 desc limit 10;
fld3
youthfulness
yelped
Wotan
workers
Witt
witchcraft
Winsett
Willy
willed
wildcats
select fld3 from t2 order by fld3 desc limit 5;
fld3
youthfulness
yelped
Wotan
workers
Witt
select fld3 from t2 order by fld3 desc limit 5,5;
fld3
witchcraft
Winsett
Willy
willed
wildcats
select t2.fld3 from t2 where fld3 = 'honeysuckle';
fld3
honeysuckle
select t2.fld3 from t2 where fld3 LIKE 'honeysuckl_';
fld3
honeysuckle
select t2.fld3 from t2 where fld3 LIKE 'hon_ysuckl_';
fld3
honeysuckle
select t2.fld3 from t2 where fld3 LIKE 'honeysuckle%';
fld3
honeysuckle
select t2.fld3 from t2 where fld3 LIKE 'h%le';
fld3
honeysuckle
select t2.fld3 from t2 where fld3 LIKE 'honeysuckle_';
fld3
select t2.fld3 from t2 where fld3 LIKE 'don_t_find_me_please%';
fld3
explain select t2.fld3 from t2 where fld3 = 'honeysuckle';
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ref	fld3	fld3	30	const	1	Using where; Using index
explain select fld3 from t2 ignore index (fld3) where fld3 = 'honeysuckle';
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
explain select fld3 from t2 use index (fld1) where fld3 = 'honeysuckle';
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
explain select fld3 from t2 use index (fld3) where fld3 = 'honeysuckle';
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ref	fld3	fld3	30	const	1	Using where; Using index
explain select fld3 from t2 use index (fld1,fld3) where fld3 = 'honeysuckle';
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ref	fld3	fld3	30	const	1	Using where; Using index
explain select fld3 from t2 ignore index (fld3,not_used);
ERROR 42000: Key column 'not_used' doesn't exist in table
explain select fld3 from t2 use index (not_used);
ERROR 42000: Key column 'not_used' doesn't exist in table
select t2.fld3 from t2 where fld3 >= 'honeysuckle' and fld3 <= 'honoring' order by fld3;
fld3
honeysuckle
honoring
explain select t2.fld3 from t2 where fld3 >= 'honeysuckle' and fld3 <= 'honoring' order by fld3;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	range	fld3	fld3	30	NULL	2	Using where; Using index
select fld1,fld3 from t2 where fld3="Colombo" or fld3 = "nondecreasing" order by fld3;
fld1	fld3
148504	Colombo
068305	Colombo
000000	nondecreasing
select fld1,fld3 from t2 where companynr = 37 and fld3 = 'appendixes';
fld1	fld3
232605	appendixes
1232605	appendixes
1232606	appendixes
1232607	appendixes
1232608	appendixes
1232609	appendixes
select fld1 from t2 where fld1=250501 or fld1="250502";
fld1
250501
250502
explain select fld1 from t2 where fld1=250501 or fld1="250502";
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	range	fld1	fld1	4	NULL	2	Using where; Using index
select fld1 from t2 where fld1=250501 or fld1=250502 or fld1 >= 250505 and fld1 <= 250601 or fld1 between 250501 and 250502;
fld1
250501
250502
250505
250601
explain select fld1 from t2 where fld1=250501 or fld1=250502 or fld1 >= 250505 and fld1 <= 250601 or fld1 between 250501 and 250502;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	range	fld1	fld1	4	NULL	4	Using where; Using index
select fld1,fld3 from t2 where companynr = 37 and fld3 like 'f%';
fld1	fld3
218401	faithful
018007	fanatic
228311	fated
018017	featherweight
218022	feed
088303	feminine
058004	Fenton
038017	fetched
018054	fetters
208101	fiftieth
238007	filial
013606	fingerings
218008	finishers
038205	firearm
188505	fitting
202301	Fitzpatrick
238008	fixedly
012001	flanking
018103	flint
018104	flopping
188007	flurried
013602	foldout
226205	foothill
232102	forgivably
228306	forthcoming
186002	freakish
208113	freest
231315	freezes
036002	funereal
226209	furnishings
198006	furthermore
select fld3 from t2 where fld3 like "L%" and fld3 = "ok";
fld3
select fld3 from t2 where (fld3 like "C%" and fld3 = "Chantilly");
fld3
Chantilly
select fld1,fld3 from t2 where fld1 like "25050%";
fld1	fld3
250501	poisoning
250502	Iraqis
250503	heaving
250504	population
250505	bomb
select fld1,fld3 from t2 where fld1 like "25050_";
fld1	fld3
250501	poisoning
250502	Iraqis
250503	heaving
250504	population
250505	bomb
select distinct companynr from t2;
companynr
00
37
36
50
58
29
40
53
65
41
34
68
select distinct companynr from t2 order by companynr;
companynr
00
29
34
36
37
40
41
50
53
58
65
68
select distinct companynr from t2 order by companynr desc;
companynr
68
65
58
53
50
41
40
37
36
34
29
00
select distinct t2.fld3,period from t2,t1 where companynr=37 and fld3 like "O%";
fld3	period
obliterates	9410
offload	9410
opaquely	9410
organizer	9410
overestimating	9410
overlay	9410
select distinct fld3 from t2 where companynr = 34 order by fld3;
fld3
absentee
accessed
ahead
alphabetic
Asiaticizations
attitude
aye
bankruptcies
belays
Blythe
bomb
boulevard
bulldozes
cannot
caressing
charcoal
checksumming
chess
clubroom
colorful
cosy
creator
crying
Darius
diffusing
duality
Eiffel
Epiphany
Ernestine
explorers
exterminated
famine
forked
Gershwins
heaving
Hodges
Iraqis
Italianization
Lagos
landslide
libretto
Majorca
mastering
narrowed
occurred
offerers
Palestine
Peruvianizes
pharmaceutic
poisoning
population
Pygmalion
rats
realest
recording
regimented
retransmitting
reviver
rouses
scars
sicker
sleepwalk
stopped
sugars
translatable
uncles
unexpected
uprisings
versatility
vest
select distinct fld3 from t2 limit 10;
fld3
abates
abiding
Abraham
abrogating
absentee
abut
accessed
accruing
accumulating
accuracies
select distinct fld3 from t2 having fld3 like "A%" limit 10;
fld3
abates
abiding
Abraham
abrogating
absentee
abut
accessed
accruing
accumulating
accuracies
select distinct substring(fld3,1,3) from t2 where fld3 like "A%";
substring(fld3,1,3)
aba
abi
Abr
abs
abu
acc
acq
acu
Ade
adj
Adl
adm
Ado
ads
adv
aer
aff
afi
afl
afo
agi
ahe
aim
air
Ald
alg
ali
all
alp
alr
ama
ame
amm
ana
and
ane
Ang
ani
Ann
Ant
api
app
aqu
Ara
arc
Arm
arr
Art
Asi
ask
asp
ass
ast
att
aud
Aug
aut
ave
avo
awe
aye
Azt
select distinct substring(fld3,1,3) as a from t2 having a like "A%" order by a limit 10;
a
aba
abi
Abr
abs
abu
acc
acq
acu
Ade
adj
select distinct substring(fld3,1,3) from t2 where fld3 like "A%" limit 10;
substring(fld3,1,3)
aba
abi
Abr
abs
abu
acc
acq
acu
Ade
adj
select distinct substring(fld3,1,3) as a from t2 having a like "A%" limit 10;
a
aba
abi
Abr
abs
abu
acc
acq
acu
Ade
adj
create table t3 (
period    int not null,
name      char(32) not null,
companynr int not null,
price     double(11,0),
price2     double(11,0),
key (period),
key (name)
);
create temporary table tmp engine = myisam select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
insert into tmp select * from t3;
insert into t3 select * from tmp;
alter table t3 add t2nr int not null auto_increment primary key first;
drop table tmp;
SET SQL_BIG_TABLES=1;
select distinct concat(fld3," ",fld3) as namn from t2,t3 where t2.fld1=t3.t2nr order by namn limit 10;
namn
Abraham Abraham
abrogating abrogating
admonishing admonishing
Adolph Adolph
afield afield
aging aging
ammonium ammonium
analyzable analyzable
animals animals
animized animized
SET SQL_BIG_TABLES=0;
select distinct concat(fld3," ",fld3) from t2,t3 where t2.fld1=t3.t2nr order by fld3 limit 10;
concat(fld3," ",fld3)
Abraham Abraham
abrogating abrogating
admonishing admonishing
Adolph Adolph
afield afield
aging aging
ammonium ammonium
analyzable analyzable
animals animals
animized animized
select distinct fld5 from t2 limit 10;
fld5
neat
Steinberg
jarring
tinily
balled
persist
attainments
fanatic
measures
rightfulness
select distinct fld3,count(*) from t2 group by companynr,fld3 limit 10;
fld3	count(*)
affixed	1
and	1
annoyers	1
Anthony	1
assayed	1
assurers	1
attendants	1
bedlam	1
bedpost	1
boasted	1
SET SQL_BIG_TABLES=1;
select distinct fld3,count(*) from t2 group by companynr,fld3 limit 10;
fld3	count(*)
affixed	1
and	1
annoyers	1
Anthony	1
assayed	1
assurers	1
attendants	1
bedlam	1
bedpost	1
boasted	1
SET SQL_BIG_TABLES=0;
select distinct fld3,repeat("a",length(fld3)),count(*) from t2 group by companynr,fld3 limit 100,10;
fld3	repeat("a",length(fld3))	count(*)
circus	aaaaaa	1
cited	aaaaa	1
Colombo	aaaaaaa	1
congresswoman	aaaaaaaaaaaaa	1
contrition	aaaaaaaaaa	1
corny	aaaaa	1
cultivation	aaaaaaaaaaa	1
definiteness	aaaaaaaaaaaa	1
demultiplex	aaaaaaaaaaa	1
disappointing	aaaaaaaaaaaaa	1
select distinct companynr,rtrim(space(512+companynr)) from t3 order by 1,2;
companynr	rtrim(space(512+companynr))
37	
78	
101	
154	
311	
447	
512	
select distinct fld3 from t2,t3 where t2.companynr = 34 and t2.fld1=t3.t2nr order by fld3;
fld3
explain select t3.t2nr,fld3 from t2,t3 where t2.companynr = 34 and t2.fld1=t3.t2nr order by t3.t2nr,fld3;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	fld1	NULL	NULL	NULL	1199	Using where; Using temporary; Using filesort
1	SIMPLE	t3	eq_ref	PRIMARY	PRIMARY	4	test.t2.fld1	1	Using where; Using index
explain select * from t3 as t1,t3 where t1.period=t3.period order by t3.period;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t1	ALL	period	NULL	NULL	NULL	41810	Using temporary; Using filesort
1	SIMPLE	t3	ref	period	period	4	test.t1.period	4181	
explain select * from t3 as t1,t3 where t1.period=t3.period order by t3.period limit 10;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t3	index	period	period	4	NULL	41810	
1	SIMPLE	t1	ref	period	period	4	test.t3.period	4181	
explain select * from t3 as t1,t3 where t1.period=t3.period order by t1.period limit 10;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t1	index	period	period	4	NULL	41810	
1	SIMPLE	t3	ref	period	period	4	test.t1.period	4181	
select period from t1;
period
9410
select period from t1 where period=1900;
period
select fld3,period from t1,t2 where fld1 = 011401 order by period;
fld3	period
breaking	9410
select fld3,period from t2,t3 where t2.fld1 = 011401 and t2.fld1=t3.t2nr and t3.period=1001;
fld3	period
breaking	1001
explain select fld3,period from t2,t3 where t2.fld1 = 011401 and t3.t2nr=t2.fld1 and 1001 = t3.period;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	const	fld1	fld1	4	const	1	
1	SIMPLE	t3	const	PRIMARY,period	PRIMARY	4	const	1	
select fld3,period from t2,t1 where companynr*10 = 37*10;
fld3	period
breaking	9410
Romans	9410
intercepted	9410
bewilderingly	9410
astound	9410
admonishing	9410
sumac	9410
flanking	9410
combed	9410
subjective	9410
scatterbrain	9410
Eulerian	9410
Kane	9410
overlay	9410
perturb	9410
goblins	9410
annihilates	9410
Wotan	9410
snatching	9410
concludes	9410
laterally	9410
yelped	9410
grazing	9410
Baird	9410
celery	9410
misunderstander	9410
handgun	9410
foldout	9410
mystic	9410
succumbed	9410
Nabisco	9410
fingerings	9410
aging	9410
afield	9410
ammonium	9410
boat	9410
intelligibility	9410
Augustine	9410
teethe	9410
dreaded	9410
scholastics	9410
audiology	9410
wallet	9410
parters	9410
eschew	9410
quitter	9410
neat	9410
Steinberg	9410
jarring	9410
tinily	9410
balled	9410
persist	9410
attainments	9410
fanatic	9410
measures	9410
rightfulness	9410
capably	9410
impulsive	9410
starlet	9410
terminators	9410
untying	9410
announces	9410
featherweight	9410
pessimist	9410
daughter	9410
decliner	9410
lawgiver	9410
stated	9410
readable	9410
attrition	9410
cascade	9410
motors	9410
interrogate	9410
pests	9410
stairway	9410
dopers	9410
testicle	9410
Parsifal	9410
leavings	9410
postulation	9410
squeaking	9410
contrasted	9410
leftover	9410
whiteners	9410
erases	9410
Punjab	9410
Merritt	9410
Quixotism	9410
sweetish	9410
dogging	9410
scornfully	9410
bellow	9410
bills	9410
cupboard	9410
sureties	9410
puddings	9410
fetters	9410
bivalves	9410
incurring	9410
Adolph	9410
pithed	9410
Miles	9410
trimmings	9410
tragedies	9410
skulking	9410
flint	9410
flopping	9410
relaxing	9410
offload	9410
suites	9410
lists	9410
animized	9410
multilayer	9410
standardizes	9410
Judas	9410
vacuuming	9410
dentally	9410
humanness	9410
inch	9410
Weissmuller	9410
irresponsibly	9410
luckily	9410
culled	9410
medical	9410
bloodbath	9410
subschema	9410
animals	9410
Micronesia	9410
repetitions	9410
Antares	9410
ventilate	9410
pityingly	9410
interdependent	9410
Graves	9410
neonatal	9410
chafe	9410
honoring	9410
realtor	9410
elite	9410
funereal	9410
abrogating	9410
sorters	9410
Conley	9410
lectured	9410
Abraham	9410
Hawaii	9410
cage	9410
hushes	9410
Simla	9410
reporters	9410
Dutchman	9410
descendants	9410
groupings	9410
dissociate	9410
coexist	9410
Beebe	9410
Taoism	9410
Connally	9410
fetched	9410
checkpoints	9410
rusting	9410
galling	9410
obliterates	9410
traitor	9410
resumes	9410
analyzable	9410
terminator	9410
gritty	9410
firearm	9410
minima	9410
Selfridge	9410
disable	9410
witchcraft	9410
betroth	9410
Manhattanize	9410
imprint	9410
peeked	9410
swelling	9410
interrelationships	9410
riser	9410
Gandhian	9410
peacock	9410
bee	9410
kanji	9410
dental	9410
scarf	9410
chasm	9410
insolence	9410
syndicate	9410
alike	9410
imperial	9410
convulsion	9410
railway	9410
validate	9410
normalizes	9410
comprehensive	9410
chewing	9410
denizen	9410
schemer	9410
chronicle	9410
Kline	9410
Anatole	9410
partridges	9410
brunch	9410
recruited	9410
dimensions	9410
Chicana	9410
announced	9410
praised	9410
employing	9410
linear	9410
quagmire	9410
western	9410
relishing	9410
serving	9410
scheduling	9410
lore	9410
eventful	9410
arteriole	9410
disentangle	9410
cured	9410
Fenton	9410
avoidable	9410
drains	9410
detectably	9410
husky	9410
impelling	9410
undoes	9410
evened	9410
squeezes	9410
destroyer	9410
rudeness	9410
beaner	9410
boorish	9410
Everhart	9410
encompass	9410
mushrooms	9410
Alison	9410
externally	9410
pellagra	9410
cult	9410
creek	9410
Huffman	9410
Majorca	9410
governing	9410
gadfly	9410
reassigned	9410
intentness	9410
craziness	9410
psychic	9410
squabbled	9410
burlesque	9410
capped	9410
extracted	9410
DiMaggio	9410
exclamation	9410
subdirectory	9410
Gothicism	9410
feminine	9410
metaphysically	9410
sanding	9410
Miltonism	9410
freakish	9410
index	9410
straight	9410
flurried	9410
denotative	9410
coming	9410
commencements	9410
gentleman	9410
gifted	9410
Shanghais	9410
sportswriting	9410
sloping	9410
navies	9410
leaflet	9410
shooter	9410
Joplin	9410
babies	9410
assails	9410
admiring	9410
swaying	9410
Goldstine	9410
fitting	9410
Norwalk	9410
analogy	9410
deludes	9410
cokes	9410
Clayton	9410
exhausts	9410
causality	9410
sating	9410
icon	9410
throttles	9410
communicants	9410
dehydrate	9410
priceless	9410
publicly	9410
incidentals	9410
commonplace	9410
mumbles	9410
furthermore	9410
cautioned	9410
parametrized	9410
registration	9410
sadly	9410
positioning	9410
babysitting	9410
eternal	9410
hoarder	9410
congregates	9410
rains	9410
workers	9410
sags	9410
unplug	9410
garage	9410
boulder	9410
specifics	9410
Teresa	9410
Winsett	9410
convenient	9410
buckboards	9410
amenities	9410
resplendent	9410
sews	9410
participated	9410
Simon	9410
certificates	9410
Fitzpatrick	9410
Evanston	9410
misted	9410
textures	9410
save	9410
count	9410
rightful	9410
chaperone	9410
Lizzy	9410
clenched	9410
effortlessly	9410
accessed	9410
beaters	9410
Hornblower	9410
vests	9410
indulgences	9410
infallibly	9410
unwilling	9410
excrete	9410
spools	9410
crunches	9410
overestimating	9410
ineffective	9410
humiliation	9410
sophomore	9410
star	9410
rifles	9410
dialysis	9410
arriving	9410
indulge	9410
clockers	9410
languages	9410
Antarctica	9410
percentage	9410
ceiling	9410
specification	9410
regimented	9410
ciphers	9410
pictures	9410
serpents	9410
allot	9410
realized	9410
mayoral	9410
opaquely	9410
hostess	9410
fiftieth	9410
incorrectly	9410
decomposition	9410
stranglings	9410
mixture	9410
electroencephalography	9410
similarities	9410
charges	9410
freest	9410
Greenberg	9410
tinting	9410
expelled	9410
warm	9410
smoothed	9410
deductions	9410
Romano	9410
bitterroot	9410
corset	9410
securing	9410
environing	9410
cute	9410
Crays	9410
heiress	9410
inform	9410
avenge	9410
universals	9410
Kinsey	9410
ravines	9410
bestseller	9410
equilibrium	9410
extents	9410
relatively	9410
pressure	9410
critiques	9410
befouled	9410
rightfully	9410
mechanizing	9410
Latinizes	9410
timesharing	9410
Aden	9410
embassies	9410
males	9410
shapelessly	9410
mastering	9410
Newtonian	9410
finishers	9410
abates	9410
teem	9410
kiting	9410
stodgy	9410
feed	9410
guitars	9410
airships	9410
store	9410
denounces	9410
Pyle	9410
Saxony	9410
serializations	9410
Peruvian	9410
taxonomically	9410
kingdom	9410
stint	9410
Sault	9410
faithful	9410
Ganymede	9410
tidiness	9410
gainful	9410
contrary	9410
Tipperary	9410
tropics	9410
theorizers	9410
renew	9410
already	9410
terminal	9410
Hegelian	9410
hypothesizer	9410
warningly	9410
journalizing	9410
nested	9410
Lars	9410
saplings	9410
foothill	9410
labeled	9410
imperiously	9410
reporters	9410
furnishings	9410
precipitable	9410
discounts	9410
excises	9410
Stalin	9410
despot	9410
ripeness	9410
Arabia	9410
unruly	9410
mournfulness	9410
boom	9410
slaughter	9410
Sabine	9410
handy	9410
rural	9410
organizer	9410
shipyard	9410
civics	9410
inaccuracy	9410
rules	9410
juveniles	9410
comprised	9410
investigations	9410
stabilizes	9410
seminaries	9410
Hunter	9410
sporty	9410
test	9410
weasels	9410
CERN	9410
tempering	9410
afore	9410
Galatean	9410
techniques	9410
error	9410
veranda	9410
severely	9410
Cassites	9410
forthcoming	9410
guides	9410
vanish	9410
lied	9410
sawtooth	9410
fated	9410
gradually	9410
widens	9410
preclude	9410
evenhandedly	9410
percentage	9410
disobedience	9410
humility	9410
gleaning	9410
petted	9410
bloater	9410
minion	9410
marginal	9410
apiary	9410
measures	9410
precaution	9410
repelled	9410
primary	9410
coverings	9410
Artemia	9410
navigate	9410
spatial	9410
Gurkha	9410
meanwhile	9410
Melinda	9410
Butterfield	9410
Aldrich	9410
previewing	9410
glut	9410
unaffected	9410
inmate	9410
mineral	9410
impending	9410
meditation	9410
ideas	9410
miniaturizes	9410
lewdly	9410
title	9410
youthfulness	9410
creak	9410
Chippewa	9410
clamored	9410
freezes	9410
forgivably	9410
reduce	9410
McGovern	9410
Nazis	9410
epistle	9410
socializes	9410
conceptions	9410
Kevin	9410
uncovering	9410
chews	9410
appendixes	9410
appendixes	9410
appendixes	9410
appendixes	9410
appendixes	9410
appendixes	9410
raining	9410
infest	9410
compartment	9410
minting	9410
ducks	9410
roped	9410
waltz	9410
Lillian	9410
repressions	9410
chillingly	9410
noncritical	9410
lithograph	9410
spongers	9410
parenthood	9410
posed	9410
instruments	9410
filial	9410
fixedly	9410
relives	9410
Pandora	9410
watering	9410
ungrateful	9410
secures	9410
poison	9410
dusted	9410
encompasses	9410
presentation	9410
Kantian	9410
select fld3,period,price,price2 from t2,t3 where t2.fld1=t3.t2nr and period >= 1001 and period <= 1002 and t2.companynr = 37 order by fld3,period, price;
fld3	period	price	price2
admonishing	1002	28357832	8723648
analyzable	1002	28357832	8723648
annihilates	1001	5987435	234724
Antares	1002	28357832	8723648
astound	1001	5987435	234724
audiology	1001	5987435	234724
Augustine	1002	28357832	8723648
Baird	1002	28357832	8723648
bewilderingly	1001	5987435	234724
breaking	1001	5987435	234724
Conley	1001	5987435	234724
dentally	1002	28357832	8723648
dissociate	1002	28357832	8723648
elite	1001	5987435	234724
eschew	1001	5987435	234724
Eulerian	1001	5987435	234724
flanking	1001	5987435	234724
foldout	1002	28357832	8723648
funereal	1002	28357832	8723648
galling	1002	28357832	8723648
Graves	1001	5987435	234724
grazing	1001	5987435	234724
groupings	1001	5987435	234724
handgun	1001	5987435	234724
humility	1002	28357832	8723648
impulsive	1002	28357832	8723648
inch	1001	5987435	234724
intelligibility	1001	5987435	234724
jarring	1001	5987435	234724
lawgiver	1001	5987435	234724
lectured	1002	28357832	8723648
Merritt	1002	28357832	8723648
neonatal	1001	5987435	234724
offload	1002	28357832	8723648
parters	1002	28357832	8723648
pityingly	1002	28357832	8723648
puddings	1002	28357832	8723648
Punjab	1001	5987435	234724
quitter	1002	28357832	8723648
realtor	1001	5987435	234724
relaxing	1001	5987435	234724
repetitions	1001	5987435	234724
resumes	1001	5987435	234724
Romans	1002	28357832	8723648
rusting	1001	5987435	234724
scholastics	1001	5987435	234724
skulking	1002	28357832	8723648
stated	1002	28357832	8723648
suites	1002	28357832	8723648
sureties	1001	5987435	234724
testicle	1002	28357832	8723648
tinily	1002	28357832	8723648
tragedies	1001	5987435	234724
trimmings	1001	5987435	234724
vacuuming	1001	5987435	234724
ventilate	1001	5987435	234724
wallet	1001	5987435	234724
Weissmuller	1002	28357832	8723648
Wotan	1002	28357832	8723648
select t2.fld1,fld3,period,price,price2 from t2,t3 where t2.fld1>= 18201 and t2.fld1 <= 18811 and t2.fld1=t3.t2nr and period = 1001 and t2.companynr = 37;
fld1	fld3	period	price	price2
018201	relaxing	1001	5987435	234724
018601	vacuuming	1001	5987435	234724
018801	inch	1001	5987435	234724
018811	repetitions	1001	5987435	234724
create table t4 (
companynr tinyint(2) unsigned zerofill NOT NULL default '00',
companyname char(30) NOT NULL default '',
PRIMARY KEY (companynr),
UNIQUE KEY companyname(companyname)
) ENGINE=MyISAM MAX_ROWS=50 PACK_KEYS=1 COMMENT='companynames';
select STRAIGHT_JOIN t2.companynr,companyname from t4,t2 where t2.companynr=t4.companynr group by t2.companynr;
companynr	companyname
00	Unknown
29	company 1
34	company 2
36	company 3
37	company 4
40	company 5
41	company 6
50	company 11
53	company 7
58	company 8
65	company 9
68	company 10
select SQL_SMALL_RESULT t2.companynr,companyname from t4,t2 where t2.companynr=t4.companynr group by t2.companynr;
companynr	companyname
00	Unknown
29	company 1
34	company 2
36	company 3
37	company 4
40	company 5
41	company 6
50	company 11
53	company 7
58	company 8
65	company 9
68	company 10
select * from t1,t1 t12;
Period	Varor_period	Period	Varor_period
9410	9412	9410	9412
select t2.fld1,t22.fld1 from t2,t2 t22 where t2.fld1 >= 250501 and t2.fld1 <= 250505 and t22.fld1 >= 250501 and t22.fld1 <= 250505;
fld1	fld1
250501	250501
250502	250501
250503	250501
250504	250501
250505	250501
250501	250502
250502	250502
250503	250502
250504	250502
250505	250502
250501	250503
250502	250503
250503	250503
250504	250503
250505	250503
250501	250504
250502	250504
250503	250504
250504	250504
250505	250504
250501	250505
250502	250505
250503	250505
250504	250505
250505	250505
insert into t2 (fld1, companynr) values (999999,99);
select t2.companynr,companyname from t2 left join t4 using (companynr) where t4.companynr is null;
companynr	companyname
99	NULL
select count(*) from t2 left join t4 using (companynr) where t4.companynr is not null;
count(*)
1199
explain select t2.companynr,companyname from t2 left join t4 using (companynr) where t4.companynr is null;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1200	
1	SIMPLE	t4	eq_ref	PRIMARY	PRIMARY	1	test.t2.companynr	1	Using where; Not exists
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr is null;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t4	ALL	NULL	NULL	NULL	NULL	12	
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1200	Using where; Not exists
delete from t2 where fld1=999999;
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr > 0;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
1	SIMPLE	t4	eq_ref	PRIMARY	PRIMARY	1	test.t2.companynr	1	
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr > 0 or t2.companynr < 0;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
1	SIMPLE	t4	eq_ref	PRIMARY	PRIMARY	1	test.t2.companynr	1	
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr > 0 and t4.companynr > 0;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
1	SIMPLE	t4	eq_ref	PRIMARY	PRIMARY	1	test.t2.companynr	1	Using where
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr > 0 or t2.companynr is null;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t4	ALL	NULL	NULL	NULL	NULL	12	
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where t2.companynr > 0 or t2.companynr < 0 or t4.companynr > 0;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t4	ALL	PRIMARY	NULL	NULL	NULL	12	
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
explain select t2.companynr,companyname from t4 left join t2 using (companynr) where ifnull(t2.companynr,1)>0;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t4	ALL	NULL	NULL	NULL	NULL	12	
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
select distinct t2.companynr,t4.companynr from t2,t4 where t2.companynr=t4.companynr+1;
companynr	companynr
37	36
41	40
explain select distinct t2.companynr,t4.companynr from t2,t4 where t2.companynr=t4.companynr+1;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using temporary
1	SIMPLE	t4	index	NULL	PRIMARY	1	NULL	12	Using where; Using index
select t2.fld1,t2.companynr,fld3,period from t3,t2 where t2.fld1 = 38208 and t2.fld1=t3.t2nr and period = 1008 or t2.fld1 = 38008 and t2.fld1 =t3.t2nr and period = 1008;
fld1	companynr	fld3	period
038008	37	reporters	1008
038208	37	Selfridge	1008
select t2.fld1,t2.companynr,fld3,period from t3,t2 where (t2.fld1 = 38208 or t2.fld1 = 38008) and t2.fld1=t3.t2nr and period>=1008 and period<=1009;
fld1	companynr	fld3	period
038008	37	reporters	1008
038208	37	Selfridge	1008
select t2.fld1,t2.companynr,fld3,period from t3,t2 where (t3.t2nr = 38208 or t3.t2nr = 38008) and t2.fld1=t3.t2nr and period>=1008 and period<=1009;
fld1	companynr	fld3	period
038008	37	reporters	1008
038208	37	Selfridge	1008
select period from t1 where (((period > 0) or period < 10000 or (period = 1900)) and (period=1900 and period <= 1901) or (period=1903 and (period=1903)) and period>=1902) or ((period=1904 or period=1905) or (period=1906 or period>1907)) or (period=1908 and period = 1909);
period
9410
select period from t1 where ((period > 0 and period < 1) or (((period > 0 and period < 100) and (period > 10)) or (period > 10)) or (period > 0 and (period > 5 or period > 6)));
period
9410
select a.fld1 from t2 as a,t2 b where ((a.fld1 = 250501 and a.fld1=b.fld1) or a.fld1=250502 or a.fld1=250503 or (a.fld1=250505 and a.fld1<=b.fld1 and b.fld1>=a.fld1)) and a.fld1=b.fld1;
fld1
250501
250502
250503
250505
select fld1 from t2 where fld1 in (250502,98005,98006,250503,250605,250606) and fld1 >=250502 and fld1 not in (250605,250606);
fld1
250502
250503
select fld1 from t2 where fld1 between 250502 and 250504;
fld1
250502
250503
250504
select fld3 from t2 where (((fld3 like "_%L%" ) or (fld3 like "%ok%")) and ( fld3 like "L%" or fld3 like "G%")) and fld3 like "L%" ;
fld3
label
labeled
labeled
landslide
laterally
leaflet
lewdly
Lillian
luckily
select count(*) from t1;
count(*)
1
select companynr,count(*),sum(fld1) from t2 group by companynr;
companynr	count(*)	sum(fld1)
00	82	10355753
29	95	14473298
34	70	17788966
36	215	22786296
37	588	83602098
40	37	6618386
41	52	12816335
50	11	1595438
53	4	793210
58	23	2254293
65	10	2284055
68	12	3097288
select companynr,count(*) from t2 group by companynr order by companynr desc limit 5;
companynr	count(*)
68	12
65	10
58	23
53	4
50	11
select count(*),min(fld4),max(fld4),sum(fld1),avg(fld1),std(fld1),variance(fld1) from t2 where companynr = 34 and fld4<>"";
count(*)	min(fld4)	max(fld4)	sum(fld1)	avg(fld1)	std(fld1)	variance(fld1)
70	absentee	vest	17788966	254128.0857	3272.5940	10709871.3069
explain extended select count(*),min(fld4),max(fld4),sum(fld1),avg(fld1),std(fld1),variance(fld1) from t2 where companynr = 34 and fld4<>"";
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	Using where
Warnings:
Note	1003	select count(0) AS `count(*)`,min(test.t2.fld4) AS `min(fld4)`,max(test.t2.fld4) AS `max(fld4)`,sum(test.t2.fld1) AS `sum(fld1)`,avg(test.t2.fld1) AS `avg(fld1)`,std(test.t2.fld1) AS `std(fld1)`,variance(test.t2.fld1) AS `variance(fld1)` from test.t2 where ((test.t2.companynr = 34) and (test.t2.fld4 <> _latin1''))
select companynr,count(*),min(fld4),max(fld4),sum(fld1),avg(fld1),std(fld1),variance(fld1) from t2 group by companynr limit 3;
companynr	count(*)	min(fld4)	max(fld4)	sum(fld1)	avg(fld1)	std(fld1)	variance(fld1)
00	82	Anthony	windmills	10355753	126289.6707	115550.9757	13352027981.7087
29	95	abut	wetness	14473298	152350.5053	8368.5480	70032594.9026
34	70	absentee	vest	17788966	254128.0857	3272.5940	10709871.3069
select companynr,t2nr,count(price),sum(price),min(price),max(price),avg(price) from t3 where companynr = 37 group by companynr,t2nr limit 10;
companynr	t2nr	count(price)	sum(price)	min(price)	max(price)	avg(price)
37	1	1	5987435	5987435	5987435	5987435.0000
37	2	1	28357832	28357832	28357832	28357832.0000
37	3	1	39654943	39654943	39654943	39654943.0000
37	11	1	5987435	5987435	5987435	5987435.0000
37	12	1	28357832	28357832	28357832	28357832.0000
37	13	1	39654943	39654943	39654943	39654943.0000
37	21	1	5987435	5987435	5987435	5987435.0000
37	22	1	28357832	28357832	28357832	28357832.0000
37	23	1	39654943	39654943	39654943	39654943.0000
37	31	1	5987435	5987435	5987435	5987435.0000
select /*! SQL_SMALL_RESULT */ companynr,t2nr,count(price),sum(price),min(price),max(price),avg(price) from t3 where companynr = 37 group by companynr,t2nr limit 10;
companynr	t2nr	count(price)	sum(price)	min(price)	max(price)	avg(price)
37	1	1	5987435	5987435	5987435	5987435.0000
37	2	1	28357832	28357832	28357832	28357832.0000
37	3	1	39654943	39654943	39654943	39654943.0000
37	11	1	5987435	5987435	5987435	5987435.0000
37	12	1	28357832	28357832	28357832	28357832.0000
37	13	1	39654943	39654943	39654943	39654943.0000
37	21	1	5987435	5987435	5987435	5987435.0000
37	22	1	28357832	28357832	28357832	28357832.0000
37	23	1	39654943	39654943	39654943	39654943.0000
37	31	1	5987435	5987435	5987435	5987435.0000
select companynr,count(price),sum(price),min(price),max(price),avg(price) from t3 group by companynr ;
companynr	count(price)	sum(price)	min(price)	max(price)	avg(price)
37	12543	309394878010	5987435	39654943	24666736.6667
78	8362	414611089292	726498	98439034	49582766.0000
101	4181	3489454238	834598	834598	834598.0000
154	4181	4112197254950	983543950	983543950	983543950.0000
311	4181	979599938	234298	234298	234298.0000
447	4181	9929180954	2374834	2374834	2374834.0000
512	4181	3288532102	786542	786542	786542.0000
select distinct mod(companynr,10) from t4 group by companynr;
mod(companynr,10)
0
9
4
6
7
1
3
8
5
select distinct 1 from t4 group by companynr;
1
1
select count(distinct fld1) from t2;
count(distinct fld1)
1199
select companynr,count(distinct fld1) from t2 group by companynr;
companynr	count(distinct fld1)
00	82
29	95
34	70
36	215
37	588
40	37
41	52
50	11
53	4
58	23
65	10
68	12
select companynr,count(*) from t2 group by companynr;
companynr	count(*)
00	82
29	95
34	70
36	215
37	588
40	37
41	52
50	11
53	4
58	23
65	10
68	12
select companynr,count(distinct concat(fld1,repeat(65,1000))) from t2 group by companynr;
companynr	count(distinct concat(fld1,repeat(65,1000)))
00	82
29	95
34	70
36	215
37	588
40	37
41	52
50	11
53	4
58	23
65	10
68	12
select companynr,count(distinct concat(fld1,repeat(65,200))) from t2 group by companynr;
companynr	count(distinct concat(fld1,repeat(65,200)))
00	82
29	95
34	70
36	215
37	588
40	37
41	52
50	11
53	4
58	23
65	10
68	12
select companynr,count(distinct floor(fld1/100)) from t2 group by companynr;
companynr	count(distinct floor(fld1/100))
00	47
29	35
34	14
36	69
37	108
40	16
41	11
50	9
53	1
58	1
65	1
68	1
select companynr,count(distinct concat(repeat(65,1000),floor(fld1/100))) from t2 group by companynr;
companynr	count(distinct concat(repeat(65,1000),floor(fld1/100)))
00	47
29	35
34	14
36	69
37	108
40	16
41	11
50	9
53	1
58	1
65	1
68	1
select sum(fld1),fld3 from t2 where fld3="Romans" group by fld1 limit 10;
sum(fld1)	fld3
11402	Romans
select name,count(*) from t3 where name='cloakroom' group by name;
name	count(*)
cloakroom	4181
select name,count(*) from t3 where name='cloakroom' and price>10 group by name;
name	count(*)
cloakroom	4181
select count(*) from t3 where name='cloakroom' and price2=823742;
count(*)
4181
select name,count(*) from t3 where name='cloakroom' and price2=823742 group by name;
name	count(*)
cloakroom	4181
select name,count(*) from t3 where name >= "extramarital" and price <= 39654943 group by name;
name	count(*)
extramarital	4181
gazer	4181
gems	4181
Iranizes	4181
spates	4181
tucked	4181
violinist	4181
select t2.fld3,count(*) from t2,t3 where t2.fld1=158402 and t3.name=t2.fld3 group by t3.name;
fld3	count(*)
spates	4181
select companynr|0,companyname from t4 group by 1;
companynr|0	companyname
0	Unknown
29	company 1
34	company 2
36	company 3
37	company 4
40	company 5
41	company 6
50	company 11
53	company 7
58	company 8
65	company 9
68	company 10
select t2.companynr,companyname,count(*) from t2,t4 where t2.companynr=t4.companynr group by t2.companynr order by companyname;
companynr	companyname	count(*)
29	company 1	95
68	company 10	12
50	company 11	11
34	company 2	70
36	company 3	215
37	company 4	588
40	company 5	37
41	company 6	52
53	company 7	4
58	company 8	23
65	company 9	10
00	Unknown	82
select t2.fld1,count(*) from t2,t3 where t2.fld1=158402 and t3.name=t2.fld3 group by t3.name;
fld1	count(*)
158402	4181
select sum(Period)/count(*) from t1;
sum(Period)/count(*)
9410.00
select companynr,count(price) as "count",sum(price) as "sum" ,abs(sum(price)/count(price)-avg(price)) as "diff",(0+count(price))*companynr as func from t3 group by companynr;
companynr	count	sum	diff	func
37	12543	309394878010	0.0000	464091
78	8362	414611089292	0.0000	652236
101	4181	3489454238	0.0000	422281
154	4181	4112197254950	0.0000	643874
311	4181	979599938	0.0000	1300291
447	4181	9929180954	0.0000	1868907
512	4181	3288532102	0.0000	2140672
select companynr,sum(price)/count(price) as avg from t3 group by companynr having avg > 70000000 order by avg;
companynr	avg
154	983543950.00
select companynr,count(*) from t2 group by companynr order by 2 desc;
companynr	count(*)
37	588
36	215
29	95
00	82
34	70
41	52
40	37
58	23
68	12
50	11
65	10
53	4
select companynr,count(*) from t2 where companynr > 40 group by companynr order by 2 desc;
companynr	count(*)
41	52
58	23
68	12
50	11
65	10
53	4
select t2.fld4,t2.fld1,count(price),sum(price),min(price),max(price),avg(price) from t3,t2 where t3.companynr = 37 and t2.fld1 = t3.t2nr group by fld1,t2.fld4;
fld4	fld1	count(price)	sum(price)	min(price)	max(price)	avg(price)
teethe	000001	1	5987435	5987435	5987435	5987435.0000
dreaded	011401	1	5987435	5987435	5987435	5987435.0000
scholastics	011402	1	28357832	28357832	28357832	28357832.0000
audiology	011403	1	39654943	39654943	39654943	39654943.0000
wallet	011501	1	5987435	5987435	5987435	5987435.0000
parters	011701	1	5987435	5987435	5987435	5987435.0000
eschew	011702	1	28357832	28357832	28357832	28357832.0000
quitter	011703	1	39654943	39654943	39654943	39654943.0000
neat	012001	1	5987435	5987435	5987435	5987435.0000
Steinberg	012003	1	39654943	39654943	39654943	39654943.0000
balled	012301	1	5987435	5987435	5987435	5987435.0000
persist	012302	1	28357832	28357832	28357832	28357832.0000
attainments	012303	1	39654943	39654943	39654943	39654943.0000
capably	012501	1	5987435	5987435	5987435	5987435.0000
impulsive	012602	1	28357832	28357832	28357832	28357832.0000
starlet	012603	1	39654943	39654943	39654943	39654943.0000
featherweight	012701	1	5987435	5987435	5987435	5987435.0000
pessimist	012702	1	28357832	28357832	28357832	28357832.0000
daughter	012703	1	39654943	39654943	39654943	39654943.0000
lawgiver	013601	1	5987435	5987435	5987435	5987435.0000
stated	013602	1	28357832	28357832	28357832	28357832.0000
readable	013603	1	39654943	39654943	39654943	39654943.0000
testicle	013801	1	5987435	5987435	5987435	5987435.0000
Parsifal	013802	1	28357832	28357832	28357832	28357832.0000
leavings	013803	1	39654943	39654943	39654943	39654943.0000
squeaking	013901	1	5987435	5987435	5987435	5987435.0000
contrasted	016001	1	5987435	5987435	5987435	5987435.0000
leftover	016201	1	5987435	5987435	5987435	5987435.0000
whiteners	016202	1	28357832	28357832	28357832	28357832.0000
erases	016301	1	5987435	5987435	5987435	5987435.0000
Punjab	016302	1	28357832	28357832	28357832	28357832.0000
Merritt	016303	1	39654943	39654943	39654943	39654943.0000
sweetish	018001	1	5987435	5987435	5987435	5987435.0000
dogging	018002	1	28357832	28357832	28357832	28357832.0000
scornfully	018003	1	39654943	39654943	39654943	39654943.0000
fetters	018012	1	28357832	28357832	28357832	28357832.0000
bivalves	018013	1	39654943	39654943	39654943	39654943.0000
skulking	018021	1	5987435	5987435	5987435	5987435.0000
flint	018022	1	28357832	28357832	28357832	28357832.0000
flopping	018023	1	39654943	39654943	39654943	39654943.0000
Judas	018032	1	28357832	28357832	28357832	28357832.0000
vacuuming	018033	1	39654943	39654943	39654943	39654943.0000
medical	018041	1	5987435	5987435	5987435	5987435.0000
bloodbath	018042	1	28357832	28357832	28357832	28357832.0000
subschema	018043	1	39654943	39654943	39654943	39654943.0000
interdependent	018051	1	5987435	5987435	5987435	5987435.0000
Graves	018052	1	28357832	28357832	28357832	28357832.0000
neonatal	018053	1	39654943	39654943	39654943	39654943.0000
sorters	018061	1	5987435	5987435	5987435	5987435.0000
epistle	018062	1	28357832	28357832	28357832	28357832.0000
Conley	018101	1	5987435	5987435	5987435	5987435.0000
lectured	018102	1	28357832	28357832	28357832	28357832.0000
Abraham	018103	1	39654943	39654943	39654943	39654943.0000
cage	018201	1	5987435	5987435	5987435	5987435.0000
hushes	018202	1	28357832	28357832	28357832	28357832.0000
Simla	018402	1	28357832	28357832	28357832	28357832.0000
reporters	018403	1	39654943	39654943	39654943	39654943.0000
coexist	018601	1	5987435	5987435	5987435	5987435.0000
Beebe	018602	1	28357832	28357832	28357832	28357832.0000
Taoism	018603	1	39654943	39654943	39654943	39654943.0000
Connally	018801	1	5987435	5987435	5987435	5987435.0000
fetched	018802	1	28357832	28357832	28357832	28357832.0000
checkpoints	018803	1	39654943	39654943	39654943	39654943.0000
gritty	018811	1	5987435	5987435	5987435	5987435.0000
firearm	018812	1	28357832	28357832	28357832	28357832.0000
minima	019101	1	5987435	5987435	5987435	5987435.0000
Selfridge	019102	1	28357832	28357832	28357832	28357832.0000
disable	019103	1	39654943	39654943	39654943	39654943.0000
witchcraft	019201	1	5987435	5987435	5987435	5987435.0000
betroth	030501	1	5987435	5987435	5987435	5987435.0000
Manhattanize	030502	1	28357832	28357832	28357832	28357832.0000
imprint	030503	1	39654943	39654943	39654943	39654943.0000
swelling	031901	1	5987435	5987435	5987435	5987435.0000
interrelationships	036001	1	5987435	5987435	5987435	5987435.0000
riser	036002	1	28357832	28357832	28357832	28357832.0000
bee	038001	1	5987435	5987435	5987435	5987435.0000
kanji	038002	1	28357832	28357832	28357832	28357832.0000
dental	038003	1	39654943	39654943	39654943	39654943.0000
railway	038011	1	5987435	5987435	5987435	5987435.0000
validate	038012	1	28357832	28357832	28357832	28357832.0000
normalizes	038013	1	39654943	39654943	39654943	39654943.0000
Kline	038101	1	5987435	5987435	5987435	5987435.0000
Anatole	038102	1	28357832	28357832	28357832	28357832.0000
partridges	038103	1	39654943	39654943	39654943	39654943.0000
recruited	038201	1	5987435	5987435	5987435	5987435.0000
dimensions	038202	1	28357832	28357832	28357832	28357832.0000
Chicana	038203	1	39654943	39654943	39654943	39654943.0000
select t3.companynr,fld3,sum(price) from t3,t2 where t2.fld1 = t3.t2nr and t3.companynr = 512 group by companynr,fld3;
companynr	fld3	sum(price)
512	boat	786542
512	capably	786542
512	cupboard	786542
512	decliner	786542
512	descendants	786542
512	dopers	786542
512	erases	786542
512	Micronesia	786542
512	Miles	786542
512	skies	786542
select t2.companynr,count(*),min(fld3),max(fld3),sum(price),avg(price) from t2,t3 where t3.companynr >= 30 and t3.companynr <= 58 and t3.t2nr = t2.fld1 and 1+1=2 group by t2.companynr;
companynr	count(*)	min(fld3)	max(fld3)	sum(price)	avg(price)
00	1	Omaha	Omaha	5987435	5987435.0000
36	1	dubbed	dubbed	28357832	28357832.0000
37	83	Abraham	Wotan	1908978016	22999735.1325
50	2	scribbled	tapestry	68012775	34006387.5000
select t3.companynr+0,t3.t2nr,fld3,sum(price) from t3,t2 where t2.fld1 = t3.t2nr and t3.companynr = 37 group by 1,t3.t2nr,fld3,fld3,fld3,fld3,fld3 order by fld1;
t3.companynr+0	t2nr	fld3	sum(price)
37	1	Omaha	5987435
37	11401	breaking	5987435
37	11402	Romans	28357832
37	11403	intercepted	39654943
37	11501	bewilderingly	5987435
37	11701	astound	5987435
37	11702	admonishing	28357832
37	11703	sumac	39654943
37	12001	flanking	5987435
37	12003	combed	39654943
37	12301	Eulerian	5987435
37	12302	dubbed	28357832
37	12303	Kane	39654943
37	12501	annihilates	5987435
37	12602	Wotan	28357832
37	12603	snatching	39654943
37	12701	grazing	5987435
37	12702	Baird	28357832
37	12703	celery	39654943
37	13601	handgun	5987435
37	13602	foldout	28357832
37	13603	mystic	39654943
37	13801	intelligibility	5987435
37	13802	Augustine	28357832
37	13803	teethe	39654943
37	13901	scholastics	5987435
37	16001	audiology	5987435
37	16201	wallet	5987435
37	16202	parters	28357832
37	16301	eschew	5987435
37	16302	quitter	28357832
37	16303	neat	39654943
37	18001	jarring	5987435
37	18002	tinily	28357832
37	18003	balled	39654943
37	18012	impulsive	28357832
37	18013	starlet	39654943
37	18021	lawgiver	5987435
37	18022	stated	28357832
37	18023	readable	39654943
37	18032	testicle	28357832
37	18033	Parsifal	39654943
37	18041	Punjab	5987435
37	18042	Merritt	28357832
37	18043	Quixotism	39654943
37	18051	sureties	5987435
37	18052	puddings	28357832
37	18053	tapestry	39654943
37	18061	trimmings	5987435
37	18062	humility	28357832
37	18101	tragedies	5987435
37	18102	skulking	28357832
37	18103	flint	39654943
37	18201	relaxing	5987435
37	18202	offload	28357832
37	18402	suites	28357832
37	18403	lists	39654943
37	18601	vacuuming	5987435
37	18602	dentally	28357832
37	18603	humanness	39654943
37	18801	inch	5987435
37	18802	Weissmuller	28357832
37	18803	irresponsibly	39654943
37	18811	repetitions	5987435
37	18812	Antares	28357832
37	19101	ventilate	5987435
37	19102	pityingly	28357832
37	19103	interdependent	39654943
37	19201	Graves	5987435
37	30501	neonatal	5987435
37	30502	scribbled	28357832
37	30503	chafe	39654943
37	31901	realtor	5987435
37	36001	elite	5987435
37	36002	funereal	28357832
37	38001	Conley	5987435
37	38002	lectured	28357832
37	38003	Abraham	39654943
37	38011	groupings	5987435
37	38012	dissociate	28357832
37	38013	coexist	39654943
37	38101	rusting	5987435
37	38102	galling	28357832
37	38103	obliterates	39654943
37	38201	resumes	5987435
37	38202	analyzable	28357832
37	38203	terminator	39654943
select sum(price) from t3,t2 where t2.fld1 = t3.t2nr and t3.companynr = 512 and t3.t2nr = 38008 and t2.fld1 = 38008 or t2.fld1= t3.t2nr and t3.t2nr = 38008 and t2.fld1 = 38008;
sum(price)
234298
select t2.fld1,sum(price) from t3,t2 where t2.fld1 = t3.t2nr and t3.companynr = 512 and t3.t2nr = 38008 and t2.fld1 = 38008 or t2.fld1 = t3.t2nr and t3.t2nr = 38008 and t2.fld1 = 38008 or t3.t2nr = t2.fld1 and t2.fld1 = 38008 group by t2.fld1;
fld1	sum(price)
038008	234298
explain select fld3 from t2 where 1>2 or 2>3;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	NULL	NULL	NULL	NULL	NULL	NULL	NULL	Impossible WHERE
explain select fld3 from t2 where fld1=fld1;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1199	
select companynr,fld1 from t2 HAVING fld1=250501 or fld1=250502;
companynr	fld1
34	250501
34	250502
select companynr,fld1 from t2 WHERE fld1>=250501 HAVING fld1<=250502;
companynr	fld1
34	250501
34	250502
select companynr,count(*) as count,sum(fld1) as sum from t2 group by companynr having count > 40 and sum/count >= 120000;
companynr	count	sum
00	82	10355753
29	95	14473298
34	70	17788966
37	588	83602098
41	52	12816335
select companynr from t2 group by companynr having count(*) > 40 and sum(fld1)/count(*) >= 120000 ;
companynr
00
29
34
37
41
select t2.companynr,companyname,count(*) from t2,t4 where t2.companynr=t4.companynr group by companyname having t2.companynr >= 40;
companynr	companyname	count(*)
68	company 10	12
50	company 11	11
40	company 5	37
41	company 6	52
53	company 7	4
58	company 8	23
65	company 9	10
select count(*) from t2;
count(*)
1199
select count(*) from t2 where fld1 < 098024;
count(*)
387
select min(fld1) from t2 where fld1>= 098024;
min(fld1)
98024
select max(fld1) from t2 where fld1>= 098024;
max(fld1)
1232609
select count(*) from t3 where price2=76234234;
count(*)
4181
select count(*) from t3 where companynr=512 and price2=76234234;
count(*)
4181
explain select min(fld1),max(fld1),count(*) from t2;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	NULL	NULL	NULL	NULL	NULL	NULL	NULL	Select tables optimized away
select min(fld1),max(fld1),count(*) from t2;
min(fld1)	max(fld1)	count(*)
0	1232609	1199
select min(t2nr),max(t2nr) from t3 where t2nr=2115 and price2=823742;
min(t2nr)	max(t2nr)
2115	2115
select count(*),min(t2nr),max(t2nr) from t3 where name='spates' and companynr=78;
count(*)	min(t2nr)	max(t2nr)
4181	4	41804
select t2nr,count(*) from t3 where name='gems' group by t2nr limit 20;
t2nr	count(*)
9	1
19	1
29	1
39	1
49	1
59	1
69	1
79	1
89	1
99	1
109	1
119	1
129	1
139	1
149	1
159	1
169	1
179	1
189	1
199	1
select max(t2nr) from t3 where price=983543950;
max(t2nr)
41807
select t1.period from t3 = t1 limit 1;
period
1001
select t1.period from t1 as t1 limit 1;
period
9410
select t1.period as "Nuvarande period" from t1 as t1 limit 1;
Nuvarande period
9410
select period as ok_period from t1 limit 1;
ok_period
9410
select period as ok_period from t1 group by ok_period limit 1;
ok_period
9410
select 1+1 as summa from t1 group by summa limit 1;
summa
2
select period as "Nuvarande period" from t1 group by "Nuvarande period" limit 1;
Nuvarande period
9410
show tables;
Tables_in_test
t1
t2
t3
t4
show tables from test like "s%";
Tables_in_test (s%)
show tables from test like "t?";
Tables_in_test (t?)
show full columns from t2;
Field	Type	Collation	Null	Key	Default	Extra	Privileges	Comment
auto	int(11)	NULL		PRI	NULL	auto_increment		
fld1	int(6) unsigned zerofill	NULL		UNI	000000			
companynr	tinyint(2) unsigned zerofill	NULL			00			
fld3	char(30)	latin1_swedish_ci		MUL				
fld4	char(35)	latin1_swedish_ci						
fld5	char(35)	latin1_swedish_ci						
fld6	char(4)	latin1_swedish_ci						
show full columns from t2 from test like 'f%';
Field	Type	Collation	Null	Key	Default	Extra	Privileges	Comment
fld1	int(6) unsigned zerofill	NULL		UNI	000000			
fld3	char(30)	latin1_swedish_ci		MUL				
fld4	char(35)	latin1_swedish_ci						
fld5	char(35)	latin1_swedish_ci						
fld6	char(4)	latin1_swedish_ci						
show full columns from t2 from test like 's%';
Field	Type	Collation	Null	Key	Default	Extra	Privileges	Comment
show keys from t2;
Table	Non_unique	Key_name	Seq_in_index	Column_name	Collation	Cardinality	Sub_part	Packed	Null	Index_type	Comment
t2	0	PRIMARY	1	auto	A	1199	NULL	NULL		BTREE	
t2	0	fld1	1	fld1	A	1199	NULL	NULL		BTREE	
t2	1	fld3	1	fld3	A	NULL	NULL	NULL		BTREE	
drop table t4, t3, t2, t1;
DO 1;
DO benchmark(100,1+1),1,1;
CREATE TABLE t1 (
id mediumint(8) unsigned NOT NULL auto_increment,
pseudo varchar(35) NOT NULL default '',
PRIMARY KEY  (id),
UNIQUE KEY pseudo (pseudo)
);
INSERT INTO t1 (pseudo) VALUES ('test');
INSERT INTO t1 (pseudo) VALUES ('test1');
SELECT 1 as rnd1 from t1 where rand() > 2;
rnd1
DROP TABLE t1;
CREATE TABLE t1 (gvid int(10) unsigned default NULL,  hmid int(10) unsigned default NULL,  volid int(10) unsigned default NULL,  mmid int(10) unsigned default NULL,  hdid int(10) unsigned default NULL,  fsid int(10) unsigned default NULL,  ctid int(10) unsigned default NULL,  dtid int(10) unsigned default NULL,  cost int(10) unsigned default NULL,  performance int(10) unsigned default NULL,  serialnumber bigint(20) unsigned default NULL,  monitored tinyint(3) unsigned default '1',  removed tinyint(3) unsigned default '0',  target tinyint(3) unsigned default '0',  dt_modified timestamp(14) NOT NULL,  name varchar(255) binary default NULL,  description varchar(255) default NULL,  UNIQUE KEY hmid (hmid,volid)) ENGINE=MyISAM;
INSERT INTO t1 VALUES (200001,2,1,1,100,1,1,1,0,0,0,1,0,1,20020425060057,'\\\\ARKIVIO-TESTPDC\\E$',''),(200002,2,2,1,101,1,1,1,0,0,0,1,0,1,20020425060057,'\\\\ARKIVIO-TESTPDC\\C$',''),(200003,1,3,2,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1,0,1,20020425060427,'c:',NULL);
CREATE TABLE t2 (  hmid int(10) unsigned default NULL,  volid int(10) unsigned default NULL,  sampletid smallint(5) unsigned default NULL,  sampletime datetime default NULL,  samplevalue bigint(20) unsigned default NULL,  KEY idx1 (hmid,volid,sampletid,sampletime)) ENGINE=MyISAM;
INSERT INTO t2 VALUES (1,3,10,'2002-06-01 08:00:00',35),(1,3,1010,'2002-06-01 12:00:01',35);
SELECT a.gvid, (SUM(CASE b.sampletid WHEN 140 THEN b.samplevalue ELSE 0 END)) as the_success,(SUM(CASE b.sampletid WHEN 141 THEN b.samplevalue ELSE 0 END)) as the_fail,(SUM(CASE b.sampletid WHEN 142 THEN b.samplevalue ELSE 0 END)) as the_size,(SUM(CASE b.sampletid WHEN 143 THEN b.samplevalue ELSE 0 END)) as the_time FROM t1 a, t2 b WHERE a.hmid = b.hmid AND a.volid = b.volid AND b.sampletime >= 'wrong-date-value' AND b.sampletime < 'wrong-date-value' AND b.sampletid IN (140, 141, 142, 143) GROUP BY a.gvid;
gvid	the_success	the_fail	the_size	the_time
Warnings:
Warning	1292	Truncated incorrect datetime value: 'wrong-date-value'
Warning	1292	Truncated incorrect datetime value: 'wrong-date-value'
Warning	1292	Truncated incorrect datetime value: 'wrong-date-value'
Warning	1292	Truncated incorrect datetime value: 'wrong-date-value'
SELECT a.gvid, (SUM(CASE b.sampletid WHEN 140 THEN b.samplevalue ELSE 0 END)) as the_success,(SUM(CASE b.sampletid WHEN 141 THEN b.samplevalue ELSE 0 END)) as the_fail,(SUM(CASE b.sampletid WHEN 142 THEN b.samplevalue ELSE 0 END)) as the_size,(SUM(CASE b.sampletid WHEN 143 THEN b.samplevalue ELSE 0 END)) as the_time FROM t1 a, t2 b WHERE a.hmid = b.hmid AND a.volid = b.volid AND b.sampletime >= NULL AND b.sampletime < NULL AND b.sampletid IN (140, 141, 142, 143) GROUP BY a.gvid;
gvid	the_success	the_fail	the_size	the_time
DROP TABLE t1,t2;
create table  t1 (  A_Id bigint(20) NOT NULL default '0',  A_UpdateBy char(10) NOT NULL default '',  A_UpdateDate bigint(20) NOT NULL default '0',  A_UpdateSerial int(11) NOT NULL default '0',  other_types bigint(20) NOT NULL default '0',  wss_type bigint(20) NOT NULL default '0');
INSERT INTO t1 VALUES (102935998719055004,'brade',1029359987,2,102935229116544068,102935229216544093);
select wss_type from t1 where wss_type ='102935229216544106';
wss_type
select wss_type from t1 where wss_type ='102935229216544105';
wss_type
select wss_type from t1 where wss_type ='102935229216544104';
wss_type
select wss_type from t1 where wss_type ='102935229216544093';
wss_type
102935229216544093
select wss_type from t1 where wss_type =102935229216544093;
wss_type
102935229216544093
drop table t1;
select 1+2,"aaaa",3.13*2.0 into @a,@b,@c;
select @a;
@a
3
select @b;
@b
aaaa
select @c;
@c
6.26
create table t1 (a int not null auto_increment primary key);
insert into t1 values ();
insert into t1 values ();
insert into t1 values ();
select * from (t1 as t2 left join t1 as t3 using (a)), t1;
a	a	a
1	1	1
2	2	1
3	3	1
1	1	2
2	2	2
3	3	2
1	1	3
2	2	3
3	3	3
select * from t1, (t1 as t2 left join t1 as t3 using (a));
a	a	a
1	1	1
2	1	1
3	1	1
1	2	2
2	2	2
3	2	2
1	3	3
2	3	3
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) straight_join t1;
a	a	a
1	1	1
2	2	1
3	3	1
1	1	2
2	2	2
3	3	2
1	1	3
2	2	3
3	3	3
select * from t1 straight_join (t1 as t2 left join t1 as t3 using (a));
a	a	a
1	1	1
2	1	1
3	1	1
1	2	2
2	2	2
3	2	2
1	3	3
2	3	3
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) inner join t1 on t1.a>1;
a	a	a
1	1	2
1	1	3
2	2	2
2	2	3
3	3	2
3	3	3
select * from t1 inner join (t1 as t2 left join t1 as t3 using (a)) on t1.a>1;
a	a	a
1	1	NULL
2	1	1
3	1	1
1	2	NULL
2	2	2
3	2	2
1	3	NULL
2	3	3
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) inner join t1 using ( a );
a	a	a
1	1	1
2	2	2
3	3	3
select * from t1 inner join (t1 as t2 left join t1 as t3 using (a)) using ( a );
a	a	a
1	1	1
2	1	NULL
3	1	NULL
1	2	NULL
2	2	2
3	2	NULL
1	3	NULL
2	3	NULL
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) left outer join t1 on t1.a>1;
a	a	a
1	1	2
1	1	3
2	2	2
2	2	3
3	3	2
3	3	3
select * from t1 left outer join (t1 as t2 left join t1 as t3 using (a)) on t1.a>1;
a	a	a
1	1	NULL
2	1	1
3	1	1
1	2	NULL
2	2	2
3	2	2
1	3	NULL
2	3	3
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) left join t1 using ( a );
a	a	a
1	1	1
2	2	2
3	3	3
select * from t1 left join (t1 as t2 left join t1 as t3 using (a)) using ( a );
a	a	a
1	1	1
2	1	NULL
3	1	NULL
1	2	NULL
2	2	2
3	2	NULL
1	3	NULL
2	3	NULL
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) natural left join t1;
a	a	a
1	1	1
2	2	2
3	3	3
select * from t1 natural left join (t1 as t2 left join t1 as t3 using (a));
a	a	a
1	1	1
2	2	2
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) right join t1 on t1.a>1;
a	a	a
1	NULL	1
2	NULL	1
3	NULL	1
1	1	2
2	2	2
3	3	2
1	1	3
2	2	3
3	3	3
select * from t1 right join (t1 as t2 left join t1 as t3 using (a)) on t1.a>1;
a	a	a
2	1	1
3	1	1
2	2	2
3	2	2
2	3	3
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) right outer join t1 using ( a );
a	a	a
1	1	1
2	NULL	1
3	NULL	1
1	NULL	2
2	2	2
3	NULL	2
1	NULL	3
2	NULL	3
3	3	3
select * from t1 right outer join (t1 as t2 left join t1 as t3 using (a)) using ( a );
a	a	a
1	1	1
2	2	2
3	3	3
select * from (t1 as t2 left join t1 as t3 using (a)) natural right join t1;
a	a	a
1	1	1
2	NULL	1
3	NULL	1
1	NULL	2
2	2	2
3	NULL	2
1	NULL	3
2	NULL	3
3	3	3
select * from t1 natural right join (t1 as t2 left join t1 as t3 using (a));
a	a	a
1	1	1
2	2	2
3	3	3
select * from t1 natural join (t1 as t2 left join t1 as t3 using (a));
a	a
1	1
2	2
3	3
select * from (t1 as t2 left join t1 as t3 using (a)) natural join t1;
a	a	a
1	1	1
2	2	2
3	3	3
drop table t1;
CREATE TABLE t1 (  aa char(2),  id int(11) NOT NULL auto_increment,  t2_id int(11) NOT NULL default '0',  PRIMARY KEY  (id),  KEY replace_id (t2_id)) ENGINE=MyISAM;
INSERT INTO t1 VALUES ("1",8264,2506),("2",8299,2517),("3",8301,2518),("4",8302,2519),("5",8303,2520),("6",8304,2521),("7",8305,2522);
CREATE TABLE t2 ( id int(11) NOT NULL auto_increment,  PRIMARY KEY  (id)) ENGINE=MyISAM;
INSERT INTO t2 VALUES (2517), (2518), (2519), (2520), (2521), (2522);
select * from t1, t2 WHERE t1.t2_id = t2.id and t1.t2_id > 0   order by t1.id   LIMIT 0, 5;
aa	id	t2_id	id
2	8299	2517	2517
3	8301	2518	2518
4	8302	2519	2519
5	8303	2520	2520
6	8304	2521	2521
drop table t1,t2;
create table t1 (id1 int NOT NULL);
create table t2 (id2 int NOT NULL);
create table t3 (id3 int NOT NULL);
create table t4 (id4 int NOT NULL, id44 int NOT NULL, KEY (id4));
insert into t1 values (1);
insert into t1 values (2);
insert into t2 values (1);
insert into t4 values (1,1);
explain select * from t1 left join t2 on id1 = id2 left join t3 on id1 = id3
left join t4 on id3 = id4 where id2 = 1 or id4 = 1;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t3	system	NULL	NULL	NULL	NULL	0	const row not found
1	SIMPLE	t1	ALL	NULL	NULL	NULL	NULL	2	
1	SIMPLE	t2	ALL	NULL	NULL	NULL	NULL	1	
1	SIMPLE	t4	ALL	id4	NULL	NULL	NULL	1	Using where
select * from t1 left join t2 on id1 = id2 left join t3 on id1 = id3
left join t4 on id3 = id4 where id2 = 1 or id4 = 1;
id1	id2	id3	id4	id44
1	1	NULL	NULL	NULL
drop table t1,t2,t3,t4;
create table t1(s varchar(10) not null);
create table t2(s varchar(10) not null primary key);
create table t3(s varchar(10) not null primary key);
insert into t1 values ('one\t'), ('two\t');
insert into t2 values ('one\r'), ('two\t');
insert into t3 values ('one '), ('two\t');
select * from t1 where s = 'one';
s
select * from t2 where s = 'one';
s
select * from t3 where s = 'one';
s
one
select * from t1,t2 where t1.s = t2.s;
s	s
two		two	
select * from t2,t3 where t2.s = t3.s;
s	s
two		two	
drop table t1, t2, t3;
CREATE TABLE t1 (
i int(11) NOT NULL default '0',
c char(10) NOT NULL default '',
PRIMARY KEY  (i),
UNIQUE KEY c (c)
) ENGINE=MyISAM;
INSERT INTO t1 VALUES (1,'a');
INSERT INTO t1 VALUES (2,'b');
INSERT INTO t1 VALUES (3,'c');
EXPLAIN SELECT i FROM t1 WHERE i=1;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t1	const	PRIMARY	PRIMARY	4	const	1	Using index
EXPLAIN SELECT i FROM t1 WHERE i=1;
id	select_type	table	type	possible_keys	key	key_len	ref	rows	Extra
1	SIMPLE	t1	const	PRIMARY	PRIMARY	4	const	1	Using index
DROP TABLE t1;
