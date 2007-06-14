ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib simpleclient

EXTRA_DIST = contrib

dist-hook:
	find $(distdir) -name ".svn" -type d -print0 | xargs -0 rm -rf
