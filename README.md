# Oboo-Packages
OpenWRT Packages Feed for Oboo Smart Clock.

This repository holds the source code and OpenWRT makefiles for all custom Oboo Smart Clock software packages.

## Where are the Packages Published?

The compiled packages can be found here: http://repo.getoboo.com/oboo-clock/packages/

## How Can I Test my Changes?

The packages from this repo are build by the [Oboo-Source build system](https://github.com/ObooSmartClock/Oboo-Source). Follow the instructions from that repo to create an initial build.

Then:

1. Make your code changes in `Oboo-Source/feeds/oboo/` and recompile the affected packages
2. Take the new ipk file from `bin/packages/mipsel_24kc/oboo/` and install on your Oboo Smart Clock
3. Test out your new changes

## How Can I Contribute?

If you have a code change that you would like to share with everyone, please create a Pull Request and provide the following:

* **Use a clear and descriptive title** for the PR to identify the code change.
* **Provide a step-by-step description of the enhancement** in as many details as possible.
* **Describe the current behavior** and **explain which behavior you expect to see instead with your change** and why.
* **Increment** the `PKG_VERSION` or `PKG_RELEASE` for the affected package for changed to be properly propagated to other users
  * For example, my PR has changes to for `clock-mcu/src`, then I would also need to [increment `PKG_RELEASE` in `clock-mcu/Makefile`](https://github.com/ObooSmartClock/Oboo-Packages/blob/master/clock-mcu/Makefile#L11)
* Pull Requests must be **atomic**, that is, focus on a single improvement

## How is Code Released?

Once the PR is accepted in this repo, create a PR with a dummy change in the [Oboo-Source](https://github.com/ObooSmartClock/Oboo-Source) repo to trigger the CD system. 

It will build the packages and release them to: http://repo.getoboo.com/oboo-clock/packages/latest/
