/*
   Copyright (c) 2010, 2023, Oracle and/or its affiliates.
   Use is subject to license terms.

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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

package testsuite.clusterj.model;

import com.mysql.clusterj.annotation.Index;
import com.mysql.clusterj.annotation.NotPersistent;
import com.mysql.clusterj.annotation.PersistenceCapable;
import com.mysql.clusterj.annotation.PrimaryKey;
import java.util.Collection;

/** This interface is based on Employee but adds not-persistent properties.
 */
@PersistenceCapable(table="t_basic")
public interface NotPersistentTypes extends IdBase {
    
    @PrimaryKey
    int getId();
    void setId(int id);

    String getName();
    void setName(String name);

    @Index(name="idx_unique_hash_magic")
    int getMagic();
    void setMagic(int magic);

    @Index(name="idx_btree_age")
    Integer getAge();
    void setAge(Integer age);

    @NotPersistent
    Collection<NotPersistentTypes> getNotPersistentChildren();
    void setNotPersistentChildren(Collection<NotPersistentTypes> value);

    @NotPersistent
    int getNotPersistentInt();
    void setNotPersistentInt(int value);

    @NotPersistent
    Integer getNotPersistentInteger();
    void setNotPersistentInteger(Integer value);

}
