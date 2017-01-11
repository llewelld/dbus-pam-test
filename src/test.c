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

/**
 * Program entry point.
 * Call a couple of flypig-test functions via DBUS.
 *
 * @param argv unusued.
 * @return 0 for success
 */
int main (int argv, char ** argc) {
	UkCoFlypigTest * proxy;
	GError * error;
	GDBusProxyFlags flags;
	gboolean result;
	gboolean beep;
	gint value;
	gint increment;

	// Open a dbus connection to the flypig-test service on the syste bus
	printf ("Opening bus\n");
	proxy = NULL;
	error = NULL;
	flags = G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS;
	
	proxy = uk_co_flypig_test_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM, flags, "uk.co.flypig.test", "/TestObject", NULL, & error);
	if (proxy == NULL) {
		printf ("Null proxy\n");
	}
	if (error != NULL) {
		printf ("Bus error: %s\n", error->message);
	}

	// Call the prod function of the flypig-test service
	printf ("Calling prod\n");
	error = NULL;
	beep = TRUE;
	result = uk_co_flypig_test_call_prod_sync (proxy, beep, NULL, & error);
	printf("Result %d\n", result);

	if (error != NULL) {
		printf ("Prod error: %s\n", error->message);
	}

	// Call the increment function of the flypig-test service
	printf ("Calling increment\n");
	error = NULL;
	value = 100;
	increment = 0;
	result = uk_co_flypig_test_call_increment_sync (proxy, value, & increment, NULL, & error);
	printf("Result %d\n", result);

	if (error != NULL) {
		printf ("Increment error: %s\n", error->message);
	}
	else {
		printf ("Value in %d, value out %d\n", value, increment);
	}

	printf ("Done\n");

	// Release the dbus proxy
	g_object_unref (proxy);

	return 0;
}

