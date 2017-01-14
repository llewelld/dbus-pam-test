/**
 * @file
 * @author  David Llewellyn-Jones <David.Llewellyn-Jones@cl.cam.ac.uk>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2017
 * MIT License
 *
 * @section DESCRIPTION
 *
 * Test out the flypig-test service by sending it some messages by DBUS.
 * 
 */

#include <stdio.h>

#include <dbus/dbus-glib-lowlevel.h>
/**
 * Program entry point.
 * Call a couple of flypig-test functions via DBUS.
 *
 * @param argv unusued.
 * @return 0 for success
 */
int main (int argv, char ** argc) {
	DBusGProxy * manager;
	DBusGConnection * connection;
	DBusConnection * conn;
	GError * error;
	gboolean result;
	int valuein;
	int valueout;
	gboolean beep;

	// Otherwise dbus-glib doesn't setup it value types
	// See https://cgit.freedesktop.org/libfprint/fprintd/tree/pam/pam_fprintd.c
	connection = dbus_g_bus_get (DBUS_BUS_SYSTEM, NULL);
	if (connection != NULL) {
		dbus_g_connection_unref (connection);
	}
	
	// Set us up a private D-Bus connection
	error = NULL;
	connection = dbus_g_bus_get_private (DBUS_BUS_SYSTEM, NULL, &error);

	// Set us up a private D-Bus connection
	if (connection == NULL) {
		printf ("Error while getting bus: %s\n", error->message);
	}
	else {
		manager = dbus_g_proxy_new_for_name (connection, "uk.co.flypig.test", "/TestObject", "uk.co.flypig.test");

		// Call the remote function
		beep = TRUE;
		result = dbus_g_proxy_call (manager, "Prod", &error, G_TYPE_BOOLEAN, beep, G_TYPE_INVALID, G_TYPE_INVALID);
		if (result == FALSE) {
			printf("Prod failed: %s\n", error->message);
			g_clear_error (&error);
		}

		// Call the remote function
		valuein = 56;
		valueout = 0;
		printf("Value in: %d\n", valuein);
		result = dbus_g_proxy_call (manager, "Increment", &error, G_TYPE_INT, valuein, G_TYPE_INVALID, G_TYPE_INT, & valueout, G_TYPE_INVALID);
		if (result == FALSE) {
			printf("Prod failed: %s\n", error->message);
			g_clear_error (&error);
		}
		printf("Value out: %d\n", valueout);

		// Pull down the connection
		g_object_unref (manager);

		conn = dbus_g_connection_get_connection(connection);
		dbus_connection_close (conn);
		dbus_g_connection_unref (connection);
	}

	printf ("Done\n");

	return 0;
}

