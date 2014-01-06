ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src
#SUBDIRS = src man doc

#SRC_WIN_DIST = \
#	src_win/build_installer.nsi \
#
#EXTRA_DIST = \
#	ROADMAP.txt \
#        m4/ax_boost_base.m4 \
#        m4/ax_boost_filesystem.m4 \
#        m4/ax_boost_system.m4 \
#        m4/ax_pthread.m4 \
#	$(SRC_WIN_DIST)

RELEASE	     = $(PACKAGE)-$(VERSION)

clean-local:
	rm -rf $(distdir).tar.gz
	rm -rf $(distdir).zip

