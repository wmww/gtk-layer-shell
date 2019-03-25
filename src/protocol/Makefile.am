all: wlr-layer-shell-unstable-v1-client.h wlr-layer-shell-unstable-v1.c xdg-shell-client.h xdg-shell.c

wlr-layer-shell-unstable-v1-client.h: wlr-layer-shell-unstable-v1.xml
	wayland-scanner -c client-header wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1-client.h

# 'code' is depricated, and can be replaced with 'private-code' when all platforms have a new enough wayland-scanner
wlr-layer-shell-unstable-v1.c: wlr-layer-shell-unstable-v1.xml
	wayland-scanner -c code wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.c

xdg-shell-client.h: xdg-shell.xml
	wayland-scanner -c client-header xdg-shell.xml xdg-shell-client.h

# 'code' is depricated, and can be replaced with 'private-code' when all platforms have a new enough wayland-scanner
xdg-shell.c: xdg-shell.xml
	wayland-scanner -c code xdg-shell.xml xdg-shell.c

clean:
	rm -f wlr-layer-shell-unstable-v1-client.h
	rm -f wlr-layer-shell-unstable-v1.c
	rm -f xdg-shell-client.h
	rm -f xdg-shell.c
