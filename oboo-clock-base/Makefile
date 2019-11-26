# Copyright (C) 2018 Onion Corporation
#
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=oboo-clock-base
PKG_VERSION:=0.0.8
PKG_RELEASE:=5


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

# cron tool
define Package/oboo-cron
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Cron Daemon System
endef

define Package/oboo-cron/description
Oboo Cron Daemon System
endef

# general scripts package
define Package/oboo-clock-scripts
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Clock scripts
	DEPENDS:=+url-enc-dec +coreutils-base64
endef

define Package/oboo-clock-scripts/description
Oboo Clock scripts
endef

# oboo clock meta package
define Package/oboo-clock-base
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Clock base package
	DEPENDS:= +oboo-repo-keys +oboo-clock-card-manager +oboo-clock-cards +oboo-clock-mcu +oboo-clock-scripts +oboo-cron +onion-script
endef

define Package/oboo-clock-base/description
Oboo Clock base package
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

# installation
define Package/oboo-cron/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/crontabs/
	# install the cron script and crontabs
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/usr/bin/run-all.sh $(1)/usr/bin/run-all
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/crontabs/root $(1)/etc/crontabs/root
	# cron directories
	$(INSTALL_DIR) $(1)/etc/cron.minute $(1)/etc/cron.hour $(1)/etc/cron.day $(1)/etc/cron.week
endef

define Package/oboo-clock-base/install
	$(INSTALL_DIR) $(1)/usr/share/oboo $(1)/etc/cron.week/ $(1)/etc/cron.hour/
	# install the package upgrade script
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/oboo-sw-upgrade.sh $(1)/usr/share/oboo/oboo-sw-upgrade.sh
	# install the overall firmware update script
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/firmware_update.sh $(1)/etc/cron.week/
	# install other scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/ntp-refresh.sh $(1)/etc/cron.hour/ntp-refresh.sh
	# config files
	$(INSTALL_DIR) $(1)/etc/ $(1)/usr/share/rpcd/acl.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/config/acl.d/oboo-app.json $(1)/usr/share/rpcd/acl.d/
	# website files
	$(INSTALL_DIR) $(1)/www/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/www/index.html $(1)/www/
endef

define Package/oboo-clock-scripts/install
	# general scripts
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/share/rpcd/acl.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/oboo.sh $(1)/usr/bin/oboo
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/configUpdate.sh $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/checkWifi.sh $(1)/usr/bin/
	# hotplug scripts
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb/ $(1)/etc/hotplug.d/iface/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/hotplug/usbUpgrade.sh $(1)/etc/hotplug.d/usb/99-oboo-upgrade
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/hotplug/wifiStatus.sh $(1)/etc/hotplug.d/iface/99-oboo-wifi
	# wifi provisioning scripts
	$(INSTALL_DIR) $(1)/www/cgi-bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/www/cgi-bin/ver $(1)/www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/www/cgi-bin/wifi-scan $(1)/www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/www/cgi-bin/wifi-setup $(1)/www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/www/cgi-bin/secure $(1)/www/cgi-bin/
endef

define Package/oboo-clock-base/postinst
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
	# update the mcu firmware
	echo "upgrading oboo packages" >> /tmp/oboo-clock-base
	opkg upgrade oboo-repo-keys oboo-clock-card-manager oboo-clock-cards oboo-clock-mcu
fi
exit 0
endef


$(eval $(call BuildPackage,oboo-cron))
$(eval $(call BuildPackage,oboo-clock-base))
$(eval $(call BuildPackage,oboo-clock-scripts))
