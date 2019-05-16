#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define LKM_TEST_NAME "/dev/LKM_TEST"

static unsigned char buf[512];

int main(int argc, char *argv[])
{
	int fd, ret, i;

	fd = open(LKM_TEST_NAME, O_RDWR);
	if(fd < 0){
		perror("open device lkm_test failed\n");
		return -1;
	}

    for(i=0; i<sizeof(buf); i++){
        buf[i] = i;
    }

    ret = write(fd, buf, sizeof(buf));
    if(ret<0){
        printf("write lkm_test error\n");
    }else{
        ret = read(fd, buf, sizeof(buf));
        if(ret < 0){
            printf("read lkm_test error\n");
        }else{
            printf("read lkm_test done.\n");
            for(i=0; i<sizeof(buf); i++){
                if(i%32==0 && i!=0) printf("\n");
                printf("%02X ", buf[i]);
            }
            printf("\n");
        }
    }
    release(LKM_TEST_NAME, 0);

	close(fd);
	return 0;
}
