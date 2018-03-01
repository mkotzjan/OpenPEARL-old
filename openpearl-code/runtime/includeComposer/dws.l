%%
.	ECHO;
\t\t 	fputs(" ",yyout); // remove multiple tabs
\r\n	fputs("\n",yyout);   // remove ^M of MSDOS file format
[ \t]*[\n]+	fputs("\n",yyout);
%%
