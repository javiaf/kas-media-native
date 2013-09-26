/*
 * (C) Copyright 2013 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "init-log.h"
#include <util/log.h>
#include <android/log.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int initialized = 0;

static void
android_media_log(MediaLogLevel level, const char *tag, const char *fmt, va_list vargs)
{
	android_LogPriority android_log = ANDROID_LOG_UNKNOWN;
	switch(level){
	case MEDIA_LOG_VERBOSE:
		android_log = ANDROID_LOG_VERBOSE;
		break;
	case MEDIA_LOG_DEBUG:
		android_log = ANDROID_LOG_DEBUG;
		break;
	case MEDIA_LOG_INFO:
		android_log = ANDROID_LOG_INFO;
		break;
	case MEDIA_LOG_WARN:
		android_log = ANDROID_LOG_WARN;
		break;
	case MEDIA_LOG_ERROR:
		android_log = ANDROID_LOG_ERROR;
		break;
	case MEDIA_LOG_FATAL:
		android_log = ANDROID_LOG_FATAL;
		break;
	}

	__android_log_vprint(android_log, tag, fmt, vargs);
}

int
init_log()
{
	int ret = 0;

	pthread_mutex_lock(&mutex);
	if(!initialized) {
		media_log_set_callback(android_media_log);
		initialized++;
	}
	pthread_mutex_unlock(&mutex);

	return ret;
}

