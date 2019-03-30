#ifndef SIMPLE_CONVERSIONS_H
#define SIMPLE_CONVERSIONS_H

#include <gdk/gdk.h>
#include "protocol/xdg-shell-client.h"
#include "protocol/wlr-layer-shell-unstable-v1-client.h"

enum xdg_positioner_gravity gdk_gravity_get_xdg_positioner_gravity (GdkGravity gravity);
enum xdg_positioner_anchor gdk_gravity_get_xdg_positioner_anchor (GdkGravity anchor);
enum xdg_positioner_constraint_adjustment gdk_anchor_hints_get_xdg_positioner_constraint_adjustment (GdkAnchorHints hints);

#endif // SIMPLE_CONVERSIONS_H
