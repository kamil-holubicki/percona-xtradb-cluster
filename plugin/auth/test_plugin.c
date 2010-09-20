/*  Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; version 2 of the
    License.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA */

/**
  @file

  dialog client authentication plugin with examples

  dialog is a general purpose client authentication plugin, it simply
  asks the user the question, as provided by the server and reports
  the answer back to the server. No encryption is involved,
  the answers are sent in clear text.

  Two examples are provided: two_questions server plugin, that asks
  the password and an "Are you sure?" question with a reply "yes, of course".
  It demonstrates the usage of "password" (input is hidden) and "ordinary"
  (input can be echoed) questions, and how to mark the last question,
  to avoid an extra roundtrip.

  And three_attempts plugin that gives the user three attempts to enter
  a correct password. It shows the situation when a number of questions
  is not known in advance.
*/

#include <my_global.h>
#include <mysql/plugin_auth.h>
#include <mysql/client_plugin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
  first byte of the question string is the question "type".
  It can be a "ordinary" or a "password" question.
  The last bit set marks a last question in the authentication exchange.
*/
#define ORDINARY_QUESTION       "\2"
#define LAST_QUESTION           "\3"
#define LAST_PASSWORD           "\4"
#define PASSWORD_QUESTION       "\5"

/********************* SERVER SIDE ****************************************/

/**
  dialog test plugin mimicing the ordinary auth mechanism. Used to test the auth plugin API
*/
static int auth_test_plugin(MYSQL_PLUGIN_VIO *vio, MYSQL_SERVER_AUTH_INFO *info)
{
  unsigned char *pkt;
  int pkt_len;

  /* send a password question */
  if (vio->write_packet(vio, (const unsigned char *) PASSWORD_QUESTION, 1))
    return CR_ERROR;

  /* read the answer */
  if ((pkt_len= vio->read_packet(vio, &pkt)) < 0)
    return CR_ERROR;

  info->password_used= PASSWORD_USED_YES;

  /* fail if the password is wrong */
  if (strcmp((const char *) pkt, info->auth_string))
    return CR_ERROR;

  /* copy auth string as a destination name to check it */
  strcpy (info->authenticated_as, info->auth_string);

  /* copy something into the external user name */
  strcpy (info->external_user, info->auth_string);

  return CR_OK;
}

static struct st_mysql_auth auth_test_handler=
{
  MYSQL_AUTHENTICATION_INTERFACE_VERSION,
  "auth_test_plugin", /* requires test_plugin client's plugin */
  auth_test_plugin
};

mysql_declare_plugin(test_plugin)
{
  MYSQL_AUTHENTICATION_PLUGIN,
  &auth_test_handler,
  "test_plugin_server",
  "Georgi Kodinov",
  "plugin API test plugin",
  PLUGIN_LICENSE_GPL,
  NULL,
  NULL,
  0x0100,
  NULL,
  NULL,
  NULL
}
mysql_declare_plugin_end;

/********************* CLIENT SIDE ***************************************/
/*
  client plugin used for testing the plugin API
*/
#include <mysql.h>

/**
  The main function of the test plugin.

  Reads the prompt, check if the handshake is done and if the prompt is a
  password request and returns the password. Otherwise return error.

  @note
   1. this plugin shows how a client authentication plugin
      may read a MySQL protocol OK packet internally - which is important
      where a number of packets is not known in advance.
   2. the first byte of the prompt is special. it is not
      shown to the user, but signals whether it is the last question
      (prompt[0] & 1 == 1) or not last (prompt[0] & 1 == 0),
      and whether the input is a password (not echoed).
   3. the prompt is expected to be sent zero-terminated
*/
static int test_plugin_client(MYSQL_PLUGIN_VIO *vio, MYSQL *mysql)
{
  unsigned char *pkt, cmd= 0;
  int pkt_len, res;
  char *reply;

  do
  {
    /* read the prompt */
    pkt_len= vio->read_packet(vio, &pkt);
    if (pkt_len < 0)
      return CR_ERROR;

    if (pkt == 0)
    {
      /*
        in mysql_change_user() the client sends the first packet, so
        the first vio->read_packet() does nothing (pkt == 0).

        We send the "password", assuming the client knows what its doing.
        (in other words, the dialog plugin should be only set as a default
        authentication plugin on the client if the first question
        asks for a password - which will be sent in cleat text, by the way)
      */
      reply= mysql->passwd;
    }
    else
    {
      cmd= *pkt++;

      /* is it MySQL protocol (0=OK or 254=need old password) packet ? */
      if (cmd == 0 || cmd == 254)
        return CR_OK_HANDSHAKE_COMPLETE; /* yes. we're done */

      /*
        asking for a password with an empty prompt means mysql->password
        otherwise return an error
      */
      if ((cmd == LAST_PASSWORD[0] || cmd == PASSWORD_QUESTION[0]) && *pkt == 0)
        reply= mysql->passwd;
      else
        return CR_ERROR;
    }
    if (!reply)
      return CR_ERROR;
    /* send the reply to the server */
    res= vio->write_packet(vio, (const unsigned char *) reply, 
                           strlen(reply) + 1);

    if (res)
      return CR_ERROR;

    /* repeat unless it was the last question */
  } while (cmd != LAST_QUESTION[0] && cmd != PASSWORD_QUESTION[0]);

  /* the job of reading the ok/error packet is left to the server */
  return CR_OK;
}


mysql_declare_client_plugin(AUTHENTICATION)
  "auth_test_plugin",
  "Georgi Kodinov",
  "Dialog Client Authentication Plugin",
  {0,1,0},
  NULL,
  NULL,
  test_plugin_client
mysql_end_client_plugin;
