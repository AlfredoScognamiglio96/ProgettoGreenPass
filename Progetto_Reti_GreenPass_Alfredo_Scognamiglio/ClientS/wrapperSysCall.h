#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>



ssize_t FullRead(int fd, void *buf, size_t count){
   size_t nleft;
   ssize_t nread;
   nleft = count;
   while (nleft > 0) {
   /* repeat until no left */
      if ( (nread = read(fd, buf, nleft)) < 0) {
         if (errno == EINTR) { /* if interrupted by system call */
            continue;
            /* repeat the loop */
         }else
         {
            exit(nread);
            /* otherwise exit */
         }
      }
      else if (nread == 0) { /* EOF */
         break;
         /* break loop here */
      }
      nleft -= nread;
      /* set left to read */
      buf +=nread;
      /* set pointer */
   }
   buf=0;
   return (nleft);
}


ssize_t FullWrite(int fd, const void *buf, size_t count){ 
   size_t nleft; 
   ssize_t nwritten; 
   nleft = count; 
   
   while (nleft > 0) {             /* repeat until no left */ 
      if(( nwritten = write(fd, buf, nleft) ) < 0){
	     if( errno == EINTR){
		    continue;
		 }
		 else{
		    perror("Fullwrite error"); 
		    exit(nwritten);
		 }
	  }
	  nleft -= nwritten;
	  buf += nwritten;
   }
       
   return (nleft); 
}

