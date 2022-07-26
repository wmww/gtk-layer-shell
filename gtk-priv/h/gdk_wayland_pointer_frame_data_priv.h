/* AUTOGENERATED, DO NOT EDIT DIRECTLY
 * See gtk-priv/README.md for more information
 *
 * This file is part of gtk-layer-shell
 *
 * Copyright (C) 2009 Carlos Garnacho <carlosg@gnome.org>
 * Copyright © 2022 gtk-priv/scripts/code.py
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef GDK_WAYLAND_POINTER_FRAME_DATA_PRIV_H
#define GDK_WAYLAND_POINTER_FRAME_DATA_PRIV_H

#include "common.h"

typedef struct _GdkWaylandPointerFrameData GdkWaylandPointerFrameData;

// Version ID 0
// Valid for GTK v3.22.0 - v3.24.35 (unreleased)
struct _GdkWaylandPointerFrameData_v3_22_0
{
  GdkEvent *event;
  gdouble delta_x;
  gdouble delta_y;
  int32_t discrete_x;
  int32_t discrete_y;
  gint8 is_scroll_stop;
  enum wl_pointer_axis_source source;
};

// For internal use only
int gdk_wayland_pointer_frame_data_priv_get_version_id() {
  static int version_id = -1;
  
  if (version_id == -1) {
    gtk_priv_assert_gtk_version_valid();
    int combo = gtk_get_minor_version() * 1000 + gtk_get_micro_version();
  
    switch (combo) {
      case 22000:
      case 22001:
      case 22002:
      case 22003:
      case 22004:
      case 22005:
      case 22006:
      case 22007:
      case 22008:
      case 22009:
      case 22010:
      case 22011:
      case 22012:
      case 22013:
      case 22014:
      case 22015:
      case 22016:
      case 22017:
      case 22018:
      case 22019:
      case 22020:
      case 22021:
      case 22022:
      case 22023:
      case 22024:
      case 22025:
      case 22026:
      case 22027:
      case 22028:
      case 22029:
      case 22030:
      case 23000:
      case 23001:
      case 23002:
      case 23003:
      case 24000:
      case 24001:
      case 24002:
      case 24003:
      case 24004:
      case 24005:
      case 24006:
      case 24007:
      case 24008:
      case 24009:
      case 24010:
      case 24011:
      case 24012:
      case 24013:
      case 24014:
      case 24015:
      case 24016:
      case 24017:
      case 24018:
      case 24020:
      case 24021:
      case 24022:
      case 24023:
      case 24024:
      case 24025:
      case 24026:
      case 24027:
      case 24028:
      case 24029:
      case 24030:
      case 24031:
      case 24032:
      case 24033:
      case 24034:
        break;
  
      default:
        gtk_priv_warn_gtk_version_may_be_unsupported();
    }
  
    {
      version_id = 0;
    }
  }
  
  return version_id;
}

// GdkWaylandPointerFrameData::event

GdkEvent * gdk_wayland_pointer_frame_data_priv_get_event(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->event;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_pointer_frame_data_priv_set_event(GdkWaylandPointerFrameData * self, GdkEvent * event) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->event = event; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::delta_x

gdouble * gdk_wayland_pointer_frame_data_priv_get_delta_x_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (gdouble *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->delta_x;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::delta_y

gdouble * gdk_wayland_pointer_frame_data_priv_get_delta_y_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (gdouble *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->delta_y;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::discrete_x

int32_t * gdk_wayland_pointer_frame_data_priv_get_discrete_x_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (int32_t *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->discrete_x;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::discrete_y

int32_t * gdk_wayland_pointer_frame_data_priv_get_discrete_y_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (int32_t *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->discrete_y;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::is_scroll_stop

gint8 * gdk_wayland_pointer_frame_data_priv_get_is_scroll_stop_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (gint8 *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->is_scroll_stop;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandPointerFrameData::source

enum wl_pointer_axis_source * gdk_wayland_pointer_frame_data_priv_get_source_ptr(GdkWaylandPointerFrameData * self) {
  switch (gdk_wayland_pointer_frame_data_priv_get_version_id()) {
    case 0: return (enum wl_pointer_axis_source *)&((struct _GdkWaylandPointerFrameData_v3_22_0*)self)->source;
    default: g_error("Invalid version ID"); g_abort();
  }
}

#endif // GDK_WAYLAND_POINTER_FRAME_DATA_PRIV_H
