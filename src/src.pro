TEMPLATE = subdirs

SUBDIRS += application
SUBDIRS += library

# Build unit tests executable only on desktop platforms
!android:!ios: SUBDIRS += tests

application.depends = library
!android:!ios: tests.depends = library
