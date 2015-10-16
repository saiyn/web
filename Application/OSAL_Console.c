/*
* Copyright (c) 2015, Hansong (Nanjing) Technology Ltd.
* All rights reserved.
* 
* ----File Info ----------------------------------------------- -----------------------------------------
* Name:     OSAL_Console.c
* Author:   saiyn.chen
* Date:     2015/4/17
* Summary:  this file define the system console function.


* Change Logs:
* Date           Author         Notes
* 2015-04-17    saiyn.chen     first implementation
*/

#include "common_head.h"
#include "OSAL_Console.h"
#include "Display_Task.h"
#include <string.h>

#ifdef CONFIG_USE_CONSOLE

extern unsigned char Load$$INIT_SECTION$$Base[];
extern unsigned char Load$$INIT_SECTION$$Length[];


int fn1(struct cmd_tbl_s *cmdtp, int argc, char * const argv[])
{
	   unsigned long i;
	   uint8 index=1;
	   cmd_tbl_t* pCmd;
	   unsigned long base = (unsigned long)Load$$INIT_SECTION$$Base;
     unsigned long len = (unsigned long)Load$$INIT_SECTION$$Length;
	
	   SYS_TRACE("\r\n-------------CMD List Table------------------\r\n");
	   for(i=0;i<len;index++)
     {
         pCmd = (cmd_tbl_t*)(base+i);
         if(pCmd->usage){
            SYS_TRACE("%d.%s          -%s\r\n", index,pCmd->name,pCmd->usage);
				 }

         i+=sizeof(cmd_tbl_t);
     }
	   SYS_TRACE("\r\n----------------------------------------------\r\n");
		 
	return 0;
}

OSAL_CMD(help, 1, fn1, "list current supported CMD");





void Init_all(unsigned long addr,unsigned long len)
{
     cmd_tbl_t* pCmd;
     unsigned long i;
     
     for(i=0;i<len;)
     {
         pCmd = (cmd_tbl_t*)(addr+i);
         if(strcmp(pCmd->name, "help") == 0){
              pCmd->cmd(NULL, 0, NULL);
				 }

         i+=sizeof(cmd_tbl_t);
     }
}

static cmd_tbl_t *find_cmd (const char *cmd)
{
  unsigned long base = (unsigned long)Load$$INIT_SECTION$$Base;
  unsigned long len = (unsigned long)Load$$INIT_SECTION$$Length;  
	
	cmd_tbl_t* pCmd;
	unsigned long i;
	
	for(i=0;i<len;){
		pCmd = (cmd_tbl_t*)(base+i);
		if(strcmp(pCmd->name, cmd) == 0){
			return pCmd;
		}
		
		i+=sizeof(cmd_tbl_t);
	}
	
	return NULL;
}


static cmd_err_t cmd_process(int argc, char * const argv[])
{
	 cmd_err_t result = CMD_NO_ERROR;
   cmd_tbl_t* pCmd;
	
	 /*first look up command in command table*/
	 pCmd = find_cmd(argv[0]);
	 if(!pCmd){
		 SYS_TRACE("\r\nUnknown cmd '%s'\r\n", argv[0]);
		 return CMD_UNKNOWN;
	 }
	 
	 /* found - check max args */
	 if(pCmd->maxargs < argc){
		 SYS_TRACE("Too many argv\r\n");
		 return CMD_PARAMETER_ERROR;
	 }
	 
	 pCmd->cmd(pCmd, argc, argv);
	 
	 return result;
}


static int parse_line(char *line, char *argv[])
{
	 int nargs = 0;
	
	 while(nargs < CONFIG_SYS_MAXARGS){
		  /* skip any white space */
		  while(is_blank(*line)){
				line++;
			}
			/*end of the line, no more args*/
			if(*line == '\0'){
				argv[nargs] = NULL;
				return nargs;
			}
			
			argv[nargs++] = line;	/* begin of argument string	*/
			
			/* find end of string */
			while(*line && !is_blank(*line)){
				line++;
			}
			
			if(*line == '\0'){
				argv[nargs] = NULL;
				return nargs;
			}
			
			*line++ = '\0';
	 }
	 
	 SYS_TRACE("** Too many args (max. %d) **\r\n", CONFIG_SYS_MAXARGS);
	 
	 return nargs;
}

cmd_err_t osal_run_command(char *cmd)
{
	 char *argv[CONFIG_SYS_MAXARGS + 1];	/* NULL terminated	*/
	 int argc;
	 cmd_err_t result;
	
	 if((argc = parse_line(cmd, argv)) == 0){
		 SYS_TRACE("no command at all\r\n");
		 return CMD_UNKNOWN;
	 }
	 
	 result = cmd_process(argc, argv);
   
	 return result;
}


#endif
