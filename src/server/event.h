/***************************************************************************
**                                                                        **
**   Copyright (C) 2009-2010 Nokia Corporation.                           **
**                                                                        **
**   Author: Ilya Dogolazky <ilya.dogolazky@nokia.com>                    **
**                                                                        **
**     This file is part of Timed                                         **
**                                                                        **
**     Timed is free software; you can redistribute it and/or modify      **
**     it under the terms of the GNU Lesser General Public License        **
**     version 2.1 as published by the Free Software Foundation.          **
**                                                                        **
**     Timed is distributed in the hope that it will be useful, but       **
**     WITHOUT ANY WARRANTY;  without even the implied warranty  of       **
**     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               **
**     See the GNU Lesser General Public License  for more details.       **
**                                                                        **
**   You should have received a copy of the GNU  Lesser General Public    **
**   License along with Timed. If not, see http://www.gnu.org/licenses/   **
**                                                                        **
***************************************************************************/
#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <string>
#include <map>

using namespace std ;

#include <QDBusPendingCallWatcher>

#include <iodata/iodata.h>

#include <timed/event>

#include "timed/event-io.h"

#include "wrappers.h"
#include "timeutil.h"
#include "flags.h"

struct recurrence_pattern_t
{
  uint64_t mins ;
  uint32_t hour ;
  uint32_t mday ;
  uint32_t wday ;
  uint32_t mons ;

  recurrence_pattern_t()
  {
    mins = hour = mday = wday = mons = 0 ; // XXX: change it
  }
  static iodata::bit_codec *mins_codec, *hour_codec, *mday_codec, *wday_codec, *mons_codec ;

  uint64_t mins_mask(const string &x) const { return mins_codec->decode(x) ; }
  uint64_t hour_mask(const string &x) const { return hour_codec->decode(x) ; }
  uint64_t mday_mask(const string &x) const { return mday_codec->decode(x) ; }
  uint64_t wday_mask(const string &x) const { return wday_codec->decode(x) ; }
  uint64_t mons_mask(const string &x) const { return mons_codec->decode(x) ; }

  iodata::record *save() const ;
  void load(const iodata::record *r) ;
} ;

struct attribute_t
{
  map<string,string> txt ;
  typedef map<string,string>::iterator iterator ;
  typedef map<string,string>::const_iterator const_iterator ;

  string operator() (const string &key) const { const_iterator it=txt.find(key) ; return it==txt.end() ? string() : it->second ; }
  void operator() (const string &key, const string &val) ;

  iodata::record *save() const ;
  void load(const iodata::record *a) ;
} ;

struct action_t
{
  attribute_t attr ;
  uint32_t flags ;

  action_t() { flags = 0 ; }

  static iodata::bit_codec *codec ;
  iodata::record *save() const ;
  void load(const iodata::record *r) ;
} ;

struct state ;

struct event_t
{
  cookie_t cookie ;

  ticker_t ticker ;
  broken_down_t t ;
  string tz ;

  attribute_t attr ;
  uint32_t flags ;

  uint32_t tsz_counter, tsz_max ;

  vector<recurrence_pattern_t> recrs ;
  vector<action_t> actions ;
  vector<unsigned> snooze ;
  vector<attribute_t> b_attr ;

  static event_t *from_dbus_iface(const Maemo::Timed::event_io_t *) ;
  static event_t *from_queue_file(iodata::record *r) ;
  static event_t *from_event_file(iodata::record *r) ;

  static bool check_attributes(string &error_message, const attribute_t &a, bool empty_only) ;

  event_t() ;
 ~event_t() ;

  ticker_t trigger, last_triggered ;
  int to_be_snoozed ;
  state *st ;
  QDBusPendingCallWatcher *dialog_req_watcher ;

  state *get_state() { return st ; }
  void set_state(state *s) { st=s ; }
  void run_actions(uint32_t) ;
  void execute_dbus(const action_t &a) ;
  void execute_command(const action_t &a) ;
  void prepare_command(const action_t &a, string &cmd, string &user) ;
  static void add_strings(QMap<QString, QVariant> &x, const map<string,string> &y) ;
  string find_action_attribute(const string &, const action_t &a, bool exc=true) ;
  bool has_ticker() { return ticker.is_valid() ; }
  bool has_time() { return t.year >= 1970 ; }
  bool has_timezone() { return ! tz.empty() ; }
  bool has_recurrence() { return recrs.size() > 0 ; }
  bool to_be_keeped() { return (flags & EventFlags::Keep_Alive) !=0 ; }
  void invalidate_t() { t.year = t.month = t.day = t.hour = t.minute = 0 ; }
  string broken_str() ;
  bool compute_recurrence() ;
  void process_dialog_ack() ;
  int fork_and_set_credentials(const action_t &action, bool &error) ;

  static iodata::bit_codec *codec ;
  static void codec_initializer() ;
  iodata::record *save() ;
} ;

#endif
