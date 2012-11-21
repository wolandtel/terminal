#ifndef CONFIG_H
#define CONFIG_H

#include <QApplication>
#include <QString>

#define DEF_CURRENCY QString::fromUtf8("руб.")
#define DEF_HELPER QApplication::applicationDirPath() + "/helper/helper.py"
#define DEF_FORMAT_DATE "dd.MM.yyyy"
#define DEF_FORMAT_TIME "hh:mm:ss"

#endif // CONFIG_H
