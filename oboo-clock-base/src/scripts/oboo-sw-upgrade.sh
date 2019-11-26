#!/bin/sh

mosquitto_pub -t /notification -m '{"cmd":"set","text":"Checking for updates", "size":30}'
opkg remove oboo-clock-alarm-system oboo-clock-fw-manager oboo-clock-provisioning-scripts base-www
opkg upgrade onion-script oboo-clock-base oboo-clock-card-manager oboo-clock-cards oboo-clock-mcu oboo-clock-scripts oboo-cron  oboo-repo-keys

oboo uhttpd cors enable
/etc/init.d/rpcd restart

sh /usr/bin/mcu/update.sh

# modifications to rc.local
rcLocalUpdate=$(grep setupMode /etc/rc.local)
if [ "$rcLocalUpdate" == "" ]; then
  echo "touch /tmp/setupMode
/etc/init.d/sysntpd restart
exit 0" > /etc/rc.local
fi

# reconstruct config.json with latest required settings
/usr/bin/ort /usr/bin/js/updateConfig.js

mosquitto_pub -t /notification -m '{"cmd":"set","text":"Done!", "size":30}'
sleep 2
/etc/init.d/card-manager restart
/etc/init.d/oboo-cards restart
