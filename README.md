Waitman Gobble
(510) 830-7975
ns@waitman.net

beenie - this program mounts SD/SDHC etc cards and dumps the contents.
it's a mess at the moment but seems to work.
(alpha version)

beenie is a daemon that uses camlib to see if you insert an usb drive (flash or memory stick), mounts the drive and launches a thread to start the copy process. The idea is to be able to stick 4 cards into the server and have it dump them. It may not work simultaneously (not yet tested). equipment is on the way and i'll check it out in a few weeks when I get a chance. It could tank the system to launch multiple copy processes, in that case we will have to get rid of the thread concept and do it one-by-one.

Anyhow..

here's an example of the log:

Sat, 19 Jan 2013 22:00:28 -0800: terminate signal
Sat, 19 Jan 2013 22:00:28 -0800: Shutting Down. Good-bye.
Sat, 19 Jan 2013 22:03:24 -0800: BURPLEX(C) WAITMAN GOBBLE
Sat, 19 Jan 2013 22:03:24 -0800: Hello! Starting beenie.
Sat, 19 Jan 2013 22:03:34 -0800: Loading: da3 cap 16025387008
Sat, 19 Jan 2013 22:03:35 -0800: Mounted /dev/da3s1 to /sdhc-da3
Sat, 19 Jan 2013 22:03:35 -0800: [13741366815] Begin Copy of /sdhc-da3
Sat, 19 Jan 2013 22:03:35 -0800: [14%/1913225689/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4579.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4579.MOV"
Sat, 19 Jan 2013 22:04:34 -0800: [15%/1942172506/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4580.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4580.MOV"
Sat, 19 Jan 2013 22:04:35 -0800: [24%/3196644131/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4581.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4581.MOV"
Sat, 19 Jan 2013 22:05:14 -0800: [31%/4241590072/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4582.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4582.MOV"
Sat, 19 Jan 2013 22:05:46 -0800: [45%/6105056585/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4583.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4583.MOV"
Sat, 19 Jan 2013 22:06:43 -0800: [72%/9860379158/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4584.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4584.MOV"
Sat, 19 Jan 2013 22:08:40 -0800: [74%/10089811711/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4585.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4585.MOV"
Sat, 19 Jan 2013 22:08:47 -0800: [74%/10136421804/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4586.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4586.MOV"
Sat, 19 Jan 2013 22:08:49 -0800: [74%/10165576221/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_4587.MOV" to "/u1/Comedy/2013-Jan-19/MVI_4587.MOV"
Sat, 19 Jan 2013 22:08:50 -0800: [99%/13545033422/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_5430.MOV" to "/u1/Comedy/2013-Jan-19/MVI_5430.MOV"
Sat, 19 Jan 2013 22:10:36 -0800: [100%/13741366815/13741366815] Copied "/sdhc-da3/DCIM/100CANON/MVI_5431.MOV" to "/u1/Comedy/2013-Jan-19/MVI_5431.MOV"
Sat, 19 Jan 2013 22:10:42 -0800: Completed Copy of /sdhc-da3
Sat, 19 Jan 2013 22:10:42 -0800: Unmounted /sdhc-da3


