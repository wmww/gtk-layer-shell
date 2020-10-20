/* This file is part of gtk-layer-shell
 *
 * Copyright (C) 2009 Carlos Garnacho <carlosg@gnome.org>
 * Copyright © 2020 gtk-priv/scripts/code.py
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

#ifndef GDK_WAYLAND_SEAT_PRIV_H
#define GDK_WAYLAND_SEAT_PRIV_H

typedef struct _GdkWaylandSeat GdkWaylandSeat;

// Version ID 0
// Valid for GTK v3.22.0 - v3.22.8
struct _GdkWaylandSeat_v3_22_0
{
  GdkSeat parent_instance;
  guint32 id;
  struct wl_seat *wl_seat;
  struct wl_pointer *wl_pointer;
  struct wl_keyboard *wl_keyboard;
  struct wl_touch *wl_touch;
  struct zwp_pointer_gesture_swipe_v1 *wp_pointer_gesture_swipe;
  struct zwp_pointer_gesture_pinch_v1 *wp_pointer_gesture_pinch;
  struct zwp_tablet_seat_v2 *wp_tablet_seat;
  GdkDisplay *display;
  GdkDeviceManager *device_manager;
  GdkDevice *master_pointer;
  GdkDevice *master_keyboard;
  GdkDevice *pointer;
  GdkDevice *wheel_scrolling;
  GdkDevice *finger_scrolling;
  GdkDevice *continuous_scrolling;
  GdkDevice *keyboard;
  GdkDevice *touch_master;
  GdkDevice *touch;
  GdkCursor *cursor;
  GdkKeymap *keymap;
  GHashTable *touches;
  GList *tablets;
  GList *tablet_tools;
  GList *tablet_pads;
  struct _GdkWaylandPointerData_v3_22_0 pointer_info;
  struct _GdkWaylandPointerData_v3_22_0 touch_info;
  GdkModifierType key_modifiers;
  GdkWindow *keyboard_focus;
  GdkAtom pending_selection;
  GdkWindow *grab_window;
  uint32_t grab_time;
  gboolean have_server_repeat;
  uint32_t server_repeat_rate;
  uint32_t server_repeat_delay;
  struct wl_callback *repeat_callback;
  guint32 repeat_timer;
  guint32 repeat_key;
  guint32 repeat_count;
  gint64 repeat_deadline;
  GSettings *keyboard_settings;
  uint32_t keyboard_time;
  uint32_t keyboard_key_serial;
  struct gtk_primary_selection_device *primary_data_device;
  struct wl_data_device *data_device;
  GdkDragContext *drop_context;
  GdkWindow *foreign_dnd_window;
  guint gesture_n_fingers;
  gdouble gesture_scale;
  GdkCursor *grab_cursor;
};

// Version ID 1
// Valid for GTK v3.22.9 - v3.22.15
struct _GdkWaylandSeat_v3_22_9
{
  GdkSeat parent_instance;
  guint32 id;
  struct wl_seat *wl_seat;
  struct wl_pointer *wl_pointer;
  struct wl_keyboard *wl_keyboard;
  struct wl_touch *wl_touch;
  struct zwp_pointer_gesture_swipe_v1 *wp_pointer_gesture_swipe;
  struct zwp_pointer_gesture_pinch_v1 *wp_pointer_gesture_pinch;
  struct zwp_tablet_seat_v2 *wp_tablet_seat;
  GdkDisplay *display;
  GdkDeviceManager *device_manager;
  GdkDevice *master_pointer;
  GdkDevice *master_keyboard;
  GdkDevice *pointer;
  GdkDevice *wheel_scrolling;
  GdkDevice *finger_scrolling;
  GdkDevice *continuous_scrolling;
  GdkDevice *keyboard;
  GdkDevice *touch_master;
  GdkDevice *touch;
  GdkCursor *cursor;
  GdkKeymap *keymap;
  GHashTable *touches;
  GList *tablets;
  GList *tablet_tools;
  GList *tablet_pads;
  struct _GdkWaylandPointerData_v3_22_0 pointer_info;
  struct _GdkWaylandPointerData_v3_22_0 touch_info;
  GdkModifierType key_modifiers;
  GdkWindow *keyboard_focus;
  GdkAtom pending_selection;
  GdkWindow *grab_window;
  uint32_t grab_time;
  gboolean have_server_repeat;
  uint32_t server_repeat_rate;
  uint32_t server_repeat_delay;
  struct wl_callback *repeat_callback;
  guint32 repeat_timer;
  guint32 repeat_key;
  guint32 repeat_count;
  gint64 repeat_deadline;
  gint32 nkeys;
  GSettings *keyboard_settings;
  uint32_t keyboard_time;
  uint32_t keyboard_key_serial;
  struct gtk_primary_selection_device *primary_data_device;
  struct wl_data_device *data_device;
  GdkDragContext *drop_context;
  GdkWindow *foreign_dnd_window;
  guint gesture_n_fingers;
  gdouble gesture_scale;
  GdkCursor *grab_cursor;
};

// Version ID 2
// Valid for GTK v3.22.16 - v3.24.23
struct _GdkWaylandSeat_v3_22_16
{
  GdkSeat parent_instance;
  guint32 id;
  struct wl_seat *wl_seat;
  struct wl_pointer *wl_pointer;
  struct wl_keyboard *wl_keyboard;
  struct wl_touch *wl_touch;
  struct zwp_pointer_gesture_swipe_v1 *wp_pointer_gesture_swipe;
  struct zwp_pointer_gesture_pinch_v1 *wp_pointer_gesture_pinch;
  struct zwp_tablet_seat_v2 *wp_tablet_seat;
  GdkDisplay *display;
  GdkDeviceManager *device_manager;
  GdkDevice *master_pointer;
  GdkDevice *master_keyboard;
  GdkDevice *pointer;
  GdkDevice *wheel_scrolling;
  GdkDevice *finger_scrolling;
  GdkDevice *continuous_scrolling;
  GdkDevice *keyboard;
  GdkDevice *touch_master;
  GdkDevice *touch;
  GdkCursor *cursor;
  GdkKeymap *keymap;
  GHashTable *touches;
  GList *tablets;
  GList *tablet_tools;
  GList *tablet_pads;
  struct _GdkWaylandPointerData_v3_22_0 pointer_info;
  struct _GdkWaylandPointerData_v3_22_0 touch_info;
  GdkModifierType key_modifiers;
  GdkWindow *keyboard_focus;
  GdkAtom pending_selection;
  GdkWindow *grab_window;
  uint32_t grab_time;
  gboolean have_server_repeat;
  uint32_t server_repeat_rate;
  uint32_t server_repeat_delay;
  struct wl_callback *repeat_callback;
  guint32 repeat_timer;
  guint32 repeat_key;
  guint32 repeat_count;
  gint64 repeat_deadline;
  GSettings *keyboard_settings;
  uint32_t keyboard_time;
  uint32_t keyboard_key_serial;
  struct gtk_primary_selection_device *primary_data_device;
  struct wl_data_device *data_device;
  GdkDragContext *drop_context;
  GdkWindow *foreign_dnd_window;
  guint gesture_n_fingers;
  gdouble gesture_scale;
  GdkCursor *grab_cursor;
};

// Version ID 3
// Valid for GTK v3.24.24 (unreleased)
struct _GdkWaylandSeat_v3_24_24
{
  GdkSeat parent_instance;
  guint32 id;
  struct wl_seat *wl_seat;
  struct wl_pointer *wl_pointer;
  struct wl_keyboard *wl_keyboard;
  struct wl_touch *wl_touch;
  struct zwp_pointer_gesture_swipe_v1 *wp_pointer_gesture_swipe;
  struct zwp_pointer_gesture_pinch_v1 *wp_pointer_gesture_pinch;
  struct zwp_tablet_seat_v2 *wp_tablet_seat;
  GdkDisplay *display;
  GdkDeviceManager *device_manager;
  GdkDevice *master_pointer;
  GdkDevice *master_keyboard;
  GdkDevice *pointer;
  GdkDevice *wheel_scrolling;
  GdkDevice *finger_scrolling;
  GdkDevice *continuous_scrolling;
  GdkDevice *keyboard;
  GdkDevice *touch_master;
  GdkDevice *touch;
  GdkCursor *cursor;
  GdkKeymap *keymap;
  GHashTable *touches;
  GList *tablets;
  GList *tablet_tools;
  GList *tablet_pads;
  struct _GdkWaylandPointerData_v3_22_0 pointer_info;
  struct _GdkWaylandPointerData_v3_22_0 touch_info;
  GdkModifierType key_modifiers;
  GdkWindow *keyboard_focus;
  GdkAtom pending_selection;
  GdkWindow *grab_window;
  uint32_t grab_time;
  gboolean have_server_repeat;
  uint32_t server_repeat_rate;
  uint32_t server_repeat_delay;
  struct wl_callback *repeat_callback;
  guint32 repeat_timer;
  guint32 repeat_key;
  guint32 repeat_count;
  gint64 repeat_deadline;
  GSettings *keyboard_settings;
  uint32_t keyboard_time;
  uint32_t keyboard_key_serial;
  struct gtk_primary_selection_device *gtk_primary_data_device;
  struct zwp_primary_selection_device_v1 *zwp_primary_data_device_v1;
  struct wl_data_device *data_device;
  GdkDragContext *drop_context;
  GdkWindow *foreign_dnd_window;
  guint gesture_n_fingers;
  gdouble gesture_scale;
  GdkCursor *grab_cursor;
};

// For internal use only
int gdk_wayland_seat_priv_get_version_id() {
  static int version_id = -1;
  
  if (version_id == -1) {
    if (gtk_get_major_version() != 3) {
      g_error("gtk-layer-shell only supports GTK3");
      g_abort();
    }
  
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
        break;
  
      default:
        g_warning(
          "gtk-layer-shell was not compiled with support for GTK v%d.%d.%d, program may crash",
          gtk_get_major_version(),
          gtk_get_minor_version(),
          gtk_get_micro_version());
    }
  
    if (combo >= 24024) {
      version_id = 3;
    } else if (combo >= 22016) {
      version_id = 2;
    } else if (combo >= 22009) {
      version_id = 1;
    } else {
      version_id = 0;
    }
  }
  
  return version_id;
}

// GdkWaylandSeat::parent_instance

GdkSeat * gdk_wayland_seat_priv_get_parent_instance_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (GdkSeat *)&((struct _GdkWaylandSeat_v3_22_0*)self)->parent_instance;
    case 1: return (GdkSeat *)&((struct _GdkWaylandSeat_v3_22_9*)self)->parent_instance;
    case 2: return (GdkSeat *)&((struct _GdkWaylandSeat_v3_22_16*)self)->parent_instance;
    case 3: return (GdkSeat *)&((struct _GdkWaylandSeat_v3_24_24*)self)->parent_instance;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::id

guint32 * gdk_wayland_seat_priv_get_id_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_0*)self)->id;
    case 1: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->id;
    case 2: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_16*)self)->id;
    case 3: return (guint32 *)&((struct _GdkWaylandSeat_v3_24_24*)self)->id;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wl_seat

struct wl_seat * gdk_wayland_seat_priv_get_wl_seat(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_seat;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_seat;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_seat;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_seat;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wl_seat(GdkWaylandSeat * self, struct wl_seat * wl_seat) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_seat = wl_seat; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_seat = wl_seat; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_seat = wl_seat; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_seat = wl_seat; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wl_pointer

struct wl_pointer * gdk_wayland_seat_priv_get_wl_pointer(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_pointer;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_pointer;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_pointer;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_pointer;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wl_pointer(GdkWaylandSeat * self, struct wl_pointer * wl_pointer) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_pointer = wl_pointer; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_pointer = wl_pointer; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_pointer = wl_pointer; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_pointer = wl_pointer; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wl_keyboard

struct wl_keyboard * gdk_wayland_seat_priv_get_wl_keyboard(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_keyboard;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_keyboard;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_keyboard;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_keyboard;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wl_keyboard(GdkWaylandSeat * self, struct wl_keyboard * wl_keyboard) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_keyboard = wl_keyboard; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_keyboard = wl_keyboard; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_keyboard = wl_keyboard; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_keyboard = wl_keyboard; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wl_touch

struct wl_touch * gdk_wayland_seat_priv_get_wl_touch(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_touch;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_touch;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_touch;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_touch;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wl_touch(GdkWaylandSeat * self, struct wl_touch * wl_touch) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wl_touch = wl_touch; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wl_touch = wl_touch; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wl_touch = wl_touch; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wl_touch = wl_touch; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wp_pointer_gesture_swipe

struct zwp_pointer_gesture_swipe_v1 * gdk_wayland_seat_priv_get_wp_pointer_gesture_swipe(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_pointer_gesture_swipe;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_pointer_gesture_swipe;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_pointer_gesture_swipe;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_pointer_gesture_swipe;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wp_pointer_gesture_swipe(GdkWaylandSeat * self, struct zwp_pointer_gesture_swipe_v1 * wp_pointer_gesture_swipe) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_pointer_gesture_swipe = wp_pointer_gesture_swipe; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_pointer_gesture_swipe = wp_pointer_gesture_swipe; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_pointer_gesture_swipe = wp_pointer_gesture_swipe; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_pointer_gesture_swipe = wp_pointer_gesture_swipe; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wp_pointer_gesture_pinch

struct zwp_pointer_gesture_pinch_v1 * gdk_wayland_seat_priv_get_wp_pointer_gesture_pinch(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_pointer_gesture_pinch;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_pointer_gesture_pinch;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_pointer_gesture_pinch;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_pointer_gesture_pinch;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wp_pointer_gesture_pinch(GdkWaylandSeat * self, struct zwp_pointer_gesture_pinch_v1 * wp_pointer_gesture_pinch) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_pointer_gesture_pinch = wp_pointer_gesture_pinch; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_pointer_gesture_pinch = wp_pointer_gesture_pinch; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_pointer_gesture_pinch = wp_pointer_gesture_pinch; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_pointer_gesture_pinch = wp_pointer_gesture_pinch; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wp_tablet_seat

struct zwp_tablet_seat_v2 * gdk_wayland_seat_priv_get_wp_tablet_seat(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_tablet_seat;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_tablet_seat;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_tablet_seat;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_tablet_seat;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wp_tablet_seat(GdkWaylandSeat * self, struct zwp_tablet_seat_v2 * wp_tablet_seat) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wp_tablet_seat = wp_tablet_seat; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wp_tablet_seat = wp_tablet_seat; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wp_tablet_seat = wp_tablet_seat; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wp_tablet_seat = wp_tablet_seat; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::display

GdkDisplay * gdk_wayland_seat_priv_get_display(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->display;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->display;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->display;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->display;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_display(GdkWaylandSeat * self, GdkDisplay * display) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->display = display; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->display = display; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->display = display; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->display = display; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::device_manager

GdkDeviceManager * gdk_wayland_seat_priv_get_device_manager(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->device_manager;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->device_manager;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->device_manager;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->device_manager;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_device_manager(GdkWaylandSeat * self, GdkDeviceManager * device_manager) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->device_manager = device_manager; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->device_manager = device_manager; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->device_manager = device_manager; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->device_manager = device_manager; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::master_pointer

GdkDevice * gdk_wayland_seat_priv_get_master_pointer(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->master_pointer;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->master_pointer;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->master_pointer;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->master_pointer;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_master_pointer(GdkWaylandSeat * self, GdkDevice * master_pointer) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->master_pointer = master_pointer; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->master_pointer = master_pointer; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->master_pointer = master_pointer; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->master_pointer = master_pointer; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::master_keyboard

GdkDevice * gdk_wayland_seat_priv_get_master_keyboard(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->master_keyboard;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->master_keyboard;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->master_keyboard;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->master_keyboard;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_master_keyboard(GdkWaylandSeat * self, GdkDevice * master_keyboard) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->master_keyboard = master_keyboard; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->master_keyboard = master_keyboard; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->master_keyboard = master_keyboard; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->master_keyboard = master_keyboard; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::pointer

GdkDevice * gdk_wayland_seat_priv_get_pointer(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->pointer;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->pointer;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->pointer;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->pointer;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_pointer(GdkWaylandSeat * self, GdkDevice * pointer) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->pointer = pointer; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->pointer = pointer; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->pointer = pointer; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->pointer = pointer; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::wheel_scrolling

GdkDevice * gdk_wayland_seat_priv_get_wheel_scrolling(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->wheel_scrolling;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->wheel_scrolling;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->wheel_scrolling;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->wheel_scrolling;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_wheel_scrolling(GdkWaylandSeat * self, GdkDevice * wheel_scrolling) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->wheel_scrolling = wheel_scrolling; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->wheel_scrolling = wheel_scrolling; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->wheel_scrolling = wheel_scrolling; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->wheel_scrolling = wheel_scrolling; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::finger_scrolling

GdkDevice * gdk_wayland_seat_priv_get_finger_scrolling(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->finger_scrolling;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->finger_scrolling;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->finger_scrolling;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->finger_scrolling;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_finger_scrolling(GdkWaylandSeat * self, GdkDevice * finger_scrolling) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->finger_scrolling = finger_scrolling; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->finger_scrolling = finger_scrolling; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->finger_scrolling = finger_scrolling; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->finger_scrolling = finger_scrolling; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::continuous_scrolling

GdkDevice * gdk_wayland_seat_priv_get_continuous_scrolling(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->continuous_scrolling;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->continuous_scrolling;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->continuous_scrolling;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->continuous_scrolling;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_continuous_scrolling(GdkWaylandSeat * self, GdkDevice * continuous_scrolling) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->continuous_scrolling = continuous_scrolling; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->continuous_scrolling = continuous_scrolling; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->continuous_scrolling = continuous_scrolling; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->continuous_scrolling = continuous_scrolling; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keyboard

GdkDevice * gdk_wayland_seat_priv_get_keyboard(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keyboard(GdkWaylandSeat * self, GdkDevice * keyboard) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard = keyboard; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard = keyboard; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard = keyboard; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard = keyboard; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::touch_master

GdkDevice * gdk_wayland_seat_priv_get_touch_master(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->touch_master;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->touch_master;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->touch_master;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->touch_master;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_touch_master(GdkWaylandSeat * self, GdkDevice * touch_master) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->touch_master = touch_master; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->touch_master = touch_master; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->touch_master = touch_master; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->touch_master = touch_master; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::touch

GdkDevice * gdk_wayland_seat_priv_get_touch(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->touch;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->touch;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->touch;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->touch;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_touch(GdkWaylandSeat * self, GdkDevice * touch) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->touch = touch; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->touch = touch; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->touch = touch; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->touch = touch; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::cursor

GdkCursor * gdk_wayland_seat_priv_get_cursor(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->cursor;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->cursor;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->cursor;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->cursor;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_cursor(GdkWaylandSeat * self, GdkCursor * cursor) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->cursor = cursor; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->cursor = cursor; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->cursor = cursor; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->cursor = cursor; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keymap

GdkKeymap * gdk_wayland_seat_priv_get_keymap(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keymap;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keymap;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keymap;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keymap;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keymap(GdkWaylandSeat * self, GdkKeymap * keymap) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keymap = keymap; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keymap = keymap; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keymap = keymap; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keymap = keymap; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::touches

GHashTable * gdk_wayland_seat_priv_get_touches(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->touches;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->touches;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->touches;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->touches;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_touches(GdkWaylandSeat * self, GHashTable * touches) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->touches = touches; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->touches = touches; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->touches = touches; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->touches = touches; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::tablets

GList * gdk_wayland_seat_priv_get_tablets(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->tablets;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->tablets;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->tablets;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->tablets;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_tablets(GdkWaylandSeat * self, GList * tablets) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->tablets = tablets; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->tablets = tablets; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->tablets = tablets; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->tablets = tablets; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::tablet_tools

GList * gdk_wayland_seat_priv_get_tablet_tools(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->tablet_tools;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->tablet_tools;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->tablet_tools;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->tablet_tools;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_tablet_tools(GdkWaylandSeat * self, GList * tablet_tools) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->tablet_tools = tablet_tools; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->tablet_tools = tablet_tools; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->tablet_tools = tablet_tools; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->tablet_tools = tablet_tools; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::tablet_pads

GList * gdk_wayland_seat_priv_get_tablet_pads(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->tablet_pads;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->tablet_pads;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->tablet_pads;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->tablet_pads;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_tablet_pads(GdkWaylandSeat * self, GList * tablet_pads) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->tablet_pads = tablet_pads; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->tablet_pads = tablet_pads; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->tablet_pads = tablet_pads; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->tablet_pads = tablet_pads; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::pointer_info

GdkWaylandPointerData * gdk_wayland_seat_priv_get_pointer_info_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_0*)self)->pointer_info;
    case 1: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_9*)self)->pointer_info;
    case 2: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_16*)self)->pointer_info;
    case 3: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_24_24*)self)->pointer_info;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::touch_info

GdkWaylandPointerData * gdk_wayland_seat_priv_get_touch_info_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_0*)self)->touch_info;
    case 1: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_9*)self)->touch_info;
    case 2: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_22_16*)self)->touch_info;
    case 3: return (GdkWaylandPointerData *)&((struct _GdkWaylandSeat_v3_24_24*)self)->touch_info;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::key_modifiers

GdkModifierType * gdk_wayland_seat_priv_get_key_modifiers_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (GdkModifierType *)&((struct _GdkWaylandSeat_v3_22_0*)self)->key_modifiers;
    case 1: return (GdkModifierType *)&((struct _GdkWaylandSeat_v3_22_9*)self)->key_modifiers;
    case 2: return (GdkModifierType *)&((struct _GdkWaylandSeat_v3_22_16*)self)->key_modifiers;
    case 3: return (GdkModifierType *)&((struct _GdkWaylandSeat_v3_24_24*)self)->key_modifiers;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keyboard_focus

GdkWindow * gdk_wayland_seat_priv_get_keyboard_focus(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_focus;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_focus;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_focus;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_focus;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keyboard_focus(GdkWaylandSeat * self, GdkWindow * keyboard_focus) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_focus = keyboard_focus; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_focus = keyboard_focus; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_focus = keyboard_focus; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_focus = keyboard_focus; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::pending_selection

GdkAtom * gdk_wayland_seat_priv_get_pending_selection_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (GdkAtom *)&((struct _GdkWaylandSeat_v3_22_0*)self)->pending_selection;
    case 1: return (GdkAtom *)&((struct _GdkWaylandSeat_v3_22_9*)self)->pending_selection;
    case 2: return (GdkAtom *)&((struct _GdkWaylandSeat_v3_22_16*)self)->pending_selection;
    case 3: return (GdkAtom *)&((struct _GdkWaylandSeat_v3_24_24*)self)->pending_selection;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::grab_window

GdkWindow * gdk_wayland_seat_priv_get_grab_window(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_window;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_window;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_window;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_window;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_grab_window(GdkWaylandSeat * self, GdkWindow * grab_window) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_window = grab_window; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_window = grab_window; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_window = grab_window; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_window = grab_window; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::grab_time

uint32_t gdk_wayland_seat_priv_get_grab_time(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_time;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_time;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_time;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_time;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_grab_time(GdkWaylandSeat * self, uint32_t grab_time) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_time = grab_time; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_time = grab_time; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_time = grab_time; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_time = grab_time; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::have_server_repeat

gboolean gdk_wayland_seat_priv_get_have_server_repeat(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->have_server_repeat;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->have_server_repeat;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->have_server_repeat;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->have_server_repeat;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_have_server_repeat(GdkWaylandSeat * self, gboolean have_server_repeat) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->have_server_repeat = have_server_repeat; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->have_server_repeat = have_server_repeat; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->have_server_repeat = have_server_repeat; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->have_server_repeat = have_server_repeat; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::server_repeat_rate

uint32_t gdk_wayland_seat_priv_get_server_repeat_rate(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->server_repeat_rate;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->server_repeat_rate;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->server_repeat_rate;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->server_repeat_rate;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_server_repeat_rate(GdkWaylandSeat * self, uint32_t server_repeat_rate) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->server_repeat_rate = server_repeat_rate; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->server_repeat_rate = server_repeat_rate; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->server_repeat_rate = server_repeat_rate; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->server_repeat_rate = server_repeat_rate; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::server_repeat_delay

uint32_t gdk_wayland_seat_priv_get_server_repeat_delay(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->server_repeat_delay;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->server_repeat_delay;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->server_repeat_delay;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->server_repeat_delay;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_server_repeat_delay(GdkWaylandSeat * self, uint32_t server_repeat_delay) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->server_repeat_delay = server_repeat_delay; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->server_repeat_delay = server_repeat_delay; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->server_repeat_delay = server_repeat_delay; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->server_repeat_delay = server_repeat_delay; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::repeat_callback

struct wl_callback * gdk_wayland_seat_priv_get_repeat_callback(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_callback;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_callback;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_callback;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_callback;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_repeat_callback(GdkWaylandSeat * self, struct wl_callback * repeat_callback) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_callback = repeat_callback; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_callback = repeat_callback; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_callback = repeat_callback; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_callback = repeat_callback; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::repeat_timer

guint32 * gdk_wayland_seat_priv_get_repeat_timer_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_timer;
    case 1: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_timer;
    case 2: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_timer;
    case 3: return (guint32 *)&((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_timer;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::repeat_key

guint32 * gdk_wayland_seat_priv_get_repeat_key_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_key;
    case 1: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_key;
    case 2: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_key;
    case 3: return (guint32 *)&((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_key;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::repeat_count

guint32 * gdk_wayland_seat_priv_get_repeat_count_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_count;
    case 1: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_count;
    case 2: return (guint32 *)&((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_count;
    case 3: return (guint32 *)&((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_count;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::repeat_deadline

gint64 * gdk_wayland_seat_priv_get_repeat_deadline_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (gint64 *)&((struct _GdkWaylandSeat_v3_22_0*)self)->repeat_deadline;
    case 1: return (gint64 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->repeat_deadline;
    case 2: return (gint64 *)&((struct _GdkWaylandSeat_v3_22_16*)self)->repeat_deadline;
    case 3: return (gint64 *)&((struct _GdkWaylandSeat_v3_24_24*)self)->repeat_deadline;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keyboard_settings

GSettings * gdk_wayland_seat_priv_get_keyboard_settings(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_settings;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_settings;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_settings;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_settings;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keyboard_settings(GdkWaylandSeat * self, GSettings * keyboard_settings) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_settings = keyboard_settings; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_settings = keyboard_settings; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_settings = keyboard_settings; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_settings = keyboard_settings; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keyboard_time

uint32_t gdk_wayland_seat_priv_get_keyboard_time(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_time;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_time;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_time;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_time;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keyboard_time(GdkWaylandSeat * self, uint32_t keyboard_time) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_time = keyboard_time; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_time = keyboard_time; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_time = keyboard_time; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_time = keyboard_time; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::keyboard_key_serial

uint32_t gdk_wayland_seat_priv_get_keyboard_key_serial(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_key_serial;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_key_serial;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_key_serial;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_key_serial;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_keyboard_key_serial(GdkWaylandSeat * self, uint32_t keyboard_key_serial) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->keyboard_key_serial = keyboard_key_serial; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->keyboard_key_serial = keyboard_key_serial; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->keyboard_key_serial = keyboard_key_serial; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->keyboard_key_serial = keyboard_key_serial; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::primary_data_device

gboolean gdk_wayland_seat_priv_get_primary_data_device_supported() {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return TRUE;
    case 1: return TRUE;
    case 2: return TRUE;
    case 3: return FALSE;
    default: g_error("Invalid version ID"); g_abort();
  }
}

struct gtk_primary_selection_device * gdk_wayland_seat_priv_get_primary_data_device_or_abort(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->primary_data_device;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->primary_data_device;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->primary_data_device;
    case 3: g_error("GdkWaylandSeat::primary_data_device not supported on this GTK"); g_abort();
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_primary_data_device_or_abort(GdkWaylandSeat * self, struct gtk_primary_selection_device * primary_data_device) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->primary_data_device = primary_data_device; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->primary_data_device = primary_data_device; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->primary_data_device = primary_data_device; break;
    case 3: g_error("GdkWaylandSeat::primary_data_device not supported on this GTK"); g_abort();
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::data_device

struct wl_data_device * gdk_wayland_seat_priv_get_data_device(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->data_device;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->data_device;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->data_device;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->data_device;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_data_device(GdkWaylandSeat * self, struct wl_data_device * data_device) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->data_device = data_device; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->data_device = data_device; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->data_device = data_device; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->data_device = data_device; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::drop_context

GdkDragContext * gdk_wayland_seat_priv_get_drop_context(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->drop_context;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->drop_context;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->drop_context;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->drop_context;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_drop_context(GdkWaylandSeat * self, GdkDragContext * drop_context) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->drop_context = drop_context; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->drop_context = drop_context; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->drop_context = drop_context; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->drop_context = drop_context; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::foreign_dnd_window

GdkWindow * gdk_wayland_seat_priv_get_foreign_dnd_window(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->foreign_dnd_window;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->foreign_dnd_window;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->foreign_dnd_window;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->foreign_dnd_window;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_foreign_dnd_window(GdkWaylandSeat * self, GdkWindow * foreign_dnd_window) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->foreign_dnd_window = foreign_dnd_window; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->foreign_dnd_window = foreign_dnd_window; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->foreign_dnd_window = foreign_dnd_window; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->foreign_dnd_window = foreign_dnd_window; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::gesture_n_fingers

guint gdk_wayland_seat_priv_get_gesture_n_fingers(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->gesture_n_fingers;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->gesture_n_fingers;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->gesture_n_fingers;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->gesture_n_fingers;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_gesture_n_fingers(GdkWaylandSeat * self, guint gesture_n_fingers) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->gesture_n_fingers = gesture_n_fingers; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->gesture_n_fingers = gesture_n_fingers; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->gesture_n_fingers = gesture_n_fingers; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->gesture_n_fingers = gesture_n_fingers; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::gesture_scale

gdouble * gdk_wayland_seat_priv_get_gesture_scale_ptr(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return (gdouble *)&((struct _GdkWaylandSeat_v3_22_0*)self)->gesture_scale;
    case 1: return (gdouble *)&((struct _GdkWaylandSeat_v3_22_9*)self)->gesture_scale;
    case 2: return (gdouble *)&((struct _GdkWaylandSeat_v3_22_16*)self)->gesture_scale;
    case 3: return (gdouble *)&((struct _GdkWaylandSeat_v3_24_24*)self)->gesture_scale;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::grab_cursor

GdkCursor * gdk_wayland_seat_priv_get_grab_cursor(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_cursor;
    case 1: return ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_cursor;
    case 2: return ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_cursor;
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_cursor;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_grab_cursor(GdkWaylandSeat * self, GdkCursor * grab_cursor) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: ((struct _GdkWaylandSeat_v3_22_0*)self)->grab_cursor = grab_cursor; break;
    case 1: ((struct _GdkWaylandSeat_v3_22_9*)self)->grab_cursor = grab_cursor; break;
    case 2: ((struct _GdkWaylandSeat_v3_22_16*)self)->grab_cursor = grab_cursor; break;
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->grab_cursor = grab_cursor; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::nkeys

gboolean gdk_wayland_seat_priv_get_nkeys_supported() {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return FALSE;
    case 1: return TRUE;
    case 2: return FALSE;
    case 3: return FALSE;
    default: g_error("Invalid version ID"); g_abort();
  }
}

gint32 * gdk_wayland_seat_priv_get_nkeys_ptr_or_null(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return NULL;
    case 1: return (gint32 *)&((struct _GdkWaylandSeat_v3_22_9*)self)->nkeys;
    case 2: return NULL;
    case 3: return NULL;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::gtk_primary_data_device

gboolean gdk_wayland_seat_priv_get_gtk_primary_data_device_supported() {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return FALSE;
    case 1: return FALSE;
    case 2: return FALSE;
    case 3: return TRUE;
    default: g_error("Invalid version ID"); g_abort();
  }
}

struct gtk_primary_selection_device * gdk_wayland_seat_priv_get_gtk_primary_data_device_or_abort(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 1: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 2: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->gtk_primary_data_device;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_gtk_primary_data_device_or_abort(GdkWaylandSeat * self, struct gtk_primary_selection_device * gtk_primary_data_device) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 1: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 2: g_error("GdkWaylandSeat::gtk_primary_data_device not supported on this GTK"); g_abort();
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->gtk_primary_data_device = gtk_primary_data_device; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

// GdkWaylandSeat::zwp_primary_data_device_v1

gboolean gdk_wayland_seat_priv_get_zwp_primary_data_device_v1_supported() {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: return FALSE;
    case 1: return FALSE;
    case 2: return FALSE;
    case 3: return TRUE;
    default: g_error("Invalid version ID"); g_abort();
  }
}

struct zwp_primary_selection_device_v1 * gdk_wayland_seat_priv_get_zwp_primary_data_device_v1_or_abort(GdkWaylandSeat * self) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 1: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 2: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 3: return ((struct _GdkWaylandSeat_v3_24_24*)self)->zwp_primary_data_device_v1;
    default: g_error("Invalid version ID"); g_abort();
  }
}

void gdk_wayland_seat_priv_set_zwp_primary_data_device_v1_or_abort(GdkWaylandSeat * self, struct zwp_primary_selection_device_v1 * zwp_primary_data_device_v1) {
  switch (gdk_wayland_seat_priv_get_version_id()) {
    case 0: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 1: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 2: g_error("GdkWaylandSeat::zwp_primary_data_device_v1 not supported on this GTK"); g_abort();
    case 3: ((struct _GdkWaylandSeat_v3_24_24*)self)->zwp_primary_data_device_v1 = zwp_primary_data_device_v1; break;
    default: g_error("Invalid version ID"); g_abort();
  }
}

#endif // GDK_WAYLAND_SEAT_PRIV_H
