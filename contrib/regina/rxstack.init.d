#!/bin/bash
#
# chkconfig: 
# description: Regina Stack daemon.
# processname: rxstack
# pidfile: /var/run/rxstack.pid
# config: /etc/sysconfig/rxstack

# source function library
. /etc/rc.d/init.d/functions

RETVAL=0

case "$1" in
  start)
	echo -n "Starting Regina Stack: "
	daemon /usr/bin/rxstack -d
	echo
	[ $RETVAL -eq 0 ] && touch /var/lock/subsys/rxstack
	;;
  stop)
	echo -n "Shutting down Regina Stack: "
	killproc /usr/bin/rxstack -2
	RETVAL=$?
	[ $RETVAL -eq 0 ] && rm -f /var/lock/subsys/rxstack
	echo
	;;
  restart|reload)
	$0 stop
	$0 start
	RETVAL=$?
	;;
  status)
	status rxstack
	RETVAL=$?
	;;
  *)
	echo "Usage: rxstack {start|stop|status|restart|reload}"
	exit 1
esac

exit $RETVAL

