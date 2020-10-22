EXTRA_DIST = autogen.sh editor4sh.desktop clipart3363684.png config.h README.md Makefile.tc
MYICON = clipart3363684.png
bin_PROGRAMS = editor4sh
editor4sh_SOURCES = editor4sh.cxx editor4sh.h

# fltk-config flags
AM_CXXFLAGS = @DOLLAR_SIGN@(shell fltk-config --cxxflags | sed 's@-I@-isystem @')
AM_LDFLAGS = @DOLLAR_SIGN@(shell fltk-config --ldflags)

AM_CXXFLAGS += -fno-rtti -fno-exceptions
AM_CXXFLAGS += -ffunction-sections -fdata-sections
AM_CXXFLAGS += -Wno-unused-parameter

# Silence this warning
AM_CXXFLAGS += -Wno-missing-field-initializers
# Fl_Text_Display::Style_Table_Entry::attr currently unused
# See the FLTK manual

# The icon
icondir = $(datadir)/pixmaps
icon_DATA = $(MYICON)

# The desktop files
desktopdir = $(datadir)/applications
desktop_DATA = $(bin_PROGRAMS).desktop

install-data-hook:
	cd $(DESTDIR)$(icondir) && \
	mv $(MYICON) $(bin_PROGRAMS).png
	cd $(DESTDIR)$(desktopdir) && \
	echo "X-FullPathIcon=$(icondir)/$(bin_PROGRAMS).png" >> $(DESTDIR)$(desktopdir)/$(desktop_DATA)
