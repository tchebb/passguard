/* passguard.c
 * libpurple plugin to prevent accidental transmission of sensitive information.
 */

#include <glib.h>
#include <string.h>
#include <time.h>

// We are a plugin!
#define PURPLE_PLUGINS

#include "plugin.h"
#include "version.h"
#include "conversation.h"
#include "pluginpref.h"
#include "prefs.h"

static void init_plugin (PurplePlugin *plugin);
static gboolean load_plugin (PurplePlugin *plugin);
static void im_filter (PurpleAccount *account,
                       const char *receiver,
                       char **message,
                       gpointer data);
static void chat_filter (PurpleAccount *account,
                         char **message,
                         int id,
                         gpointer data);
static gboolean is_message_bad (char *message);
static void write_error (PurpleConversation *conv);

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	"core-tchebb-passguard",
	"Pass Guard",
	"0.1",
	"Prevent accidental transmission of sensitive information",
	"Have you ever typed in your password and pressed enter, only to \
realize a second too late that your chat client was focused and 1,296 people \
could now access your account? Worry no more, for Pass Guard will check your \
message for sensitive information and warn you before it gets sent.",
	"Thomas Hebb <tommyhebb@gmail.com>",
	"http://placeholder.site/",

	load_plugin,
	NULL,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL
};

PURPLE_INIT_PLUGIN(passguard, init_plugin, info);

static void init_plugin (PurplePlugin *plugin) {

}

static gboolean load_plugin (PurplePlugin *plugin) {
	purple_signal_connect(purple_conversations_get_handle(), "sending-im-msg", plugin, PURPLE_CALLBACK(im_filter), NULL);
	purple_signal_connect(purple_conversations_get_handle(), "sending-chat-msg", plugin, PURPLE_CALLBACK(chat_filter), NULL);

	return TRUE;
}

static void im_filter (PurpleAccount *account,
                       const char *receiver,
                       char **message,
                       gpointer data) {
	if (is_message_bad(*message)) {
		free(*message);
		*message = NULL;

		PurpleConversation *conv;
		conv = purple_find_conversation_with_account(
				PURPLE_CONV_TYPE_ANY, receiver, account);

		write_error(conv);
	}
}

static void chat_filter (PurpleAccount *account,
                         char **message,
                         int id,
                         gpointer data) {
	if (is_message_bad(*message)) {
		free(*message);
		*message = NULL;

		PurpleConversation *conv;
		conv = purple_find_chat(account->gc, id);

		write_error(conv);
	}
}

static gboolean is_message_bad (char *message) {
	return strstr(message, "asdf") != NULL;
}

static void write_error (PurpleConversation *conv) {
	if (conv != NULL) {
		purple_conversation_write(conv, NULL,
				"You probably don't want to send that!",
				PURPLE_MESSAGE_ERROR, time(NULL));
	}
}

