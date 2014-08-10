/*
   Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "client_priv.h"
#include "abstract_program.h"
#include "my_default.h"

using namespace Mysql::Tools::Base;

extern const char *load_default_groups[];

my_bool Abstract_program::callback_option_parsed(
  int optid,
  const struct my_option *opt __attribute__((unused)),
  char *argument)
{
  // Check if option uses My::Tools::Base::Options, and it should.
  Options::I_option* app_type = (Options::I_option*)opt->app_type;
  Options::I_option* option = dynamic_cast<Options::I_option*>(app_type);
  if (option != NULL)
  {
    option->call_callbacks(argument);
  }
  return 0;
}


const string Abstract_program::get_name()
{
  return this->m_name;
}

my_option* Abstract_program::get_options_array()
{
  return &this->m_options[0];
}

Abstract_program::Abstract_program()
  : m_debug_options(this),
  m_help_options(this),
  m_defaults_argv(NULL)
{
  this->add_providers(&this->m_help_options, &this->m_debug_options, NULL);
}

void Abstract_program::run(int argc, char **argv)
{
  vector<Options::I_options_provider*>::iterator it;
  this->init_name(argv[0]);

  MY_INIT(this->m_name.c_str());

  this->aggregate_options();

  my_getopt_use_args_separator= TRUE;
  if (load_defaults("my",load_default_groups,&argc,&argv))
    this->error(1);
  my_getopt_use_args_separator= FALSE;

  this->m_defaults_argv= argv;

  int ho_error= handle_options(&argc, &argv, this->get_options_array(),
    Abstract_program::callback_option_parsed);
  if (ho_error != 0)
  {
    this->error(ho_error);
  }

  // Let providers handle their parsed options.
  this->options_parsed();

  vector<string> positional_options;
  for (; argc > 0; argc--, argv++)
  {
    positional_options.push_back(*argv);
  }

  // Execute main body of program.
  int result= this->execute(positional_options);

  exit(result);
}

Abstract_program::~Abstract_program()
{
  if (this->m_defaults_argv)
  {
    free_defaults(this->m_defaults_argv);
  }
}

void Abstract_program::init_name(char *name_from_cmd_line)
{
#if _WIN32
  char* name;

  char name_buf[FN_REFLEN];
  if (GetModuleFileName(NULL, name_buf, FN_REFLEN) != 0)
  {
    name= name_buf;
  }
  else
  {
    name= name_from_cmd_line;
  }

  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  _splitpath_s( name, drive, dir, fname, ext);

  this->m_name= fname;
#else
  string name= name_from_cmd_line;
  this->m_name= name.substr( name.find_last_of( '/' ) +1 );
#endif
}

void Abstract_program::aggregate_options()
{
  // Concatenate all available command line options.
  this->m_options.clear();

  this->m_options= this->generate_options();

  // Adding sentinel, handle_options assume input as array with sentinel.
  my_option sentinel=
    {0, 0, 0, 0, 0, 0, GET_NO_ARG, NO_ARG, 0, 0, 0, 0, 0, 0};
  this->m_options.push_back(sentinel);
}
