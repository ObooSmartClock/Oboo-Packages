# Copyright (C) 2018 Onion Corporation
#
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=oboo-clock-cards
PKG_VERSION:=0.2.9
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk


# application package
define Package/oboo-clock-cards
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Card framework and applications
	DEPENDS:=+libmosquitto-ssl +mosquitto-ssl +mosquitto-client-ssl +libcurl +ca-bundle +mpg123 +alsa-utils
endef

define Package/oboo-clock-cards/description
Oboo Card framework and applications
endef

# script package
define Package/oboo-clock-cards-scripts
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo Card demos scripts
	DEPENDS:=+libmosquitto-ssl +mosquitto-ssl +mosquitto-client-ssl
endef

define Package/oboo-clock-cards-scripts/description
Oboo Card demos scripts
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib
# TARGET_LIB += -lmosquitto

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/oboo-clock-cards/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/bin/img $(1)/usr/bin/js $(1)/etc/init.d/ $(1)/etc/cron.minute/
	# install the images
	$(CP) $(PKG_BUILD_DIR)/weather-card/img/bin/* $(1)/usr/bin/img/.
	$(CP) $(PKG_BUILD_DIR)/timer-card/img/bin/* $(1)/usr/bin/img/.
	$(CP) $(PKG_BUILD_DIR)/upgrade-card/img/bin/* $(1)/usr/bin/img/.
	$(CP) $(PKG_BUILD_DIR)/music-card/img/bin/* $(1)/usr/bin/img/.
	$(CP) $(PKG_BUILD_DIR)/system-card/img/bin/* $(1)/usr/bin/img/.
	# install the audio files
	$(INSTALL_DIR) $(1)/usr/share/oboo/alarm/
	$(CP) $(PKG_BUILD_DIR)/audio-manager/audio/silence-1s.mp3 $(1)/usr/share/oboo/alarm/silence-1s.mp3
	$(CP) $(PKG_BUILD_DIR)/alarm/audio/timer-0.mp3 $(1)/usr/share/oboo/alarm/timer-0.mp3
	$(CP) $(PKG_BUILD_DIR)/alarm/audio/alarm_classic.mp3 $(1)/usr/share/oboo/alarm/alarm_classic.mp3
	# install the runtime binary
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oboo-runtime/bin/ort $(1)/usr/bin/
	# install the js scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oboo-runtime/js/card-lib.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oboo-runtime/js/yahooWeather.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/timer-card/timer.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/weather-card/weather.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/calendar-card/calendar.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/upgrade-card/upgrade.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/input-processor/mcu.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/audio-manager/audio.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/music-card/music.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/device-client/client.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/system-card/system.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/alarm/alarm.js $(1)/usr/bin/js/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/config-update/updateConfig.js $(1)/usr/bin/js/
	# install the init.d script
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/device/etc/init.d/oboo-cards $(1)/etc/init.d/oboo-cards
	# install the cron scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/alarm/alarm-cron.sh $(1)/etc/cron.minute/alarm.sh
endef

define Package/oboo-clock-cards-scripts/install
	$(INSTALL_DIR) $(1)/usr/bin
	# install the demo scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/weather-card/init.sh $(1)/usr/bin/weather.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/timer-card/init.sh $(1)/usr/bin/timer.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/demo.sh $(1)/usr/bin/demo.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/runDemo.sh $(1)/usr/bin/runDemo.sh

endef

$(eval $(call BuildPackage,oboo-clock-cards))
$(eval $(call BuildPackage,oboo-clock-cards-scripts))
