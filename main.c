#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <gps.h>
#include <math.h>
#include <syslog.h>

#include "memory_utils.h"

#define _WAIT_GPS_ 2
#define _MIN_MOVE_ 1
#define _TIMEOUT_ 1

static void fix(struct gps_data_t *gpsdata)
{
    double move;
    static double moved = 0;
    static double time_saved = 0;
    static double lat_saved = 0, lon_saved = 0;
    static bool first = true;
    int speedkph;
    char speedstr[4] = "\0";
    char movedstr[6] = "\0";
    static int speed_saved = 0;

    if ((gpsdata->fix.time == time_saved) || gpsdata->fix.mode < MODE_2D)
        return;

    if (!first) {
        move = earth_distance(gpsdata->fix.latitude, gpsdata->fix.longitude,
                lat_saved, lon_saved);
        if (move < _MIN_MOVE_)
            return;
        moved += move;
        snprintf(movedstr, 5, "%f", moved/1000);
        send_to_server("[moved]", movedstr);
    }

    if (first) {
        first = false;
    }

    lat_saved = gpsdata->fix.latitude;
    lon_saved = gpsdata->fix.longitude;
    time_saved = gpsdata->fix.time;

    speedkph = round(gpsdata->fix.speed*3.6);

    if ((speedkph >= 0) && 
            (speedkph < 300) && 
            (speedkph != speed_saved)) {
        sprintf(speedstr, "%d", speedkph);
        send_to_server("[speed]", speedstr);
        speed_saved = speedkph;
    }
}

int main()
{
    int rc;
//    struct timeval tv;
    struct gps_data_t gps_data;

    while ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
        syslog(LOG_ERR, "%s: code: %d, reason: %s\n", __func__, rc, 
                gps_errstr(rc));
        sleep(_WAIT_GPS_);
    }

    gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
#if 1
    gps_mainloop(&gps_data, 5000000, fix);
    send_to_server("[gps]", "timeout");
#else
    while (1) {
        /* wait for 2 seconds to receive data */
        if (gps_waiting (&gps_data, 2000000)) {
            /* read data */
            if ((rc = gps_read(&gps_data)) == -1) {
                syslog(LOG_ERR, "%s: error occured reading gps data. code: %d, \
                        reason: %s\n", __func__, rc, gps_errstr(rc));
            } else {
                /* Display data from the GPS receiver. */
                if ((gps_data.status == STATUS_FIX) && 
                        (gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) &&
                        !isnan(gps_data.fix.latitude) && 
                        !isnan(gps_data.fix.longitude)) {
                    gettimeofday(&tv, NULL);

                    syslog(LOG_DEBUG, "%s: %f, %f, %f, %d", __func__, 
                            gps_data.fix.latitude, gps_data.fix.longitude,
                            gps_data.fix.speed, tv.tv_sec);

                    speedkph = round(gps_data.fix.speed*3.6);

                    if ((speedkph > 0) && 
                            (speedkph < 300) && 
                            (speedkph != speedsaved)) {
                        sprintf(speedstr, "%d", speedkph);
                        send_to_server("[speed]", speedstr);
                        speedsaved = speedkph;
                    }
                }/* else {
                    printf(".");
                }*/
            }
        }

//        sleep(1);
    }
#endif
    /* When you are done... */
    gps_stream(&gps_data, WATCH_DISABLE, NULL);
    gps_close (&gps_data);

    return EXIT_SUCCESS;
}
