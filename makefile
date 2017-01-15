CC = gcc
LIBS = `pkg-config --libs glib-2.0` `pkg-config --libs gio-unix-2.0`
CFLAGS = -Wall -Werror `pkg-config --cflags glib-2.0` `pkg-config --cflags gio-unix-2.0`

LIBS_DBUSGLIB = `pkg-config --libs dbus-glib-1`
CFLAGS_DBUSGLIB = `pkg-config --cflags dbus-glib-1`

all: service test pam_test

service: src/service.c src/generaged-code.c
	$(CC) src/service.c src/generated-code.c $(CFLAGS) $(LIBS) -o service

test: src/test.c src/generated-code.c
	$(CC) src/test.c src/generated-code.c $(CFLAGS) $(CFLAGS_DBUSGLIB) $(LIBS) $(LIBS_DBUSGLIB) -o test

pam_test: src/pam_test.c src/generated-code.c
	$(CC) -c src/pam_test.c src/generated-code.c -fPIC -DPIC $(CFLAGS) $(CFLAGS_DBUSGLIB)
	$(CC) -Wl,-z,nodelete -shared -o pam_test.so pam_test.o generated-code.o -lpam -ldl $(LIBS) $(LIBS_DBUSGLIB)

src/generaged-code.c src/generated-code.h: src/test-objectmanager.xml
	cd src && gdbus-codegen --interface-prefix uk.co.flypig.test.. --generate-c-code generated-code --c-namespace "" --c-generate-object-manager --generate-docbook generated-docs test-objectmanager.xml && cd ..

clean:
	rm service test pam_test.so *.o src/generated-*

install: all
	systemctl stop flypig-test.service | true
	mkdir -p /usr/share/flypig-test
	cp data/beep.wav /usr/share/flypig-test/
	cp service /usr/share/flypig-test/
	cp data/uk.co.flypig.test.conf /etc/dbus-1/system.d/
	cp data/flypig-test.service /etc/systemd/system/
	systemctl daemon-reload
	systemctl start flypig-test.service
	systemctl enable flypig-test.service
	cp pam_test.so /usr/lib/x86_64-linux-gnu/security/
	cp data/flypig-test /etc/pam.d/

uninstall:
	systemctl stop flypig-test.service | true
	systemctl disable flypig-test.service | true
	rm /usr/share/flypig-test/beep.wav
	rm /usr/share/flypig-test/service
	rm /etc/dbus-1/system.d/uk.co.flypig.test.conf
	rm /etc/systemd/system/flypig-test.service
	rmdir /usr/share/flypig-test
	systemctl daemon-reload
	rm /usr/lib/x86_64-linux-gnu/security/pam_test.so
	rm /etc/pam.d/flypig-test


