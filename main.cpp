#include <QApplication>
#include <iostream>
#include "debug.h"
#include "config.h"
#include "jconfig.h"
#include "terminal.h"
#include "ipc.h"
#include "options.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Options options;
	
	JConfig conf(options.config());
	if (conf.error() != JConfig::ErrorNone)
	{
		// TODO: print error [log]
		return 1;
	}
	
	Ipc ipc(conf, (options.command().isNull() ? Ipc::ModeApplication : Ipc::ModeManager)); // Инициализация межпроцессного взаимодействия
	if (ipc.error() == Ipc::ErrorNone)
		switch (ipc.mode())
		{
			case Ipc::ModeManager:
				if (options.command() == "pid")
					ipc.cmd(Ipc::CmdPid);
				else if (options.command() == "status")
					ipc.cmd(Ipc::CmdStatus);
				else if (options.command() == "stop")
					ipc.cmd(Ipc::CmdStop);
				else if (options.command() == "configRead")
					ipc.cmd(Ipc::CmdConfigRead);
				else if (options.command() == "logReopen")
					ipc.cmd(Ipc::CmdLogReopen);
				else if (options.command() == "stateSave")
					ipc.cmd(Ipc::CmdStateSave);
				else
				{
					std::cerr << "Unknown command" << std::endl;
					options.showHelp(true, 2);
					ipc.disable();
					return 2;
				}
				
				QObject::connect(&app, SIGNAL(aboutToQuit()), &ipc, SLOT(disable()));
				return app.exec();
			case Ipc::ModeApplication:
			{
				QPalette p = app.palette();
				p.setColor(QPalette::Window, "white");
				p.setColor(QPalette::Active, QPalette::Button, *(new QColor(223, 128, 38)));
				p.setColor(QPalette::Disabled, QPalette::Button, *(new QColor(225, 225, 225)));
				p.setColor(QPalette::ButtonText, "white");
				app.setPalette(p);
				
				Terminal term(conf, ipc);
				QObject::connect(&app, SIGNAL(aboutToQuit()), &term, SLOT(shutdown()));
				
				return app.exec();
			}
		}
	else
	{
		ipc.disable();
		switch (ipc.error())
		{
			case Ipc::ErrorAlreadyExists:
				std::cerr << "Aplication is already running" << std::endl;
				return 3;
			case Ipc::ErrorNotFound:
				std::cerr << "Aplication is not running" << std::endl;
				return 4;
			case Ipc::ErrorShmem:
				std::cerr << "Shared memory error" << std::endl;
				return 5;
			case Ipc::ErrorOther:
				std::cerr << "Unknown IPC error" << std::endl;
				return 6;
			default:;
		}
	}
	
	ipc.disable();
	return -1;
}
