# Copyright (C) 2018 Onion Corporation
#
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=oboo-clock-mcu
PKG_VERSION:=0.4.7
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Clock Microcontroller Firmware
	DEPENDS:=+arduino-dock-2 +bc
endef


define Package/$(PKG_NAME)/description
Oboo Clock Microcontroller Firmware
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

#define Package/$(PKG_NAME)/Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR)
#endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin/mcu
	# install the compiled firmware
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/bin/atmega328p/firmware.hex $(1)/usr/bin/mcu/firmware.hex
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/bin/atmega328p/firmware.hex $(1)/usr/bin/mcu/firmware-max7219.hex
	# install the version file
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/version $(1)/usr/bin/mcu/
	# scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/flash.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/reset.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/update.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/setButton.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/setColor.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/setMcuTime.sh $(1)/usr/bin/mcu/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/setSystemTime.sh $(1)/usr/bin/mcu/
	# service
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/hotplug.d/ntp/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/device/etc/init.d/coprocessor $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firmware/scripts/ntpd.sh $(1)/etc/hotplug.d/ntp/26-oboo-mcu
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
	# update the mcu firmware
	sh /usr/bin/mcu/update.sh
fi
exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
