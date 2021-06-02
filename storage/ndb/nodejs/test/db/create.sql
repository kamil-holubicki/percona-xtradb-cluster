-- Copyright (c) 2014, 2020, Oracle and/or its affiliates.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
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
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

use test;
drop table if exists db_new_schema1;
drop table if exists db_new_schema2;
drop table if exists db_new_schema3;
drop table if exists db_freeform;
create table if not exists db_freeform (
  id int not null AUTO_INCREMENT,
  SPARSE_FIELDS varchar(1000),
  primary key(id)
);

insert into db_freeform(id, SPARSE_FIELDS) values(1, '{"name": "Name 1", "number": 1, "a": [{"a10": "a10"}, {"a11": "a11"}]}');
insert into db_freeform(id, SPARSE_FIELDS) values(2, '{"name": "Name 2", "number": 2, "a": [{"a20": "a20"}, {"a21": "a21"}]}');

drop table if exists db_semistruct;
create table if not exists db_semistruct (
  id int not null AUTO_INCREMENT,
  name varchar(30),
  number int,
  SPARSE_FIELDS varchar(1000),
  primary key(id)
);

insert into db_semistruct(id, name, number, SPARSE_FIELDS) values(1, "Name 1", 1, '{"a": [{"a10": "a10"}, {"a11": "a11"}]}');
insert into db_semistruct(id, name, number, SPARSE_FIELDS) values(2, "Name 2", 2, '{"a": [{"a20": "a20"}, {"a21": "a21"}]}');

drop table if exists db_basic;
create table if not exists db_basic (
  id int not null,
  name varchar(32) default 'Employee 666',
  age int,
  magic int not null,
  primary key(id),

  unique key idx_unique_hash_magic (magic) using hash,
  key idx_btree_age (age)
);
insert into db_basic values(0, 'Employee 0', 0, 0);
insert into db_basic values(1, 'Employee 1', 1, 1);
insert into db_basic values(2, 'Employee 2', 2, 2);
insert into db_basic values(3, 'Employee 3', 3, 3);
insert into db_basic values(4, 'Employee 4', 4, 4);
insert into db_basic values(5, 'Employee 5', 5, 5);
insert into db_basic values(6, 'Employee 6', 6, 6);
insert into db_basic values(7, 'Employee 7', 7, 7);
insert into db_basic values(8, 'Employee 8', 8, 8);
insert into db_basic values(9, 'Employee 9', 9, 9);
