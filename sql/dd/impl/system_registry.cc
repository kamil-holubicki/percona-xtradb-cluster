/* Copyright (c) 2014, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#include "dd/impl/system_registry.h"

#include "dd/impl/system_views/character_sets.h"     // Character_sets
#include "dd/impl/system_views/collation_charset_applicability.h" // Collati...
#include "dd/impl/system_views/collations.h"         // Collations
#include "dd/impl/system_views/column_statistics.h"  // Column_statistics
#include "dd/impl/system_views/columns.h"            // Columns
#include "dd/impl/system_views/events.h"             // Events
#include "dd/impl/system_views/innodb_datafiles.h"   // Innodb_datafiles
#include "dd/impl/system_views/innodb_foreign.h"     // Innodb_foreign
#include "dd/impl/system_views/innodb_foreign_cols.h"// Innodb_foreign_cols
#include "dd/impl/system_views/innodb_fields.h"      // Innodb_fields
#include "dd/impl/system_views/innodb_tablespaces_brief.h"   // Innodb_tablespace_brief
#include "dd/impl/system_views/key_column_usage.h"   // key_column_usage
#include "dd/impl/system_views/parameters.h"         // Parameters
#include "dd/impl/system_views/partitions.h"         // Partitions
#include "dd/impl/system_views/routines.h"           // Routines
#include "dd/impl/system_views/schemata.h"           // Schemata
#include "dd/impl/system_views/st_geometry_columns.h"// st_geometry_columns
#include "dd/impl/system_views/st_spatial_reference_systems.h" // St_spatial...
#include "dd/impl/system_views/statistics.h"         // Statistics
#include "dd/impl/system_views/table_constraints.h"  // Table_constraints
#include "dd/impl/system_views/tables.h"             // Tables
#include "dd/impl/system_views/triggers.h"           // Triggers
#include "dd/impl/system_views/views.h"              // Views

#include "dd/impl/tables/catalogs.h"                 // Catalog
#include "dd/impl/tables/character_sets.h"           // Character_sets
#include "dd/impl/tables/collations.h"               // Collations
#include "dd/impl/tables/column_statistics.h"        // Column_statistics
#include "dd/impl/tables/column_type_elements.h"     // Column_type_elements
#include "dd/impl/tables/columns.h"                  // Columns
#include "dd/impl/tables/dd_properties.h"            // DD_properties
#include "dd/impl/tables/events.h"                   // Events
#include "dd/impl/tables/foreign_key_column_usage.h" // Foreign_key_column_usage
#include "dd/impl/tables/foreign_keys.h"             // Foreign_keys
#include "dd/impl/tables/index_column_usage.h"       // Index_column_usage
#include "dd/impl/tables/index_partitions.h"         // Index_partitions
#include "dd/impl/tables/index_stats.h"              // Index_stats
#include "dd/impl/tables/indexes.h"                  // Indexes
#include "dd/impl/tables/parameter_type_elements.h"  // Parameter_type_elements
#include "dd/impl/tables/parameters.h"               // Parameters
#include "dd/impl/tables/routines.h"                 // Routines
#include "dd/impl/tables/schemata.h"                 // Schemata
#include "dd/impl/tables/spatial_reference_systems.h"// Spatial_reference_systems
#include "dd/impl/tables/table_partition_values.h"   // Table_partition_values
#include "dd/impl/tables/table_partitions.h"         // Table_partitions
#include "dd/impl/tables/table_stats.h"              // Table_stats
#include "dd/impl/tables/tables.h"                   // Tables
#include "dd/impl/tables/tablespace_files.h"         // Tablespace_files
#include "dd/impl/tables/tablespaces.h"              // Tablespaces
#include "dd/impl/tables/triggers.h"                 // Triggers
#include "dd/impl/tables/view_routine_usage.h"       // View_routine_usage
#include "dd/impl/tables/view_table_usage.h"         // View_table_usage
#include "lex_string.h"
#include "sql/table.h"                               // MYSQL_SYSTEM_SCHEMA

using namespace dd::tables;

///////////////////////////////////////////////////////////////////////////

namespace {
template <typename X>
void register_table(dd::System_tables::Types type)
{
  dd::System_tables::instance()->add(MYSQL_SCHEMA_NAME.str,
                                     X::instance().table_name(),
                                     type, &X::instance());
}

template <typename X>
void register_view(dd::System_views::Types type)
{
  DBUG_EXECUTE_IF("test_i_s_metadata_version",
                  {
                    if (X::view_name() == "EVENTS")
                      return;
                  });

  dd::System_views::instance()->add(INFORMATION_SCHEMA_NAME.str,
                                    X::instance().name(),
                                    type, &X::instance());
}
}

namespace dd {

System_tables * System_tables::instance()
{
  static System_tables s_instance;
  return &s_instance;
}

System_views * System_views::instance()
{
  static System_views s_instance;
  return &s_instance;
}

System_tablespaces * System_tablespaces::instance()
{
  static System_tablespaces s_instance;
  return &s_instance;
}


void System_tables::init()
{
  // Se header file for explanation of table categories.
  dd::System_tables::Types inert=  dd::System_tables::Types::INERT;
  dd::System_tables::Types core=   dd::System_tables::Types::CORE;
  dd::System_tables::Types second= dd::System_tables::Types::SECOND;

  // Order below is dictated by the foreign key constraints.
  register_table<DD_properties>(inert);

  register_table<Character_sets>(core);
  register_table<Collations>(core);
  register_table<Tablespaces>(core);
  register_table<Tablespace_files>(core);
  register_table<Catalogs>(core);
  register_table<dd::tables::Column_statistics>(core);
  register_table<Schemata>(core);
  register_table<Spatial_reference_systems>(second);
  register_table<Tables>(core);
  register_table<View_table_usage>(core);
  register_table<View_routine_usage>(core);
  register_table<Columns>(core);
  register_table<Indexes>(core);
  register_table<Index_column_usage>(core);
  register_table<Column_type_elements>(core);
  register_table<Foreign_keys>(core);
  register_table<Foreign_key_column_usage>(core);
  register_table<Table_partitions>(core);
  register_table<Table_partition_values>(core);
  register_table<Index_partitions>(core);

  register_table<Table_stats>(second);
  register_table<Index_stats>(second);
  register_table<Events>(second);
  register_table<Routines>(second);
  register_table<Parameters>(second);
  register_table<Parameter_type_elements>(second);
  register_table<Triggers>(core);
}

void System_views::init()
{
  // Register system views with the server.
  dd::System_views::Types is=  dd::System_views::Types::INFORMATION_SCHEMA;

  register_view<dd::system_views::Character_sets>(is);
  register_view<dd::system_views::Collations>(is);
  register_view<dd::system_views::Collation_charset_applicability>(is);
  register_view<dd::system_views::Columns>(is);
  register_view<dd::system_views::Column_statistics>(is);
  register_view<dd::system_views::Events>(is);
  register_view<dd::system_views::Innodb_datafiles>(is);
  register_view<dd::system_views::Innodb_foreign>(is);
  register_view<dd::system_views::Innodb_foreign_cols>(is);
  register_view<dd::system_views::Innodb_fields>(is);
  register_view<dd::system_views::Innodb_tablespaces_brief>(is);
  register_view<dd::system_views::Key_column_usage>(is);
  register_view<dd::system_views::Parameters>(is);
  register_view<dd::system_views::Routines>(is);
  register_view<dd::system_views::Schemata>(is);
  register_view<dd::system_views::Show_statistics>(is);
  register_view<dd::system_views::Show_statistics_dynamic>(is);
  register_view<dd::system_views::St_spatial_reference_systems>(is);
  register_view<dd::system_views::St_geometry_columns>(is);
  register_view<dd::system_views::Statistics>(is);
  register_view<dd::system_views::Statistics_dynamic>(is);
  register_view<dd::system_views::Table_constraints>(is);
  register_view<dd::system_views::Tables>(is);
  register_view<dd::system_views::Tables_dynamic>(is);
  register_view<dd::system_views::Triggers>(is);
  register_view<dd::system_views::Views>(is);
  register_view<dd::system_views::Partitions>(is);
}

} // namespace dd

