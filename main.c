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

int main()
{
    int rc;
    struct timeval tv;
    int speedkph;
    char speedstr[3];

    struct gps_data_t gps_data;
    if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
        syslog(LOG_ERR, "%s: code: %d, reason: %s\n", __func__, rc, 
                gps_errstr(rc));
        return EXIT_FAILURE;
    }

    gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
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

                    if (speedkph > 0 && speedkph < 300) {
                        sprintf(speedstr, "%d", speedkph);
                        send_to_server("[speed]", speedstr);
                    }
                }/* else {
                    printf(".");
                }*/
            }
        }

        sleep(1);
    }

    /* When you are done... */
    gps_stream(&gps_data, WATCH_DISABLE, NULL);
    gps_close (&gps_data);

    return EXIT_SUCCESS;
}
