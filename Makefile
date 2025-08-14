APP = sensor-dashboard
CFLAGS += `pkg-config --cflags gtk+-3.0` -O2 -Wall
LDLIBS += `pkg-config --libs gtk+-3.0`

all: $(APP)

$(APP): sensor_dashboard.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(APP) *.o
