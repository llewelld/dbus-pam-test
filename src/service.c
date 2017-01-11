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
 * The flypig-test service.provides a few test functions that can be called
 * using the system dbus.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "generated-code.h"

void beep();

/**
 * Perform the prod operation.
 *
 * @param object the dbus object.
 * @param invocation method invocation structure.
 * @param arg_beep true if the function should try to generate a beep.
 * @param user_data pointer to user date provided when signal is registered.
 * @return TRUE for success.
 */
static gboolean on_handle_prod (UkCoFlypigTest * object, GDBusMethodInvocation * invocation, gboolean arg_beep, gpointer user_data) {
	printf("uk.co.flypig.test.prod called\n");

	// Beep if the caller has requested it
	if (arg_beep) {
		printf("Beep!\n");
		beep();
	}

	// Complete the function call
	uk_co_flypig_test_complete_prod (object, invocation);

	printf("Returned\n");

	return TRUE;
}

/**
 * Perform the increment operation.
 *
 * @param object the dbus object.
 * @param invocation method invocation structure.
 * @param value The value to increment by 1.
 * @param user_data pointer to user date provided when signal is registered.
 * @return TRUE for success.
 */
static gboolean on_handle_increment (UkCoFlypigTest * object, GDBusMethodInvocation * invocation, guint value, gpointer user_data) {
	printf("uk.co.flypig.test.increment called\n");

	// Increment the output
	value++;

	// Complete the function call and return the incremented value
	uk_co_flypig_test_complete_increment (object, invocation, value);

	printf("Returned\n");

	return TRUE;
}

/**
 * Close down the service.
 *
 * @param object the dbus object.
 * @param invocation method invocation structure.
 * @param user_data pointer to user date provided when signal is registered.
 * @return TRUE for success.
 */
static gboolean on_handle_exit (UkCoFlypigTest * object, GDBusMethodInvocation * invocation, gpointer user_data) {
	printf("Exit\n");

	// Quit the service loop
	g_main_loop_quit ((GMainLoop *)user_data);
	
	// Complete the function call
	uk_co_flypig_test_complete_exit (object, invocation);

	return TRUE;
}

/**
 * Callback to set up the signal connections once the bus has been acquired.
 *
 * @param connection the dbus connection.
 * @param name message bus name.
 * @param user_data pointer to user date provided when callback is registered.
 */
static void on_bus_acquired (GDBusConnection * connection, const gchar * name, gpointer user_data) {
	GError *error;
	error = NULL;

	printf ("Acquired message bus connection\n");

	UkCoFlypigTest * object;

	object = uk_co_flypig_test_skeleton_new ();

	g_signal_connect (object, "handle-prod", G_CALLBACK (on_handle_prod), user_data);

	g_signal_connect (object, "handle-increment", G_CALLBACK (on_handle_increment), user_data);

	g_signal_connect (object, "handle-exit", G_CALLBACK (on_handle_exit), user_data);

	g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (object), connection, "/TestObject", & error);
}

/**
 * Callback triggered when thee bus name has been acquired.
 *
 * @param connection the dbus connection.
 * @param name message bus name.
 * @param user_data pointer to user date provided when callback is registered.
 */
static void on_name_acquired (GDBusConnection * connection, const gchar * name, gpointer user_data) {
	printf ("Acquired name: %s\n", name);
}

/**
 * Callback triggered when thee bus name is lost.
 *
 * @param connection the dbus connection.
 * @param name message bus name.
 * @param user_data pointer to user date provided when callback is registered.
 */
static void on_name_lost (GDBusConnection * connection, const gchar * name, gpointer user_data) {
	printf ("Lost name: %s\n", name);
}

/**
 * Service entry point.
 * Call a couple of flypig-test functions via DBUS.
 *
 * @param argv unusued.
 * @return 0 for success
 */
gint main (gint argc, gchar * argv[]) {
	GMainLoop *loop;
	guint id;

	loop = g_main_loop_new (NULL, FALSE);

	printf("Requesting to own bus\n");
	id = g_bus_own_name (G_BUS_TYPE_SYSTEM, "uk.co.flypig.test", G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE, on_bus_acquired, on_name_acquired, on_name_lost, loop, NULL);
	
	printf("Entering main loop\n");
	g_main_loop_run (loop);

	printf("Exited main loop\n");	
	g_bus_unown_name (id);
	g_main_loop_unref (loop);

	printf ("The End\n");
	return 0;
}

/**
 * Left as an exercise for the reader.
 *
 */
void beep() {
	system("aplay -q /usr/share/flypig-test/beep.wav");
}


