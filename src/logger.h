/*
 * logger.h
 *
 *  Created on: 16.09.2015
 *      Author: jrenken
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <QObject>

class Logger: public QObject
{
    Q_OBJECT

public:
    Logger(QObject* parent = 0);

public slots:
    static void logMessage(const QString& msg);
};

#endif /* SRC_LOGGER_H_ */
