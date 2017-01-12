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

#include "generated-code.h"

gboolean timeout_exit (gpointer user_data);
void bus_proxy_acquired (GObject *source_object, GAsyncResult *res, gpointer user_data);
void prod_callback (GObject *source_object, GAsyncResult *res, gpointer user_data);
void increment_callback (GObject *source_object, GAsyncResult *res, gpointer user_data);

/**
 * Program entry point.
 * Call a couple of flypig-test functions via DBUS.
 *
 * @param argv unusued.
 * @return 0 for success
 */
int main (int argv, char ** argc) {
	GDBusProxyFlags flags;
	GMainLoop *loop;

	loop = g_main_loop_new (NULL, FALSE);

	// Open a dbus connection to the flypig-test service on the syste bus
	printf("Opening bus\n");
	flags = G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS;
	
	uk_co_flypig_test_proxy_new_for_bus (G_BUS_TYPE_SYSTEM, flags, "uk.co.flypig.test", "/TestObject", NULL, bus_proxy_acquired, NULL);

	g_timeout_add(1000, timeout_exit, loop);

	printf("Entering main loop\n");
	g_main_loop_run (loop);

	printf("Exited main loop\n");	
	g_main_loop_unref (loop);

	printf("The End\n");

	return 0;
}

gboolean timeout_exit (gpointer user_data) {
	// Quit the service loop
	g_main_loop_quit ((GMainLoop *)user_data);

	return FALSE;
}

void bus_proxy_acquired (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	GError * error;
	UkCoFlypigTest * proxy;
	gboolean beep;

	error = NULL;
	proxy = uk_co_flypig_test_proxy_new_for_bus_finish (res, & error);

	if (proxy == NULL) {
		printf("Null proxy\n");
	}
	if (error != NULL) {
		printf("Bus error: %s\n", error->message);
	}

	// Call the prod function of the flypig-test service
	printf("Calling prod\n");
	error = NULL;
	beep = TRUE;

	uk_co_flypig_test_call_prod (proxy, beep, NULL, prod_callback, user_data);

}

void prod_callback (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	GError * error;
	gboolean result;
	UkCoFlypigTest * proxy = (UkCoFlypigTest *)source_object;
	gint value;

	error = NULL;
	result = uk_co_flypig_test_call_prod_finish (proxy, res, & error);

	printf("Result %d\n", result);

	if (error != NULL) {
		printf("Prod error: %s\n", error->message);
	}

	value = 100;
	printf("Calling increment\n");
	printf("Increment in %d\n", value);
	uk_co_flypig_test_call_increment (proxy, value, NULL, increment_callback, user_data);
}

void increment_callback (GObject *source_object, GAsyncResult *res, gpointer user_data) {
	GError * error;
	gboolean result;
	UkCoFlypigTest * proxy = (UkCoFlypigTest *)source_object;
	gint out_increment;

	error = NULL;

	result = uk_co_flypig_test_call_increment_finish (proxy, & out_increment, res, & error);
	printf("Result %d\n", result);

	if (error != NULL) {
		printf("Increment error: %s\n", error->message);
	}

	printf("Output %d\n", out_increment);

	// Release the dbus proxy
	g_object_unref (proxy);

	printf("Done\n");
}


