/**
 * @file
 * @author  David Llewellyn-Jones <David.Llewellyn-Jones@cl.cam.ac.uk>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2017
 * Derived from code written by Markus Gutschke and released under the Apache 
 * License, Version 2.0
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * @section DESCRIPTION
 *
 * A pam module that uses dbus to call a service. For testing purposes. Not
 * much use for authentication.
 * 
 */

#include <stdlib.h>
#include <syslog.h>

#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib-bindings.h>

// Useful reference material:
// The Linux-PAM Module Writers' Guide:
//   http://www.linux-pam.org/Linux-PAM-html/Linux-PAM_MWG.html
// The Linux-PAM Application Developers' Guide
//   http://www.linux-pam.org/Linux-PAM-html/Linux-PAM_ADG.html

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

/* Prevent certain PAM functions from using const arguments */
#define PAM_CONST const

#if !defined(LOG_AUTHPRIV) && defined(LOG_AUTH)
#define LOG_AUTHPRIV LOG_AUTH
#endif

#define PAM_SM_AUTH
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define MODULE_NAME "pam_test"

static int converse(pam_handle_t *pamh, int nargs, PAM_CONST struct pam_message **message, struct pam_response **response);
void prompt(pam_handle_t *pamh, int style, PAM_CONST char *prompt);
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv);
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv);

/**
 * Calls the PAM conversation function. This is the feedback callback provided 
 * by the client application for communicating with it.
 *
 * @param pamh The handle provided by PAM
 * @param nargs The number of messages to send (usually 1)
 * @param message The message to be displayed by the client
 * @param response Pointer to a struct pam_response array that the response(s) 
 *                 will be returned using
 * @return One of PAM_BUF_ERR (memory buffer error), PAM_CONV_ERR (conversation
 *         failure) or PAM_SUCCESS (success!)
 */
static int converse(pam_handle_t *pamh, int nargs, PAM_CONST struct pam_message **message, struct pam_response **response) {
	struct pam_conv *conv;
	int retval;

	// Retrieve the conversation callback
	retval = pam_get_item(pamh, PAM_CONV, (void *)&conv);
	if (retval != PAM_SUCCESS) {
		return retval;
	}

	// Call the conversation callback and return the result
	return conv->conv(nargs, message, response, conv->appdata_ptr);
}

/**
 * Prompt the user via the client application, by invoking the client
 * applications conversation callback. Messages can either expect a
 * response from the user, or be entirely informational (no response 
 * expected).
 *
 * @param pamh The handle provided by PAM
 * @param style One of either PAM_PROMPT_ECHO_OFF or PAM_PROMPT_TEXT_INFO
 * @param prompt The message text to display to the user
 */
void prompt(pam_handle_t *pamh, int style, PAM_CONST char *prompt) {
	// Display QR code
	struct pam_message message;
	PAM_CONST struct pam_message *msgs = &message;
	struct pam_response *resp = NULL;
	int retval;

	// Set up the message structure
	message.msg_style = style;
	message.msg = prompt;

	// Call our wrapper to the conversation function
	retval = converse(pamh, 1, &msgs, &resp);
	if (retval != PAM_SUCCESS) {
	  syslog(LOG_WARNING, "Converse returned failure %d.", retval);
	}

	// Deallocate temporary storage
	if (resp) {
		if (resp->resp) {
			free(resp->resp);
		}
		free(resp);
	}
}

/**
 * Service function for user authentication.
 * This is the service module's implementation of the pam_authenticate(3)
 * interface.
 *
 * @param pamh The handle provided by PAM
 * @param flags Flags, potentially Or'd with PAM_SILENT.
 *              PAM_SILENT (do not emit any messages),
 *              PAM_DISALLOW_NULL_AUTHTOK (return PAM_AUTH_ERR if the database
 *              of authentication tokens has a NULL entry for this user
 * @param argc Number of PAM module arguments
 * @param argv Array of pointers to the arguments
 * @return PAM_AUTH_ERR (authentication failure),
 *         PAM_SUCCESS (athentication success)
 */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	gboolean result;
	GError * error;
	DBusGConnection * connection;

	syslog(LOG_INFO, "Test authentication for pam_test");

	// Otherwise dbus-glib doesn't setup it value types
	// See https://cgit.freedesktop.org/libfprint/fprintd/tree/pam/pam_fprintd.c
	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);
	if (connection == NULL) {
			syslog(LOG_ERR, "pam_test: connection error: %s\n", error->message);
			g_clear_error (&error);
	}
	else {
		dbus_g_connection_unref (connection);
	}

	result = PAM_SUCCESS;

	return result;
}

/**
 * Service function to alter credentials.
 * This function performs the task of altering the credentials of the user
 * with respect to the corresponeding authorization scheme.
 *
 * @param pamh The handle provided by PAM
 * @param flags Flags, potentially OR'd with PAM_SILENT.
 *              PAM_SILENT (do not emit any messages),
 *              PAM_ESTABLISH_CRED (initialize the credentials for the user),
 *              PAM_DELETE_CRED (delete the credentials associated with the
 *              authentication service),
 *              PAM_REINITIALIZE_CRED (reinitialize the user credentials),
 *              PAM_REFRESH_CRED (extend the lifetime of the user credentials)
 * @param argc Number of PAM module arguments
 * @param argv Array of pointers to the arguments
 * @return PAM_CRED_UNAVAIL (this module cannot retrieve the user's
 *         credentials),
 *         PAM_CRED_EXPIRED (the user's credentials have expired),
 *         PAM_CRED_ERR (this module was unable to set the crednetials for the
 *         user),
 *         PAM_SUCCESS (the user credential was successfully set),
 *         PAM_USER_UNKNOWN (the user is not known to this authentication
 *         module)
 */
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}


// Provide details of the module and callbacks
#ifdef PAM_STATIC
struct pam_module _pam_listfile_modstruct = {
  MODULE_NAME,
  pam_sm_authenticate,
  pam_sm_setcred,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif


