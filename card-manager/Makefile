# Copyright (C) 2018 Onion Corporation
#
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=oboo-clock-card-manager
PKG_VERSION:=0.2.9
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk


# application package
define Package/oboo-clock-card-manager
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Oboo card manager and GUI framework
	DEPENDS:=+libmosquitto-ssl +mosquitto-ssl +mosquitto-client-ssl
endef

define Package/oboo-clock-card-manager/description
Oboo card manager and GUI framework
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/oboo-clock-card-manager/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d
	# install the binary
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/card-manager $(1)/usr/bin/
	# install the init.d script
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/device/etc/init.d/card-manager $(1)/etc/init.d/card-manager
	# install scripts
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/test/cards/status.sh $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/test/cards/select.sh $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
