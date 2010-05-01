/*
 * IRC - Internet Relay Chat, ircd/m_pseudo.c
 * Copyright (C) 2002 Zoot <zoot@gamesnet.net>
 *
 * See file AUTHORS in IRC package for additional names of
 * the programmers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id$
 */

/*
 * m_functions execute protocol messages on this server:
 *
 *    cptr    is always NON-NULL, pointing to a *LOCAL* client
 *            structure (with an open socket connected!). This
 *            identifies the physical socket where the message
 *            originated (or which caused the m_function to be
 *            executed--some m_functions may call others...).
 *
 *    sptr    is the source of the message, defined by the
 *            prefix part of the message if present. If not
 *            or prefix not found, then sptr==cptr.
 *
 *            (!IsServer(cptr)) => (cptr == sptr), because
 *            prefixes are taken *only* from servers...
 *
 *            (IsServer(cptr))
 *                    (sptr == cptr) => the message didn't
 *                    have the prefix.
 *
 *                    (sptr != cptr && IsServer(sptr) means
 *                    the prefix specified servername. (?)
 *
 *                    (sptr != cptr && !IsServer(sptr) means
 *                    that message originated from a remote
 *                    user (not local).
 *
 *            combining
 *
 *            (!IsServer(sptr)) means that, sptr can safely
 *            taken as defining the target structure of the
 *            message in this server.
 *
 *    *Always* true (if 'parse' and others are working correct):
 *
 *    1)      sptr->from == cptr  (note: cptr->from == cptr)
 *
 *    2)      MyConnect(sptr) <=> sptr == cptr (e.g. sptr
 *            *cannot* be a local connection, unless it's
 *            actually cptr!). [MyConnect(x) should probably
 *            be defined as (x == x->from) --msa ]
 *
 *    parc    number of variable parameter strings (if zero,
 *            parv is allowed to be NULL)
 *
 *    parv    a NULL terminated list of parameter pointers,
 *
 *                    parv[0], sender (prefix string), if not present
 *                            this points to an empty string.
 *                    parv[1], pointer to "extra" data (service mapping)
 *                    parv[2]...parv[parc-1]
 *                            pointers to additional parameters
 *                    parv[parc] == NULL, *always*
 *
 *            note:   it is guaranteed that parv[0]..parv[parc-1] are all
 *                    non-NULL pointers.
 */
#include "config.h"

#include "client.h"
#include "hash.h"
#include "ircd.h"
#include "ircd_features.h"
#include "ircd_reply.h"
#include "ircd_string.h"
#include "msg.h"
#include "numeric.h"
#include "numnicks.h"
#include "s_conf.h"
#include "s_user.h"

#include <assert.h>

/*
 * m_pseudo - generic service message handler
 *
 * parv[0] = sender prefix
 * parv[1] = service mapping (s_map * disguised as char *)
 * parv[2] = message
 */
int m_pseudo(struct Client* cptr, struct Client* sptr, int parc, char* parv[])
{
  struct s_map *map;
  unsigned int i;

  assert(0 != cptr);
  assert(cptr == sptr);
  assert(0 != cli_user(sptr));

  if (parc < 3 || EmptyString(parv[parc - 1]))
    return send_reply(sptr, ERR_NOTEXTTOSEND);

  /* HACK! HACK! HACK! HACK! Yes. It's icky, but
   * it's the only way. */
  map = (struct s_map *)parv[1];
  assert(0 != map);

  for (i = 0; i < map->count; i++) {
      struct Client *target, *server;
      char *host, *nick;

      nick = map->services[i].nick;
      host = map->services[i].host;

      if (0 == (server = FindServer(host + 1))) {
          continue;
      }

      /* Sigh. We emulate some parts of relay_directed_message
       * here so a no such nick error can be intercepted and
       * replaced with a "Services down" reply. */
      *host = '\0';
      if (!(target = FindUser(nick)) || server != cli_user(target)->server) {
          continue;
      }
      *host = '@';

      relay_directed_message(sptr, nick, host, parv[parc - 1]);
      return 0;
  }

  return send_reply(sptr, ERR_SERVICESDOWN, map->name);
}
