#ifndef POWER_H
#define POWER_H

enum Action
{
    sleep,
    hibernate,
    shutdownnormal,
    shutdownhybrid,
    restart,
    lock,
    signout,
    monitoroff
};

#define	STRING_SLEEP					"Sleep"
#define	STRING_HIBERNATE				"Hibernate"
#define	STRING_SHUTDOWN					"Shut down"
#define	STRING_SHUTDOWN_HYBRID			"Shut down (Hybrib)"
#define	STRING_RESTART					"Restart"
#define	STRING_LOCK                     "Lock"
#define	STRING_SIGNOUT					"Sign out"
#define	STRING_MONITOR_OFF				"Turn off monitor"

class Power
{
public:
    static bool sleep();
    static bool hibernate();
    static bool is_support_hibernate();
    static bool shutdown(bool forced);
    static bool shutdown_hybrib(bool forced);
    static bool restart(bool forced);
    static bool lock();
    static bool signout();
    static void monitor_off();
};

#endif // POWER_H
