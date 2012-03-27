/*
 * Kurento Android Media: Android Media Library based on FFmpeg.
 * Copyright (C) 2011  Tikal Technologies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
		android_log = ANDROID_LOG_FATAL;
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

