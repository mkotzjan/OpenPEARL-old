BEGIN{retval=0; limit=79}
length()>limit {printf("%s:%d line longer than %d characters (%d)\n",
                        FILENAME,NR,limit,length());
             retval=1;}

END{exit retval};

