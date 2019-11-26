# Copyright (C) 2018 Onion Corporation
# 
# Author: Lazar Demin  <lazar@onion.io>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
PKG_NAME:=oboo-repo-keys
PKG_VERSION:=0.0.1
PKG_RELEASE:=1


PKG_BUILD_DEPENDS:=opkg/host usign/host
PKG_CONFIG_DEPENDS := CONFIG_SIGNED_PACKAGES

include $(INCLUDE_DIR)/package.mk

# oboo repo keys
define Package/oboo-repo-keys
	SECTION:=onion
	CATEGORY:=Onion
	SUBMENU:=Oboo
	TITLE:=Signature keys required for Oboo Package Repo
	DEPENDS:=+SIGNED_PACKAGES:usign
endef

define Package/oboo-repo-keys/description
Signature keys required for Onion Package Repo
endef



define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile

endef

define Package/oboo-repo-keys/install
	mkdir -p $(1)/etc/opkg/keys
	$(CP) ./files/oboo.pub $(1)/etc/opkg/keys/`$(STAGING_DIR_HOST)/bin/usign -F -p ./files/oboo.pub`
endef



$(eval $(call BuildPackage,oboo-repo-keys))


