/*
 * logger.cpp
 *
 *  Created on: 16.09.2015
 *      Author: jrenken
 */

#include <syslog.h>
#include "logger.h"

Logger::Logger(QObject* parent)
    : QObject(parent)
{
}

void Logger::logMessage(const QString& msg)
{
    syslog(LOG_INFO, msg.toLatin1().data());
}
