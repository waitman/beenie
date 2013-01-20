/*
BURPLEX(C) WAITMAN GOBBLE
clang -o beenie beenie.c
daemon that watches for SD/SDHC card inserts (/dev/da*), 
mounts filesystem then launches copy child process
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/disk.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/uio.h>

#include <time.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <camlib.h>
#include <cam/scsi/scsi_message.h>
#include <cam/scsi/scsi_pass.h>
#include <pthread.h>

#define RUNNING_DIR	"/var/run"
#define LOCK_FILE	"beenie.pid"
#define LOG_FILE	"/var/log/beenie"
#define MAX_NUM_DEV	100
#define MAX_THREADS	4
//we do not want to scan large drives, only SD/Flash type
//max limit is 5*16G = 80GB + 1
#define MAX_DRIVE_SIZE	80147906561 


struct src_parm {
	char *src;
	int tid;
};

void
build_iovec(struct iovec **iov, int *iovlen, const char *name, void *val,
	    size_t len)
{
	int i;

	if (*iovlen < 0)
		return;
	i = *iovlen;
	*iov = realloc(*iov, sizeof **iov * (i + 2));
	if (*iov == NULL) {
		*iovlen = -1;
		return;
	}
	(*iov)[i].iov_base = strdup(name);
	(*iov)[i].iov_len = strlen(name) + 1;
	i++;
	(*iov)[i].iov_base = val;
	if (len == (size_t)-1) {
		if (val != NULL)
			len = strlen(val) + 1;
		else
			len = 0;
	}
	(*iov)[i].iov_len = (int)len;
	*iovlen = ++i;
}

void 
log_message(filename,message)
char *filename;
char *message;
{
char outstr[200] = {0};
time_t t;
struct tm *tmp;
FILE *logfile;
	logfile=fopen(filename,"a");
	if(!logfile) return;
	t = time(NULL);
	tmp = localtime(&t);
	strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", tmp);

	fprintf(logfile,"%s: %s\n",outstr,message);
	fclose(logfile);
}

void 
*launch_copy(void *arg) 
{
        char str[200]={0};
        sprintf(str,"/x1/cpp/sddump %s",((struct src_parm*)arg)->src);
        system(str);
	sprintf(str,"Unmounted %s",((struct src_parm*)arg)->src);
	unmount(((struct src_parm*)arg)->src,0);
	log_message(LOG_FILE,str);
        pthread_exit(NULL);
}


void 
signal_handler(sig)
int sig;
{
	switch(sig) {
	case SIGHUP:
		log_message(LOG_FILE,"hangup signal");
		break;
	case SIGTERM:
		log_message(LOG_FILE,"terminate signal");
		log_message(LOG_FILE,"Shutting Down. Good-bye.");
		remove(LOCK_FILE);
		exit(0);
		break;
	}
}

void 
daemonize()
{
int i,lfp;
char str[10] = {0};
	if(getppid()==1) return; /* already a daemon */
	i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	chdir(RUNNING_DIR); /* change running directory */
	lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); /* can not open */
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
}

