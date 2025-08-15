APP = sensor-dashboard
PKG = gtk+-3.0

override CFLAGS += $(shell pkg-config --cflags $(PKG)) -O2 -Wall
LDLIBS          += $(shell pkg-config --libs   $(PKG))

all: $(APP)

$(APP): sensor_dashboard.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(APP) *.o
