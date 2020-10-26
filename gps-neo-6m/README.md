# GSP-NMEA-Data #


* $GPGSA - Detailed GSP DOP and detailed satellite tracking information (ed. individual satellite numbers)
* $GPGSV - Detailed GPS satellite information such as azimuth and elevation of each satellite being tracked
* $GPVTG - Speed over ground and tracking offset
* $GPGST - Estimated horizontal and vertical precision


[GPS - NMEA sentence information](https://www.gpsinformation.org/dale/nmea.htm)


## $GPGSA ##

$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:

* GSA - Satellite status
* A - Auto selection of 2D or 3D fix (M = manual)
* 3 - 3D fix -values include: 1=no-fix, 2 = 2D-fix, 3=3D-fix
* 04,05... - PRNs of satellites used for fix (room for 12)
* 2.5 - PDOP (dilution of precision)
* 1.3 - Horizontal dilution of precision (HDOP)
* 2.1 - Vertical dilution of precision (VDOP)
* "*39" - the checksum data, always begins with ("*")