int 
main()
{

	daemonize();
	int rc,tnum,thread_j=0;
	pthread_t *threads;
	threads = malloc(MAX_THREADS * sizeof(*threads));
	

	log_message(LOG_FILE,"BURPLEX(C) WAITMAN GOBBLE");
	log_message(LOG_FILE,"Hello! Starting beenie.");

	off_t states[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	while(1) {
		sleep(10);
		//log_message(LOG_FILE,"running...");




	int fd;
	fd = open(XPT_DEVICE, O_RDWR);
	union ccb ccb;
	bzero(&ccb, sizeof(union ccb));
	ccb.ccb_h.path_id = CAM_XPT_PATH_ID;
	ccb.ccb_h.target_id = CAM_TARGET_WILDCARD;
	ccb.ccb_h.target_lun = CAM_LUN_WILDCARD;
	ccb.ccb_h.func_code = XPT_DEV_MATCH;

	int bufsize = sizeof(struct dev_match_result) * MAX_NUM_DEV;
	ccb.cdm.match_buf_len = bufsize;
	ccb.cdm.matches = (struct dev_match_result *)malloc(bufsize);
	ccb.cdm.num_matches = 0;
	ccb.cdm.num_patterns = 0;
	ccb.cdm.pattern_buf_len = 0;	
	int skip_device = 0, skip_bus = 0, changed = 0; 
	char devname[200] = {0};
  
  do {
    ioctl(fd, CAMIOCOMMAND, &ccb);
    for (unsigned i = 0; i < ccb.cdm.num_matches; i++) {
      struct bus_match_result *bus_result;
      struct device_match_result *dev_result;
      struct periph_match_result *periph_result;
      bus_result = &ccb.cdm.matches[i].result.bus_result;
      dev_result = &ccb.cdm.matches[i].result.device_result;
      periph_result =  &ccb.cdm.matches[i].result.periph_result;
if (strcmp(periph_result->periph_name,"da") == 0) { 

int chfd=0;
char dev_path[200] = {0};
off_t capacity=0;
sprintf(dev_path,"/dev/%s%ds1",periph_result->periph_name,periph_result->unit_number);
chfd=open(dev_path,O_RDONLY);
if (chfd>0) {
	ioctl(chfd,DIOCGMEDIASIZE,&capacity);
}
close(chfd);

if (capacity<MAX_DRIVE_SIZE) {
	if (states[periph_result->unit_number]!=capacity) {
	states[periph_result->unit_number]=capacity;
	if (capacity>0){
      sprintf(devname,"Loading: %s%d cap %ld", periph_result->periph_name, periph_result->unit_number,capacity);

log_message(LOG_FILE,devname);

char to_path[200] = {0};
sprintf(to_path,"/sdhc-%s%d",periph_result->periph_name,periph_result->unit_number);


int mountAttempts=0;
int status=0;
struct iovec *iov = NULL;
int iovlen = 0;
int mntflags = MNT_RDONLY;
char fstype[] = "msdosfs";
char errmsg[255] = {0};
build_iovec(&iov, &iovlen, "fstype", fstype, (size_t)-1);
build_iovec(&iov, &iovlen, "fspath", to_path, (size_t)-1);
build_iovec(&iov, &iovlen, "from", dev_path, (size_t)-1);
build_iovec(&iov, &iovlen, "errmsg", errmsg, sizeof(errmsg));


do
{
status = nmount(iov, iovlen, mntflags);

        if(status <0)
            sleep(1);
        mountAttempts++;
if (mountAttempts>1) { //for some reason the first attempt always fails(?)
	char errstr[200] = {0};
	sprintf(errstr,"Attempt %d mount %s failed.",mountAttempts,to_path);
	log_message(LOG_FILE,errstr);
}
}while(status <0 && mountAttempts < 5);

if (status==0) {
	char msg[200] = {0};
	sprintf(msg,"Mounted %s to %s",dev_path,to_path);
	log_message(LOG_FILE,msg);


//copy the files

tnum = periph_result->unit_number;

struct src_parm *srcpc;
srcpc = malloc(sizeof(*srcpc));
srcpc->tid = tnum;
strcpy(srcpc->src,to_path);
if ((rc=pthread_create(&threads[tnum],NULL,launch_copy,(void*)srcpc))) {
	log_message(LOG_FILE,"Error Creating Thread!");
}


} else {
	char msg[200] = {0};

if (errmsg[0]) {
	sprintf(msg,"Error - %s",errmsg);
} else {
	sprintf(msg,"Error - unk");
}
	log_message(LOG_FILE,msg);
}

	} else {
sprintf(devname,"Removed: %s%d", periph_result->periph_name, periph_result->unit_number);
log_message(LOG_FILE,devname);
	}
	}
}
}
      sprintf(devname,"");
   }
  } while ((ccb.ccb_h.status == CAM_REQ_CMP) && (ccb.cdm.status == CAM_DEV_MATCH_MORE));
  free(ccb.cdm.matches);
  close(fd);



	}

}

/* EOF */
